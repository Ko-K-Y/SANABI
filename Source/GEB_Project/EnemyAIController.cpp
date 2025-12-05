// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "EnemyMoveComponent.h"
#include "AttackComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "PatrolRoute.h"



AEnemyAIController::AEnemyAIController()
{
    // Behavior Tree �� Blackboard ������Ʈ ����
    BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

    // Perception ������Ʈ �� �þ� ���� ����
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    SightConfig->SightRadius = 1500.f;
    SightConfig->LoseSightRadius = 1700.f;
    SightConfig->PeripheralVisionAngleDegrees = 90.f;
    SightConfig->SetMaxAge(50.f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    PerceptionComp->ConfigureSense(*SightConfig);
    PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetUpdated);
}


void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // Behavior Tree ����
    if (BehaviorTree)
    {
        BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
        BehaviorComp->StartTree(*BehaviorTree);
    }

    // ������Ʈ ĳ��
    AttackComp = InPawn ? InPawn->FindComponentByClass<UAttackComponent>() : nullptr;
    MoveComp = InPawn ? InPawn->FindComponentByClass<UEnemyMoveComponent>() : nullptr;

    if (MoveComp && SightConfig) {
        float traceRange = IEnemyMove::Execute_GettraceRange(MoveComp);
        SightConfig->LoseSightRadius = traceRange + 200.f;
        PerceptionComp->RequestStimuliListenerUpdate();
    }

    if (MoveComp) {
        if (MoveComp->AssignedRoute) {
			Spline = MoveComp->AssignedRoute->PatrolSpline;
        }
    }
}

void AEnemyAIController::OnTargetUpdated(AActor* Target, FAIStimulus Stimulus)
{
    if (!Target || !AttackComp || !MoveComp) return;
    if (!Target->GetInstigatorController()->IsPlayerController()) { return; }

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    //if (Stimulus.WasSuccessfullySensed()){
    //    SetFocus(Target); // AI�� Ÿ���� �ٶ󺸰� ��
    //}
    //else{
    //    ClearFocus(EAIFocusPriority::Gameplay); // �������� ���ϸ� ��Ŀ�� ����
    //}

    // �÷��̾���� �Ÿ� ���
    float Distance = FVector::Dist(Target->GetActorLocation(), ControlledPawn->GetActorLocation());

    // BlackBoard Ű�� ��� ����
    BlackboardComp->SetValueAsObject("Target", Target);

    // ���� �� ���� ���� ����
    bool bInTraceRange = Distance >= IAttack::Execute_GetattackRange(AttackComp) && Distance <= IEnemyMove::Execute_GettraceRange(MoveComp);
    bool bInAttackRange = Distance <= IAttack::Execute_GetattackRange(AttackComp);

    BlackboardComp->SetValueAsBool("IsInTraceRange", bInTraceRange);
    BlackboardComp->SetValueAsBool("IsInAttackRange", bInAttackRange);
}