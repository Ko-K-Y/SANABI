// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ADirectionsLighting.generated.h"

class UBoxComponent;
class UPointLightComponent;

UCLASS()
class GEB_PROJECT_API AADirectionsLighting : public AActor
{
	GENERATED_BODY()

public:
	AADirectionsLighting();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	// === Components ===
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPointLightComponent* PointLight;

	// === Runtime state ===
	int32 PlayerOverlapCount = 0;

	// === Settings ===
	UPROPERTY(EditAnywhere, Category = "Light")
	float OnIntensity = 5000.f;

	// 플레이어 조종 중인 Pawn만 반응할지 여부
	UPROPERTY(EditAnywhere, Category = "Light")
	bool bOnlyPlayerControlled = true;

	// === Handlers ===
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateLightState();
};
