// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBaseAnimInstance.h"

// Sets default values for this component's properties
UAttackComponent::UAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	damage = 1;
	maxAttackCoolTime = 2.f;
	attackRange = 300.f;
	isCooldown = false;
	coolTime = 0.f;
}


// Called when the game starts
void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(GEngine)
	{
		FString CooldownStatus = isCooldown ? TEXT("On Cooldown") : TEXT("Ready to Attack");
		GEngine->AddOnScreenDebugMessage(22, DeltaTime, FColor::Green, FString::Printf(TEXT("Attack Component Status: %s"), *CooldownStatus));
	}

	if (isCooldown) {
		coolTime -= DeltaTime;
		if (coolTime <= 0.f) {
			coolTime = 0.f;
			isCooldown = false;
		}
	}
}

void UAttackComponent::PerformAttack_Implementation() {
	if (isCooldown) { return; }

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return;
	//Owner의 고유 공격 사용, 애니매이션, 이펙트 호출
	// 애니매이션 or 이펙트가 플레이어 충돌시 이벤트 핸들러에서 OnAttackHit 실행
	UAnimInstance* AnimInst = Owner->GetMesh()->GetAnimInstance();
	UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(AnimInst); 
	if (EnemyAnimInst) {
		EnemyAnimInst->SetAnimStateAttack();
	}


	/*AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return;*/ //AI 상태변화 사용할 일 있으면 사용
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Attack!")); }

	isCooldown = true;
	coolTime = maxAttackCoolTime;
}

bool UAttackComponent::GetisCoolDown_Implementation() {
	return isCooldown;
}

float UAttackComponent::GetattackRange_Implementation() {
	return attackRange;
}

void UAttackComponent::OnAttackHit(AActor* Target) {
	if (!Target) { return; }
	// Target HealthComponent ApplyDamage 호출 등
}
