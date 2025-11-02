#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapGameMode.generated.h"

/**
 * SavePointSubsystem에 기록된 Transform이 있으면 그 위치에서 스폰/리스폰.
 * 없으면 기존 PlayerStart 규칙을 사용.
 */
UCLASS()
class GEB_PROJECT_API AMapGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMapGameMode();

	/** 사망/리스폰 등으로 호출될 때 세이브 포인트가 있으면 그 위치에서 재시작 */
	virtual void RestartPlayer(AController* NewPlayer) override;

	/** 첫 입장(처음 스폰) 시에도 세이브 포인트를 적용하고 싶으면 true */
	UPROPERTY(EditAnywhere, Category = "SavePoint")
	bool bUseSavePointForInitialSpawn = true;

protected:
	/** 첫 스폰 시점(플레이어 입장)에 세이브 포인트 적용 옵션 */
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	/** 바닥에 살짝 스냅해서 안전한 위치로 보정할지 */
	UPROPERTY(EditAnywhere, Category = "SavePoint|Snap")
	bool bSnapToFloor = true;

	/** 위/아래로 최대 몇 cm까지 바닥 트레이스를 시도할지 */
	UPROPERTY(EditAnywhere, Category = "SavePoint|Snap", meta = (EditCondition = "bSnapToFloor", ClampMin = "0.0"))
	float FloorTraceDistance = 2000.f;

	/** 바닥과 살짝 띄우는 오프셋(cm) */
	UPROPERTY(EditAnywhere, Category = "SavePoint|Snap", meta = (EditCondition = "bSnapToFloor"))
	float FloorOffset = 5.f;

private:
	/** 서브시스템에서 세이브 스폰 Transform을 얻어오고, 필요하면 바닥 스냅 적용 */
	bool TryGetSaveSpawnTransform(FTransform& OutTransform) const;

	/** 바닥 방향 라인트레이스로 안전한 위치로 보정 */
	FTransform ApplyFloorSnap(const FTransform& InTransform) const;
};
