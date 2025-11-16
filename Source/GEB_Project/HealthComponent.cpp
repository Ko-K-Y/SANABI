// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "GEB_ProjectCharacter.h"
#include "PlayerStateComponent.h"
#include "ShieldInterface.h"
#include "ShieldComponent.h"
#include "EnemyBaseAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
// 필요 없다면 아래 둘은 지워도 OK
#include "BaseEnemy.h"
#include "StateInterface.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	MaxHealth = FMath::Clamp(MaxHealth, 1, MaxLimit); // 기본 3, 한도 5
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
	Broadcast();
}

void UHealthComponent::Broadcast()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	if (CurrentHealth <= 0)
	{
		OnDeath.Broadcast();
	}
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int UHealthComponent::GetCurrentHealth_Implementation()
{
	return CurrentHealth;
}

int UHealthComponent::GetMaxHealth_Implementation()
{
	return MaxHealth;
}

void UHealthComponent::ApplyDamage_Implementation(float Damage)
{
	if (Damage <= 0.f || CurrentHealth <= 0) return;

	AActor* Owner = GetOwner();                             
	const int32 IntDamage = FMath::Max(1, FMath::FloorToInt(Damage)); // 최소 1로 정수화

	// 플레이어인 경우: 무적/쉴드 처리
	if (Owner && Owner->IsA(AGEB_ProjectCharacter::StaticClass()))
	{
		if (UPlayerStateComponent* PlayerState = Owner->FindComponentByClass<UPlayerStateComponent>())
		{
			if (PlayerState->bIsAttacked)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player Invincible - damage ignored"));
				return;
			}

			// 쉴드가 켜져 있으면 먼저 쉴드에 적용
			if (UShieldComponent* ShieldComp = Owner->FindComponentByClass<UShieldComponent>())
			{
				if (IShieldInterface::Execute_IsShieldActive(ShieldComp))
				{
					// 팀 정책에 따라: 반환값을 '남은 데미지'로 쓰도록 합의했다면 그걸 체력에 반영
					const int32 RemainingDamage =
						IShieldInterface::Execute_ApplyDamageToShield(ShieldComp, (float)IntDamage);

					// 남은 데미지가 없으면 체력은 깎지 않고 무적/피격 처리만
					if (RemainingDamage <= 0)
					{
						PlayerState->bIsAttacked = true;
						PlayerState->Invincibility();
						Broadcast(); // UI는 갱신(무적/피격 반응을 위해)
						UE_LOG(LogTemp, Log, TEXT("[HP] %d / %d (shield absorbed)"), CurrentHealth, MaxHealth);
						return;
					}

					// 남은 데미지가 있으면 그 값을 적용
					CurrentHealth = FMath::Clamp(CurrentHealth - RemainingDamage, 0, MaxHealth);
				}
				else
				{
					// 쉴드 비활성: 일반 적용
					CurrentHealth = FMath::Clamp(CurrentHealth - IntDamage, 0, MaxHealth);
				}
			}
			else
			{
				// 쉴드 컴포넌트 없음: 일반 적용
				CurrentHealth = FMath::Clamp(CurrentHealth - IntDamage, 0, MaxHealth);
			}

			// 피격 처리(무적 시작)
			PlayerState->bIsAttacked = true;
			PlayerState->Invincibility();
		}
		else
		{
			// PlayerState가 없으면 그냥 적용
			CurrentHealth = FMath::Clamp(CurrentHealth - IntDamage, 0, MaxHealth);
		}
	}
	else
	{
		// 적/기타 액터: 피격 애니 트리거 후 적용
		if (Owner)
		{
			if (USkeletalMeshComponent* Skel = Owner->FindComponentByClass<USkeletalMeshComponent>())
			{
				if (UAnimInstance* Anim = Skel->GetAnimInstance())
				{
					if (UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(Anim))
					{
						EnemyAnimInst->State = EAnimState::Hit;
					}
				}
			}
		}

		CurrentHealth = FMath::Clamp(CurrentHealth - IntDamage, 0, MaxHealth);
	}

	Broadcast();
	UE_LOG(LogTemp, Log, TEXT("[HP] %d / %d"), CurrentHealth, MaxHealth);
}

void UHealthComponent::Init(int32 InMax, int32 InCurrent)
{
	MaxHealth = FMath::Clamp(InMax, 1, MaxLimit);
	CurrentHealth = FMath::Clamp(InCurrent, 0, MaxHealth);
	Broadcast();
}

void UHealthComponent::SetMax(int32 InMax, bool bFillToMax /*=false*/)
{
	MaxHealth = FMath::Clamp(InMax, 1, MaxLimit);
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
	if (bFillToMax) CurrentHealth = MaxHealth;
	Broadcast();
}

void UHealthComponent::AddMax(int32 Delta, bool bFillToMax /*=false*/)
{
	if (Delta == 0) return;
	SetMax(MaxHealth + Delta, bFillToMax); // Clamp 포함
}

void UHealthComponent::Heal(int32 Amount)
{
	if (Amount <= 0) return;
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0, MaxHealth);
	Broadcast();
}

void UHealthComponent::FillToMax()
{
	CurrentHealth = MaxHealth;
	Broadcast();
}
