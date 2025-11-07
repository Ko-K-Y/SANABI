#include "BTService_UpdateTargetDistance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "AttackComponent.h"
#include "EnemyMoveComponent.h"
#include "Attack.h"
#include "EnemyMove.h"
#include "EnemyBaseAnimInstance.h"

UBTService_UpdateTargetDistance::UBTService_UpdateTargetDistance()
{
    NodeName = TEXT("Update Target Distance");
    // 기본 갱신 주기 (에디터에서 Service 노드의 Interval로 조정 가능)
    Interval = 0.2f;
}

void UBTService_UpdateTargetDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    AAIController* AICont = OwnerComp.GetAIOwner();
    if (!BlackboardComp || !AICont) return;

    // 키 이름: 에디터에서 바인딩된 키가 있으면 사용, 없으면 기존 문자열 키 사용
    const FName TargetKeyName = TargetKey.SelectedKeyName.IsNone() ? FName(TEXT("Target")) : TargetKey.SelectedKeyName;
    const FName AttackKeyName = IsInAttackRangeKey.SelectedKeyName.IsNone() ? FName(TEXT("IsInAttackRange")) : IsInAttackRangeKey.SelectedKeyName;
    const FName TraceKeyName = IsInTraceRangeKey.SelectedKeyName.IsNone() ? FName(TEXT("IsInTraceRange")) : IsInTraceRangeKey.SelectedKeyName;
    const FName CanAttackKeyName = CanAttackKey.SelectedKeyName.IsNone() ? FName(TEXT("CanAttack")) : CanAttackKey.SelectedKeyName;

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKeyName));
    APawn* ControlledPawn = AICont->GetPawn();
    if (!TargetActor || !ControlledPawn) {
        // 대상이 없으면 플래그 false로 설정
        BlackboardComp->SetValueAsBool(AttackKeyName, false);
        BlackboardComp->SetValueAsBool(TraceKeyName, false);
        BlackboardComp->SetValueAsBool(CanAttackKeyName, false);
        return;
    }

    // 컴포넌트 획득
    UAttackComponent* AttackComp = ControlledPawn->FindComponentByClass<UAttackComponent>();
    UEnemyMoveComponent* MoveComp = ControlledPawn->FindComponentByClass<UEnemyMoveComponent>();
    if (!AttackComp || !MoveComp) {
        BlackboardComp->SetValueAsBool(AttackKeyName, false);
        BlackboardComp->SetValueAsBool(TraceKeyName, false);
        BlackboardComp->SetValueAsBool(CanAttackKeyName, false);
        return;
    }

    const float Distance = FVector::Dist(TargetActor->GetActorLocation(), ControlledPawn->GetActorLocation());

    // 인터페이스 호출로 범위 얻기
    const float AttackRange = IAttack::Execute_GetattackRange(AttackComp);
    const float TraceRange = IEnemyMove::Execute_GettraceRange(MoveComp);

    const bool bInAttackRange = Distance <= AttackRange;
    const bool bInTraceRange = Distance >= AttackRange && Distance <= TraceRange;

    // 쿨다운 확인: IAttack의 GetisCoolDown 사용 (true면 쿨다운 중)
    const bool bIsCooldown = IAttack::Execute_GetisCoolDown(AttackComp);
    const bool bCanAttack = (!bIsCooldown) && bInAttackRange;

    BlackboardComp->SetValueAsBool(AttackKeyName, bInAttackRange);
    BlackboardComp->SetValueAsBool(TraceKeyName, bInTraceRange);
    BlackboardComp->SetValueAsBool(CanAttackKeyName, bCanAttack);
}