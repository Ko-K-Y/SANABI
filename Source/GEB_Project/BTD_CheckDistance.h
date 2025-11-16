// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_CheckDistance.generated.h"

/**
 * 
 */
UCLASS()
class GEB_PROJECT_API UBTD_CheckDistance : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_CheckDistance();

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	// 비교에 사용할 거리(블루프린트에서 노출)
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "BlackBoard")
	FBlackboardKeySelector DistanceKey;


};
