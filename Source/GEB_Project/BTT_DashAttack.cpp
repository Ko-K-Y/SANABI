// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_DashAttack.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTT_DashAttack::UBTT_DashAttack()
{
	bNotifyTick = true;  // TickTask 활성화
}

EBTNodeResult::Type UBTT_DashAttack::ExcuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	AIPawn = AIController->GetPawn();
	if (!AIPawn)
		return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
		return EBTNodeResult::Failed;

	// 블랙보드에서 플레이어 위치 읽기 (예: "PlayerLocation"이라는 키)
	TargetLocation = BlackboardComp->GetValueAsVector("Target");
	StartLocation = AIPawn->GetActorLocation();

	// 점프 시작 신호 세팅
	bIsDashing = true;

	// 충돌체, 무브먼트 컴포넌트 등 준비 (필요시)

	return EBTNodeResult::InProgress;  
}

void UBTT_DashAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!bIsDashing || !AIPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
    
	PerformDash(DeltaSeconds);

	// 점프 완료 판단 후 FinishLatentTask 호출 필요
}

void UBTT_DashAttack::PerformDash(float DeltaTime)
{
	if (!AIPawn)
		return;

	// *** AnimInstance의 State를 DashAttack으로 바꿔주어야 함.
	
	const float DashSpeed = 2000.f; // 원하는 대시 속도 (단위: cm/s)
	FVector CurrentLocation = AIPawn->GetActorLocation();

	// 대시 방향(normalize), 목표와 현 위치 사이 거리
	FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
	float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);

	// 한번 이동할 거리 계산
	float MoveStep = DashSpeed * DeltaTime;

	if (DistanceToTarget > MoveStep)
	{
		// 다음 위치로 한 스텝 이동
		FVector NextLocation = CurrentLocation + Direction * MoveStep;
		AIPawn->SetActorLocation(NextLocation, true); // Sweep 옵션으로 충돌 검사 가능
	}
	else
	{
		// 목표 위치 도달 or 지나칠 경우, 정확히 타겟 위치로 이동 후 종료
		AIPawn->SetActorLocation(TargetLocation, true);
		// 대시 끝나면 TickTask/FinishLatentTask 호출 등 완료 처리
		bIsDashing = false;
		
		// *** 데미지 처리 등 추가 로직
	}

	
}
