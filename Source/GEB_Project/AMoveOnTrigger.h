#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AMoveOnTrigger.generated.h"

class UBoxComponent;

UCLASS()
class GEB_PROJECT_API AAMoveOnTrigger : public AActor
{
	GENERATED_BODY()

public:
	AAMoveOnTrigger();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	// === Components ===
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Trigger;

	// === Settings ===
	// 초당 이동 거리 (유닛/초)
	UPROPERTY(EditAnywhere, Category = "Move")
	float MoveSpeed = 100.f;

	// 현재 위치에서 상대적으로 이동할 거리 (예: X로 500만큼 이동하려면 (500,0,0))
	UPROPERTY(EditAnywhere, Category = "Move")
	FVector MoveOffset = FVector(0.f, 0.f, -500.f);

	// === Internal State ===
	bool bTriggered = false;      // 한 번만 동작
	bool bMoving = false;
	FVector StartLocation;
	FVector TargetLocation;

private:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
