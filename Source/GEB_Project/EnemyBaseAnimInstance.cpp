// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBaseAnimInstance.h"
#include "BaseEnemy.h"
#include "HealthInterface.h"

void UEnemyBaseAnimInstance::OnStateAnimationEnds()
{
	if (State == EAnimState::Attack) {
		State = EAnimState::Locomotion;
	}
	else {
		ABaseEnemy* enemy = Cast<ABaseEnemy>(TryGetPawnOwner());
		if (State == EAnimState::Hit) {
			if (IHealthInterface::Execute_GetCurrentHealth(enemy->GetHealthComponent()) > 0.0f) {
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Enemy Hit Animation Ended - Returning to Locomotion"));
				State = EAnimState::Locomotion;
			}
		}
		else if (State == EAnimState::Die) {
			enemy->DieProcessEnd();
		}

	}
}

void UEnemyBaseAnimInstance::SetAnimStateAttack()
{
	if (State == EAnimState::Die || State == EAnimState::Hit) {
		return;
	}
	State = EAnimState::Attack;
}

void UEnemyBaseAnimInstance::SetAnimStateHit()
{
	if (State == EAnimState::Die) {
		return;
	}
	State = EAnimState::Hit;
}

void UEnemyBaseAnimInstance::SetAnimStateDie()
{
	State = EAnimState::Die;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Enemy Die Animation Started"));
}

void UEnemyBaseAnimInstance::HitEndForce()
{
	if (State == EAnimState::Hit) {
		State = EAnimState::Locomotion;
	}
}

void UEnemyBaseAnimInstance::DieEndForce()
{
	if (State == EAnimState::Die) {
		ABaseEnemy* enemy = Cast<ABaseEnemy>(TryGetPawnOwner());
		enemy->DieProcessEnd();
	}
}