#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapGameMode.generated.h"

/**
 * SavePointSubsystem의 Transform을 그대로 사용해 스폰/리스폰.
 * 이동 위치는 화면/로그로 출력.
 */
UCLASS()
class GEB_PROJECT_API AMapGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMapGameMode();

	/** 사망/리스폰 시 세이브 포인트가 있으면 그 위치로 재시작 */
	virtual void RestartPlayer(AController* NewPlayer) override;

	/** 첫 스폰에도 세이브 포인트 적용할지 */
	UPROPERTY(EditAnywhere, Category = "SavePoint")
	bool bUseSavePointForInitialSpawn = true;

protected:
	/** 첫 입장 시에도 세이브 포인트 적용(옵션) */
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

private:
	/** 서브시스템에서 세이브 Transform을 얻는다 (바닥 스냅 없음) */
	bool TryGetSaveSpawnTransform(FTransform& OutTransform) const;

	/** 이동하는 위치를 화면/로그로 출력 */
	void PrintSpawnLocation(const FTransform& T, const TCHAR* Context) const;
};
