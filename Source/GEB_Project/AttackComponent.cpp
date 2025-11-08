// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBaseAnimInstance.h"
#include "HealthInterface.h"
#include "HealthComponent.h"

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
	//Owner�� ���� ���� ���, �ִϸ��̼�, ����Ʈ ȣ��
	// �ִϸ��̼� Notify���� �ǰ���ġ ����� OnAttackHit ȣ��
	UAnimInstance* AnimInst = Owner->GetMesh()->GetAnimInstance();
	UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(AnimInst); 
	if (EnemyAnimInst) {
		EnemyAnimInst->SetAnimStateAttack();
	}


	/*AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return;*/ //AI ���º�ȭ ����� �� ������ ���
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
	// Target HealthComponent ApplyDamage ȣ�� ��
	
	// �����ڿ� ����� ��Ʈ�ѷ��� Ȯ���Ͽ� �÷��̾����÷��̾�(Enemy) ���踸 ���
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APawn* TargetPawn = Cast<APawn>(Target);

	AController* OwnerController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
	AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;

	const bool OwnerIsPlayer = OwnerController ? OwnerController->IsPlayerController() : false;
	const bool TargetIsPlayer = TargetController ? TargetController->IsPlayerController() : false;

	// ���� ����(�� �� �÷��̾��̰ų� �� �� ���÷��̾�)�� ��� ����
	/*if (OwnerIsPlayer == TargetIsPlayer) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow,
				FString::Printf(TEXT("Ignored hit (same side): %s -> %s"), *GetOwner()->GetName(), *Target->GetName()));
		}
		return;
	}*/

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(33, 1.f, FColor::Yellow, FString::Printf(TEXT("Attack Hit Target: %s for %d damage"), *Target->GetName(), damage));
		UE_LOG(LogTemp, Log, TEXT("%s hit %s for %d"), *GetOwner()->GetName(), *Target->GetName(), damage);
	}

	if(Target->Implements<UHealthInterface>()) {
		IHealthInterface::Execute_ApplyDamage(Target, damage);
	}

	if (UHealthComponent* HealthComp = Target->FindComponentByClass<UHealthComponent>()) {
		IHealthInterface::Execute_ApplyDamage(HealthComp, static_cast<float>(damage));
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(34, 1.f, FColor::Cyan, FString::Printf(TEXT("Target Current Health: %d"), HealthComp->GetCurrentHealth_Implementation()));
		}
		return;
	}
}
