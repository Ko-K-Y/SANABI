#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASavePointTrigger.generated.h"

class UBoxComponent;
class USceneComponent;

/** 플레이어가 닿으면 현재 위치/회전을 세이브 포인트로 기록하는 트리거 */
UCLASS()
class GEB_PROJECT_API AASavePointTrigger : public AActor
{
	GENERATED_BODY()

public:
	AASavePointTrigger();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Trigger;

	/** 저장 기준점(없으면 액터 Transform 사용). 위치/회전 미세조정용 */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SavePoint;

	UPROPERTY(EditAnywhere, Category = "SavePoint")
	bool bOnlyPlayerControlled = true;

	UPROPERTY(EditAnywhere, Category = "SavePoint")
	bool bTriggerOnce = false;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugLog = true;

	bool bAlreadyTriggered = false;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	FTransform GetSaveTransform() const;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "SavePoint")
	void OnSavePointActivated(const FTransform& SavedTransform);
};
