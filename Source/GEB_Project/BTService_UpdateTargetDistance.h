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

    // Blackboard key selectors (연결하지 않으면 기본 문자열 키 사용)
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IsInAttackRangeKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IsInTraceRangeKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector CanAttackKey;
};