#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateTargetDistance.generated.h"

UCLASS()
class GEB_PROJECT_API UBTService_UpdateTargetDistance : public UBTService
{
    GENERATED_BODY()

public:
    UBTService_UpdateTargetDistance();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    // Blackboard key selectors (�������� ������ �⺻ ���ڿ� Ű ���)
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IsInAttackRangeKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IsInTraceRangeKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector CanAttackKey;
    
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector Distance_MoveKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector Distance_AttackKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector Distance_PunchKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IsDashingKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IsStunnedKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IsPhaseTwoKey;
    
};