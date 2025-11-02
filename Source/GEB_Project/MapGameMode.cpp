#include "MapGameMode.h"
#include "SavePointSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"

AMapGameMode::AMapGameMode()
{
	// 기본 설정은 필요시 조정
}

void AMapGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// 보통 Super가 기본 스폰 로직을 수행
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// 첫 스폰에도 세이브 포인트를 적용하고 싶을 때만 수행
	if (bUseSavePointForInitialSpawn && IsValid(NewPlayer))
	{
		FTransform SaveXform;
		if (TryGetSaveSpawnTransform(SaveXform))
		{
			// 기본 스폰을 한 번 수행했더라도, 세이브 포인트가 있다면 해당 위치에서 다시 시작시킴
			RestartPlayerAtTransform(NewPlayer, SaveXform);
		}
	}
}

void AMapGameMode::RestartPlayer(AController* NewPlayer)
{
	if (!IsValid(NewPlayer))
	{
		Super::RestartPlayer(NewPlayer);
		return;
	}

	FTransform SaveXform;
	if (TryGetSaveSpawnTransform(SaveXform))
	{
		// 세이브 포인트가 있으면 해당 위치에서 리스타트
		RestartPlayerAtTransform(NewPlayer, SaveXform);
	}
	else
	{
		// 없으면 기존 규칙대로
		Super::RestartPlayer(NewPlayer);
	}
}

bool AMapGameMode::TryGetSaveSpawnTransform(FTransform& OutTransform) const
{
	UWorld* World = GetWorld();
	if (!World) return false;

	const USavePointSubsystem* SP = World->GetSubsystem<USavePointSubsystem>();
	if (!SP || !SP->HasSavePoint())
	{
		return false;
	}

	FTransform T = SP->GetSavePoint();
	if (bSnapToFloor)
	{
		T = ApplyFloorSnap(T);
	}
	OutTransform = T;
	return true;
}

FTransform AMapGameMode::ApplyFloorSnap(const FTransform& InTransform) const
{
	UWorld* World = GetWorld();
	if (!World) return InTransform;

	const FVector BaseLoc = InTransform.GetLocation();
	const FVector Start = BaseLoc + FVector(0.f, 0.f, FloorTraceDistance * 0.5f);
	const FVector End = BaseLoc - FVector(0.f, 0.f, FloorTraceDistance);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SavePointFloorSnap), /*bTraceComplex*/false);
	Params.bReturnPhysicalMaterial = false;

	// 지형/월드 정적 오브젝트를 대상으로 바닥 검출
	const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	// 디버그 확인이 필요하면 주석 해제
	// DrawDebugLine(World, Start, End, FColor::Green, false, 1.5f, 0, 1.0f);

	if (bHit)
	{
		FTransform Out = InTransform;
		Out.SetLocation(Hit.ImpactPoint + FVector(0.f, 0.f, FloorOffset));
		// 회전/스케일은 원래 값 유지
		return Out;
	}
	return InTransform;
}
