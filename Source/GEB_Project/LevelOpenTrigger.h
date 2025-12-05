#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelOpenTrigger.generated.h"

class UBoxComponent;

UCLASS()
class GEB_PROJECT_API ALevelOpenTrigger : public AActor
{
	GENERATED_BODY()

public:
	ALevelOpenTrigger();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TriggerBox;

	bool bTriggered = false;

	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	UPROPERTY(EditAnywhere, Category = "Level")
	FName LevelToOpen = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Level")
	bool bOneShot = true;

	UPROPERTY(EditAnywhere, Category = "Level")
	bool bOnlyPlayer = true;
};
