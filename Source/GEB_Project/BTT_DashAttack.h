// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Delegates/Delegate.h"
#include "BTT_DashAttack.generated.h"


class UBehaviorTreeComponent;

UCLASS()
class GEB_PROJECT_API UBTT_DashAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	UAnimMontage* DashMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;
	
	UBTT_DashAttack();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackStat", meta = (AllowPrivateAccess = "true"))
	int32 damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackStat", meta = (AllowPrivateAccess = "true"))
	float maxAttackCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackStat", meta = (AllowPrivateAccess = "true"))
	float attackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackStat", meta = (AllowPrivateAccess = "true"))
	bool isCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackStat", meta = (AllowPrivateAccess = "true"))
	float coolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, CateGory = "DashAttack", meta = (AllowPrivateAccess = "true"))
	float DashVelocity;

	UPROPERTY(EditAnywhere, Category = "DashAttack", meta = (AllowPrivateAccess = "true"))
	float MontagePlayRate = 1.0f;
	
	UPROPERTY()
	UBehaviorTreeComponent* MyOwnerComp; // 델리게이트 콜백에서 태스크 종료를 위해 컴포넌트 포인터를 저장하는 변수

	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AActor> WarningActorClass;

	UPROPERTY(EditAnywhere, Category = "AI")
	float WarningDelayTime = 0.4f;

	// 마찰력 복구를 위한 변수
	float OriginalGroundFriction;
	float OriginalBrakingDeceleration;
protected:
	// Call when Task Start
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UFUNCTION()
	void PlayMontage(ACharacter* TargetCharacter, UAnimMontage* Montage, float PlayRate);
	FTimerHandle DashDelayHandle;

	void DoDash(ACharacter* AICharacter, FVector LaunchVelocity);

public:
	bool GetisCoolDown();
	float GetattackRange();

private:
	FVector SavedLaunchVelocity; // 콜백 함수에서 사용하기 위한 변수
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
