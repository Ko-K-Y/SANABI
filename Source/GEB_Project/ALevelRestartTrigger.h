#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALevelRestartTrigger.generated.h"

class UBoxComponent;

/**
 * 플레이어가 Trigger에 닿으면 현재 레벨을 다시 시작하는 트리거 액터
 */
UCLASS()
class GEB_PROJECT_API AALevelRestartTrigger : public AActor
{
	GENERATED_BODY()

public:
	AALevelRestartTrigger();

protected:
	virtual void BeginPlay() override;

private:
	/** Overlap을 감지할 트리거 박스 (Root) */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Trigger;

	/** 플레이어만 반응할지 여부 */
	UPROPERTY(EditAnywhere, Category = "Restart")
	bool bOnlyPlayerControlled = true;

	/** 트리거 후 레벨 재시작까지 지연(초). 0이면 즉시 재시작 */
	UPROPERTY(EditAnywhere, Category = "Restart", meta = (ClampMin = "0.0"))
	float RestartDelay = 0.0f;

	/** 한 번 트리거되면 더 이상 동작하지 않음 */
	UPROPERTY(EditAnywhere, Category = "Restart")
	bool bTriggerOnce = true;

	/** 이미 트리거되었는지(중복 실행 방지) */
	bool bAlreadyTriggered = false;

	FTimerHandle RestartTimerHandle;

	/** BeginOverlap 핸들러 */
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	/** 레벨 재시작 수행 */
	void DoRestartLevel();

	/** (멀티플레이 대비) 서버에 재시작 요청 */
	UFUNCTION(Server, Reliable)
	void ServerRequestRestart();
	void ServerRequestRestart_Implementation();
};
