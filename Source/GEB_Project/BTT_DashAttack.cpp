// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_DashAttack.h"
#include "AIController.h"
#include "SWarningOrErrorBox.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"

UBTT_DashAttack::UBTT_DashAttack()
{
	bNotifyTick = true;  // TickTask 활성화

	damage = 1;
	maxAttackCoolTime = 2.f;
	attackRange = 1500.f;
	isCooldown = false;
	coolTime = 0.f;
}

EBTNodeResult::Type UBTT_DashAttack::ExcuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("DashAttackExcuteTask1"));
	AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	AIPawn = AIController->GetPawn();
	if (!AIPawn)
		return EBTNodeResult::Failed;

	UE_LOG(LogTemp, Warning, TEXT("DashAttack ExcuteTask2"));
	
	// AIPawn이 ACharacter인지 확인하고 Movement Component를 가져옵니다.
	ACharacter* OwnerChar = Cast<ACharacter>(AIPawn);
	if (OwnerChar)
	{
		// 대시 시작 시 이동 컴포넌트를 비활성화하여 강제 이동을 방해하지 않도록 합니다.
		OwnerChar->GetCharacterMovement()->SetMovementMode(MOVE_None); 
	}
	
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
		return EBTNodeResult::Failed;
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	// 액터의 현재 위치를 가져옵니다.
	TargetLocation = TargetActor->GetActorLocation();
	StartLocation = AIPawn->GetActorLocation();

	
	bIsDashing = true; // 점프 시작 신호 세팅
	bMontagePlayed = false;  // 몽타주 재생 플래그 초기화

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
	if (!AIPawn || isCooldown) return;

	UE_LOG(LogTemp, Warning, TEXT("PerformDash"))
	// *** Dash Anim Montage 재생
	if (DashMontage && bIsDashing && !bMontagePlayed) 
	{
		PlayMontage(DashMontage, 1.f);
		bMontagePlayed = true; // 몽타주 재생 완료 플래그
	}
	
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
		bMontagePlayed = false; // 다음 대시를 위해 플래그 초기화
		
		// *** 데미지 처리 등 추가 로직

		// 쿨다운 시작
		isCooldown = true;
		coolTime = maxAttackCoolTime;

		// 이동 컴포넌트를 다시 활성화 (예: 걷기)
		ACharacter* OwnerChar = Cast<ACharacter>(AIPawn);
		if (OwnerChar)
		{
			OwnerChar->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		
		// BTT 노드 실행 종료 및 결과 반환
		UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(AIController->GetBrainComponent());
		if (OwnerComp)
		{
			// 성공적으로 대시 완료
			FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

void UBTT_DashAttack::PlayMontage(UAnimMontage* Montage, float PlaySpeed)
{
	ACharacter* OwnerChar = Cast<ACharacter>(AIPawn);
	if (!OwnerChar)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTT_DashAttack::PlayMontage - AIPawn is not an ACharacter!"));
		return;
	};
	UAnimInstance* AnimInstance = OwnerChar->GetMesh() ? OwnerChar->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AnimInstance is nullptr!"));
		return;
	}
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage, PlaySpeed);
	}
}

bool UBTT_DashAttack::GetisCoolDown()
{
	return isCooldown;

}

float UBTT_DashAttack::GetattackRange()
{
	return attackRange;

}
