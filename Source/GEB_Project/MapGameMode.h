#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapGameMode.generated.h"

/**
 * SavePointSubsystem에 기록된 Transform이 있으면 그 위치에서 스폰/리스폰.
 * ChoosePlayerStart 단계에서 저장 위치를 PlayerStart로 강제 반환하여 안정적으로 반영.
 */
UCLASS()
class GEB_PROJECT_API AMapGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMapGameMode();

	/** 리스폰 시 저장 위치가 있으면 그 위치에서 재시작 */
	virtual void RestartPlayer(AController* NewPlayer) override;

	/** 저장 위치를 PlayerStart로 반환 (엔진의 스폰 위치 결정 단계에서 개입) */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
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
	/** 저장 위치용 임시 PlayerStart를 캐시 (필요 시 생성/재활용) */
	UPROPERTY()
	class APlayerStart* CachedSaveStart = nullptr;

	/** 서브시스템에서 세이브 스폰 Transform을 얻어오고, 필요하면 바닥 스냅 적용 */
	bool TryGetSaveSpawnTransform(FTransform& OutTransform) const;

	/** 바닥 방향 라인트레이스로 안전한 위치로 보정 */
	FTransform ApplyFloorSnap(const FTransform& InTransform) const;

	/** 저장용 PlayerStart를 리턴(없으면 생성) */
	AActor* GetOrCreateSavePlayerStart();

	/** 저장용 PlayerStart의 트랜스폼을 최신 저장 위치로 갱신 */
	void EnsureSaveStartAt(const FTransform& SaveXform);
};
