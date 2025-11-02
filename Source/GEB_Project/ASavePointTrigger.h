#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASavePointTrigger.generated.h"

class UBoxComponent;
class USceneComponent;

/**
 * 플레이어가 트리거에 닿으면 SavePointSubsystem에 '이 위치'를 세이브 포인트로 기록
 */
UCLASS()
class GEB_PROJECT_API AASavePointTrigger : public AActor
{
	GENERATED_BODY()

public:
	AASavePointTrigger();

protected:
	virtual void BeginPlay() override;

private:
	/** Overlap 감지를 위한 트리거 박스 (Root) */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Trigger;

	/** 실제 저장할 Transform 기준점 (회전/높이 미세조정용). 기본은 Actor Transform 사용 */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SavePoint;

	/** 플레이어만 반응할지 여부 */
	UPROPERTY(EditAnywhere, Category = "SavePoint")
	bool bOnlyPlayerControlled = true;

	/** 한 번 세이브한 뒤에는 다시 발동하지 않도록 할지 여부 */
	UPROPERTY(EditAnywhere, Category = "SavePoint")
	bool bTriggerOnce = false;

	/** 이미 발동했는지 (중복 방지용) */
	bool bAlreadyTriggered = false;

	/** 디버그 메시지/그림 표시 */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugLog = true;

	/** Overlap Begin */
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	/** 저장할 최종 트랜스폼 계산 (SavePoint가 있으면 그걸, 없으면 Actor Transform) */
	FTransform GetSaveTransform() const;

public:
	/** 블루프린트에서 후처리(UI/SFX 등) 붙일 수 있게 이벤트 노출 */
	UFUNCTION(BlueprintImplementableEvent, Category = "SavePoint")
	void OnSavePointActivated(const FTransform& SavedTransform);
};
