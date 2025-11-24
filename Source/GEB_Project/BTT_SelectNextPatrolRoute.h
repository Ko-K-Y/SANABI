// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SelectNextPatrolRoute.generated.h"

/**
 * 
 */
UCLASS()
class GEB_PROJECT_API UBTT_SelectNextPatrolRoute : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_SelectNextPatrolRoute();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere,Category = "Blackboard")
	FBlackboardKeySelector NextLocationKey;
	
};
