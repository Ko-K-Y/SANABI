// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAttackComponent;
class UEnemyMoveComponent;

UCLASS()
class GEB_PROJECT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
    AEnemyAIController();

    UPROPERTY()
    class USplineComponent* Spline;

    UPROPERTY()
    int32 CurrentSplinePointIndex;

protected:
    virtual void OnPossess(APawn* InPawn) override;

    UFUNCTION()
    void OnTargetUpdated(AActor* Target, FAIStimulus Stimulus);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    UAIPerceptionComponent* PerceptionComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    UAISenseConfig_Sight* SightConfig;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    UBehaviorTreeComponent* BehaviorComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    UBlackboardComponent* BlackboardComp;

    UAttackComponent* AttackComp;
    UEnemyMoveComponent* MoveComp;

public:
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTree;

    static const FName KeyTargetActor;
    static const FName KeyIsInAttackRange;
    static const FName KeyIsInTraceRange;
};


