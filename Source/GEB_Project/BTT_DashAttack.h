// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "BTT_DashAttack.generated.h"

/**
 * 
 */
UCLASS()
class GEB_PROJECT_API UBTT_DashAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
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
	
protected:
	// Call when Task Start
	EBTNodeResult::Type ExcuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	// Task가 완료될 때까지 실행되는 틱 함수
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
private:
	FVector StartLocation; // Boss Location
	FVector TargetLocation; // Player Location

	class AAIController* AIController;
	class APawn* AIPawn;
	bool bIsDashing = false; // 대쉬 공격 중인지 확인
	void PerformDash(float DeltaTime); // 대쉬 공격 로직 구현


	// *** Montage Play
private:
	bool bMontagePlayed;
protected:
	void PlayMontage(UAnimMontage* Montage, float PlaySpeed);

public:
	bool GetisCoolDown();
	float GetattackRange();
};
