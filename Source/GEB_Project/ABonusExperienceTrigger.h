#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABonusExperienceTrigger.generated.h"

class UBoxComponent;

UCLASS()
class GEB_PROJECT_API AABonusExperienceTrigger : public AActor
{
	GENERATED_BODY()

public:
	AABonusExperienceTrigger();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Reward")
	int32 ExpAmount = 100;

private:
	bool bConsumed = false;

	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
