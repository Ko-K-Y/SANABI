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

	// === Settings ===
	UPROPERTY(EditAnywhere, Category = "Light")
	float OnIntensity = 5000.f;

	// Trigger 진입 시 최소 몇 초간은 반드시 켜두기
	UPROPERTY(EditAnywhere, Category = "Light|Timing")
	float MinOnDuration = 1.0f; // 요구사항: 1초

	// Trigger 해제 후 서서히 꺼지는 시간
	UPROPERTY(EditAnywhere, Category = "Light|Timing")
	float FadeOutDuration = 1.0f; // 요구사항: 1초

	// 플레이어만 반응할지 여부
	UPROPERTY(EditAnywhere, Category = "Light")
	bool bOnlyPlayerControlled = true;

	// === Runtime state ===
	int32 OverlapCount = 0;

	// 페이드 제어
	bool bIsFadingOut = false;
	bool bPendingFade = false;      // 최소 on 시간 끝나길 대기 중
	float FadeStartTime = 0.f;
	float EarliestFadeTime = 0.f;   // 이 시각 전에는 꺼지지 않음

	// === Handlers ===
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// === Helpers ===
	void StartFadeOut(float StartTime);
	void CancelFadeOut();
	void TurnOnHard();
};
