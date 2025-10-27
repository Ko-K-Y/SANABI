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



AEnemyAIController::AEnemyAIController()
{
    // Behavior Tree 및 Blackboard 컴포넌트 생성
    BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

    // Perception 컴포넌트 및 시야 감각 설정
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    SightConfig->SightRadius = 1500.f;
    SightConfig->LoseSightRadius = 1700.f;
    SightConfig->PeripheralVisionAngleDegrees = 90.f;
    SightConfig->SetMaxAge(0.5f);
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

    // Behavior Tree 시작
    if (BehaviorTree)
    {
        BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
        BehaviorComp->StartTree(*BehaviorTree);
    }

    // 컴포넌트 캐싱
    AttackComp = InPawn ? InPawn->FindComponentByClass<UAttackComponent>() : nullptr;
    MoveComp = InPawn ? InPawn->FindComponentByClass<UEnemyMoveComponent>() : nullptr;

    if (MoveComp && SightConfig) {
        float traceRange = IEnemyMove::Execute_GettraceRange(MoveComp);
        SightConfig->LoseSightRadius = traceRange + 200.f;
        PerceptionComp->RequestStimuliListenerUpdate();
    }
}

void AEnemyAIController::OnTargetUpdated(AActor* Target, FAIStimulus Stimulus)
{
    if (!Target || !AttackComp || !MoveComp) return;

    if (!Target->GetInstigatorController()->IsPlayerController()) { return; }

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    //if (Stimulus.WasSuccessfullySensed()){
    //    SetFocus(Target); // AI가 타겟을 바라보게 함
    //}
    else{
        ClearFocus(EAIFocusPriority::Gameplay); // 감지하지 못하면 포커스 해제
    }

    // 플레이어와의 거리 계산
    float Distance = FVector::Dist(Target->GetActorLocation(), ControlledPawn->GetActorLocation());

    // BlackBoard 키에 대상 설정
    BlackboardComp->SetValueAsObject("Target", Target);

    // 추적 및 공격 여부 결정
    bool bInTraceRange = Distance >= IAttack::Execute_GetattackRange(AttackComp) && Distance <= IEnemyMove::Execute_GettraceRange(MoveComp);
    bool bInAttackRange = Distance <= IAttack::Execute_GetattackRange(AttackComp);

    BlackboardComp->SetValueAsBool("IsInTraceRange", bInTraceRange);
    BlackboardComp->SetValueAsBool("IsInAttackRange", bInAttackRange);
}