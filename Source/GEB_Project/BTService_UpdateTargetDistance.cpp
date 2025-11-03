#include "BTService_UpdateTargetDistance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "AttackComponent.h"
#include "EnemyMoveComponent.h"
#include "Attack.h"
#include "EnemyMove.h"

UBTService_UpdateTargetDistance::UBTService_UpdateTargetDistance()
{
    NodeName = TEXT("Update Target Distance");
    // �⺻ ���� �ֱ� (�����Ϳ��� Service ����� Interval�� ���� ����)
    Interval = 0.2f;
}

void UBTService_UpdateTargetDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    AAIController* AICont = OwnerComp.GetAIOwner();
    if (!BlackboardComp || !AICont) return;

    // Ű �̸�: �����Ϳ��� ���ε��� Ű�� ������ ���, ������ ���� ���ڿ� Ű ���
    const FName TargetKeyName = TargetKey.SelectedKeyName.IsNone() ? FName(TEXT("Target")) : TargetKey.SelectedKeyName;
    const FName AttackKeyName = IsInAttackRangeKey.SelectedKeyName.IsNone() ? FName(TEXT("IsInAttackRange")) : IsInAttackRangeKey.SelectedKeyName;
    const FName TraceKeyName = IsInTraceRangeKey.SelectedKeyName.IsNone() ? FName(TEXT("IsInTraceRange")) : IsInTraceRangeKey.SelectedKeyName;
    const FName CanAttackKeyName = CanAttackKey.SelectedKeyName.IsNone() ? FName(TEXT("CanAttack")) : CanAttackKey.SelectedKeyName;

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKeyName));
    APawn* ControlledPawn = AICont->GetPawn();
    if (!TargetActor || !ControlledPawn) {
        // ����� ������ �÷��� false�� ����
        BlackboardComp->SetValueAsBool(AttackKeyName, false);
        BlackboardComp->SetValueAsBool(TraceKeyName, false);
        BlackboardComp->SetValueAsBool(CanAttackKeyName, false);
        return;
    }

    // ������Ʈ ȹ��
    UAttackComponent* AttackComp = ControlledPawn->FindComponentByClass<UAttackComponent>();
    UEnemyMoveComponent* MoveComp = ControlledPawn->FindComponentByClass<UEnemyMoveComponent>();
    if (!AttackComp || !MoveComp) {
        BlackboardComp->SetValueAsBool(AttackKeyName, false);
        BlackboardComp->SetValueAsBool(TraceKeyName, false);
        BlackboardComp->SetValueAsBool(CanAttackKeyName, false);
        return;
    }

    const float Distance = FVector::Dist(TargetActor->GetActorLocation(), ControlledPawn->GetActorLocation());

    // �������̽� ȣ��� ���� ���
    const float AttackRange = IAttack::Execute_GetattackRange(AttackComp);
    const float TraceRange = IEnemyMove::Execute_GettraceRange(MoveComp);

    const bool bInAttackRange = Distance <= AttackRange;
    const bool bInTraceRange = Distance <= TraceRange;

    // ��ٿ� Ȯ��: IAttack�� GetisCoolDown ��� (true�� ��ٿ� ��)
    const bool bIsCooldown = IAttack::Execute_GetisCoolDown(AttackComp);
    const bool bCanAttack = (!bIsCooldown) && bInAttackRange;

    BlackboardComp->SetValueAsBool(AttackKeyName, bInAttackRange);
    BlackboardComp->SetValueAsBool(TraceKeyName, bInTraceRange);
    BlackboardComp->SetValueAsBool(CanAttackKeyName, bCanAttack);

    UE_LOG(LogTemp, Warning, TEXT("Distance: %.2f | AttackRange: %.2f | TraceRange: %.2f | InAttack: %s | InTrace: %s | CanAttack: %s"),
        Distance, AttackRange, TraceRange,
        bInAttackRange ? TEXT("TRUE") : TEXT("FALSE"),
        bInTraceRange ? TEXT("TRUE") : TEXT("FALSE"),
        bCanAttack ? TEXT("TRUE") : TEXT("FALSE"));
}