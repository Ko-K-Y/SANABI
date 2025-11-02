#include "MapGameMode.h"
#include "SavePointSubsystem.h"

#include "Engine/World.h"
#include "EngineUtils.h"                 // TActorIterator
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"

AMapGameMode::AMapGameMode()
{
	// 기본값은 UPROPERTY에서 설정
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
		// 저장 위치에서 리스타트(엔진 한계 케이스를 대비해 PlayerStart도 옮겨둔다)
		EnsureSaveStartAt(SaveXform);
		RestartPlayerAtTransform(NewPlayer, SaveXform);
	}
	else
	{
		Super::RestartPlayer(NewPlayer);
	}
}

AActor* AMapGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	FTransform SaveXform;
	if (TryGetSaveSpawnTransform(SaveXform))
	{
		// 엔진이 스폰 위치를 고르는 '바로 그 순간'에 저장 위치를 PlayerStart로 반환
		EnsureSaveStartAt(SaveXform);
		return CachedSaveStart;
	}

	// 저장 지점이 없으면 기존 규칙대로
	return Super::ChoosePlayerStart_Implementation(Player);
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

	const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	// 필요하면 확인용 디버그 라인
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

AActor* AMapGameMode::GetOrCreateSavePlayerStart()
{
	

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	// 1) 기존 PlayerStart 재활용 시도 (레벨에 하나라도 있으면 잡아씀)
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		CachedSaveStart = *It;
		break;
	}

	// 2) 없으면 새로 스폰
	if (!CachedSaveStart)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CachedSaveStart = World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), FTransform::Identity, Params);
	}

	return CachedSaveStart;
}

void AMapGameMode::EnsureSaveStartAt(const FTransform& SaveXform)
{
	AActor* PS = GetOrCreateSavePlayerStart();
	if (PS)
	{
		PS->SetActorTransform(SaveXform);
	}
}
