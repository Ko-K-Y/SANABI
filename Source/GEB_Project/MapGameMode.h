#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapGameMode.generated.h"

/**
 * 저장된 세이브 포인트가 있으면 그 위치에서 스폰/리스폰.
 * - ChoosePlayerStart 단계에서 임시 PlayerStart를 반환해 엔진 스폰 시점부터 강제
 * - SpawnDefaultPawnAtTransform에서 AlwaysSpawn로 충돌 보정
 * - 바닥 스냅 시 캡슐 절반 높이만큼 올려 충돌 회피
 */
UCLASS()
class GEB_PROJECT_API AMapGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMapGameMode();

	/** 리스폰 진입점 */
	virtual void RestartPlayer(AController* NewPlayer) override;

	/** 스폰 위치 선택 단계에서 세이브 포인트 강제 */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** 실제 폰 스폰 정책을 충돌 조정+항상 스폰으로 */
	virtual APawn* SpawnDefaultPawnAtTransform(AController* NewPlayer, const FTransform& SpawnTransform) override;

protected:
	/** 바닥 스냅 사용 여부 */
	UPROPERTY(EditAnywhere, Category = "SavePoint|Snap")
	bool bSnapToFloor = true;

	/** 위/아래로 바닥 탐색 거리 */
	UPROPERTY(EditAnywhere, Category = "SavePoint|Snap", meta = (EditCondition = "bSnapToFloor", ClampMin = "0.0"))
	float FloorTraceDistance = 2000.f;

	/** 발바닥에서 띄우는 여유 오프셋(cm) */
	UPROPERTY(EditAnywhere, Category = "SavePoint|Snap", meta = (EditCondition = "bSnapToFloor"))
	float FloorOffset = 5.f;

private:
	/** 런타임에 스폰하는 전용 PlayerStart (레벨의 기본 PlayerStart는 건드리지 않음) */
	UPROPERTY()
	class APlayerStart* CachedSaveStart = nullptr;

	/** GISubsystem에서 세이브 스폰 Transform 획득(+바닥 스냅 적용) */
	bool TryGetSaveSpawnTransform(FTransform& OutTransform) const;

	/** 바닥 스냅(Z = Impact + HalfHeight + Offset) */
	FTransform ApplyFloorSnap(const FTransform& InTransform) const;

	/** 전용 PlayerStart를 생성/확보 (항상 런타임 스폰) */
	AActor* GetOrCreateSavePlayerStart();

	/** 전용 PlayerStart를 저장 위치로 이동 */
	void EnsureSaveStartAt(const FTransform& SaveXform);

	/** 기본 Pawn(캐릭터) CDO에서 캡슐 절반 높이 추출 */
	float GetDefaultPawnCapsuleHalfHeight() const;
};
