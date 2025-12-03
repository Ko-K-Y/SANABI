// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_DashAttack.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Delegates/Delegate.h"               // AddUObject 오류 해결
#include "BehaviorTree/BehaviorTreeComponent.h" // UBehaviorTreeComponent 정의 (MyOwnerComp 관련)


UBTT_DashAttack::UBTT_DashAttack()
{
	bNotifyTaskFinished = true;
	
	damage = 1;
	maxAttackCoolTime = 2.f;
	attackRange = 1500.f;
	isCooldown = false;
	coolTime = 0.f;
	DashVelocity = 2000.f;
	MontagePlayRate = 1.f;

	NodeName = "Dash Attack";
}

EBTNodeResult::Type UBTT_DashAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// ... (1. AI Controller, AI Character 유효성 검사 및 MyOwnerComp 저장)
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());
	if (!AICharacter) return EBTNodeResult::Failed;
	MyOwnerComp = &OwnerComp;

	// ... (3. Player Location 및 LaunchVelocity 계산 로직 유지)
	APawn* PlayerPawn = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerPawn) return EBTNodeResult::Failed;

	FVector AICurrentLocation = AICharacter->GetActorLocation();
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector DirectionVector = PlayerLocation - AICurrentLocation;
	FVector DashDirection = DirectionVector.GetSafeNormal(0.0001f);
	FVector LaunchVelocity = DashDirection * DashVelocity;

	// 여기에 공격을 할 거라는 예고로 플레이어의 발 부분에 빨간 원 생성하기 + 생성하고 1초 딜레이 주기
	if (WarningActorClass)
	{
		// 블루프린트의 Break Transform + (0,0,15) 로직 구현
		FVector SpawnLocation = PlayerLocation;
		SpawnLocation.Z -= 85.0f; 

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// BP_Warning 생성
		GetWorld()->SpawnActor<AActor>(WarningActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
	
	UAnimInstance* AnimInstance = AICharacter->GetMesh() ? AICharacter->GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInstance && DashMontage)
	{
		// 몽타주 재생
		PlayMontage(AICharacter, DashMontage, MontagePlayRate);
		
		// 몽타주 종료 델리게이트 바인딩 (Finish Execute 역할)
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &UBTT_DashAttack::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DashMontage);
		UE_LOG(LogTemp, Warning, TEXT("Delegate Bound"));
		

		AICharacter->GetWorldTimerManager().SetTimer(
		DashDelayHandle,
		FTimerDelegate::CreateUObject(this, &UBTT_DashAttack::DoDash, AICharacter, LaunchVelocity),
		WarningDelayTime,
		false
	);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DashMontage or AnimInstance is null. Skipping animation."));
	}
    
	// 몽타주 종료를 기다리기 위해 InProgress 반환
	return EBTNodeResult::InProgress;
}

void UBTT_DashAttack::PlayMontage(ACharacter* TargetCharacter, UAnimMontage* Montage, float PlayRate)
{
	if (!TargetCharacter) return;
	UAnimInstance* AnimInstance = TargetCharacter->GetMesh() ? TargetCharacter->GetMesh()->GetAnimInstance() : nullptr;

	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage, PlayRate);
	}
}

void UBTT_DashAttack::DoDash(ACharacter* AICharacter, FVector LaunchVelocity)
{
	// ✨ 3. 이동 실행 (몽타주 재생 여부와 관계없이 블루프린트처럼 즉시 실행)
	AICharacter->LaunchCharacter(LaunchVelocity, false, false);
}

bool UBTT_DashAttack::GetisCoolDown()
{
	return isCooldown;

}

float UBTT_DashAttack::GetattackRange()
{
	return attackRange;

}

void UBTT_DashAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMontageEnded"));
	
	if (Montage == DashMontage && MyOwnerComp) 
	{
		FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
       
		UE_LOG(LogTemp, Warning, TEXT("Montage End/Interrupted. Finishing task as SUCCEEDED."));
       
		MyOwnerComp = nullptr;
	}
}
