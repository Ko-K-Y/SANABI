#include "MapGameMode.h"
#include "SavePointSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"            // GEngine
#include "GameFramework/Pawn.h"

AMapGameMode::AMapGameMode()
{
	// 기본값 유지
}

void AMapGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (bUseSavePointForInitialSpawn && IsValid(NewPlayer))
	{
		FTransform SaveXform;
		if (TryGetSaveSpawnTransform(SaveXform))
		{
			PrintSpawnLocation(SaveXform, TEXT("InitialSpawn"));
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
		PrintSpawnLocation(SaveXform, TEXT("RestartPlayer"));
		RestartPlayerAtTransform(NewPlayer, SaveXform);
	}
	else
	{
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

	OutTransform = SP->GetSavePoint(); // 바닥 스냅/보정 없이 그대로 사용
	return true;
}

void AMapGameMode::PrintSpawnLocation(const FTransform& T, const TCHAR* Context) const
{
	const FVector L = T.GetLocation();
	const FRotator R = T.Rotator();

	// 화면 출력
	if (GEngine)
	{
		const FString Msg = FString::Printf(
			TEXT("[%s] Respawn to Location=%s  Rotation=%s"),
			Context,
			*L.ToString(), *R.ToString()
		);
		GEngine->AddOnScreenDebugMessage(
			/*Key*/-1, /*Time*/2.5f, FColor::Cyan, Msg
		);
	}

	// 로그 출력
	UE_LOG(LogTemp, Log, TEXT("[%s] Respawn to Location=%s  Rotation=%s"),
		Context, *L.ToString(), *R.ToString());
}
