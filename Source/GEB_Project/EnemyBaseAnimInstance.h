// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyBaseAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EAnimState : uint8 {
	Locomotion,
	Attack,
	Hit,
	Die
};

UCLASS()
class GEB_PROJECT_API UEnemyBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Params")
	EAnimState State;
	UFUNCTION(BlueprintCallable)
	void OnStateAnimationEnds();

	virtual void SetAnimStateAttack();
	virtual void SetAnimStateHit();
	virtual void SetAnimStateDie();
};
