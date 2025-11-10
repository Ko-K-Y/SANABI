// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DashAttack.generated.h"

/**
 * 
 */
UCLASS()
class GEB_PROJECT_API UBTT_DashAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_DashAttack();
	
protected:
	EBTNodeResult::Type ExcuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	FVector StartLocation; // Boss Location
	FVector TargetLocation; // Player Location

	class AAIController* AIController;
	class APawn* AIPawn;

	bool bIsDashing = false; // 대쉬 공격 중인지 확인

	void PerformDash(float DeltaTime); // 대쉬 공격 로직 구현
};
