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

	// 타이머 관리를 위한 핸들
	FTimerHandle LevelLoadTimerHandle;

	// 실제 레벨 로딩을 수행할 함수 (타이머 종료 후 호출)
	void LoadLevel();

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

	// 페이드 아웃 시간 (기본값 0.5초)
	UPROPERTY(EditAnywhere, Category = "Level")
	float FadeDuration = 0.5f;
};