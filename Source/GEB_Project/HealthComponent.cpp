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
    const int32 IntDamage = FMath::Max(1, FMath::FloorToInt(Damage)); // 최소 1

    // ───────── 플레이어일 때: 무적/쉴드 처리 ─────────
    if (Owner && Owner->IsA(AGEB_ProjectCharacter::StaticClass()))
    {
        if (UPlayerStateComponent* PlayerState = Owner->FindComponentByClass<UPlayerStateComponent>())
        {
            // 무적 중이면 데미지 무시
            if (PlayerState->bIsAttacked)
            {
                UE_LOG(LogTemp, Warning, TEXT("Player Invincible - damage ignored"));
                return;
            }

		PlayerState->bIsAttacked = true;
		PlayerState->Invincibility();
		if (CurrentHealth > 0) CurrentHealth -= Damage;

		// 11.24 권신혁 추가. 데미지 입으면 방송
		if (CurrentHealth > 0) // 죽은게 아니라면
		{
			OnDamaged.Broadcast();
		}
	}
	// 공통 로직
	else if (ABaseEnemy* EnemyOwner = Cast<ABaseEnemy>(Owner)) {
		UShieldComponent* ShieldComp = EnemyOwner->FindComponentByClass<UShieldComponent>();
		if (ShieldComp && IShieldInterface::Execute_IsShieldActive(ShieldComp)){
			int RemainingShield = IShieldInterface::Execute_ApplyDamageToShield(ShieldComp, Damage);
		}
		else {
			UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(Owner->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());
			if (EnemyAnimInst) {
				if (EnemyAnimInst->State == EAnimState::Hit || EnemyAnimInst->State == EAnimState::Die) { return; }
				EnemyAnimInst->SetAnimStateHit();
			}
			CurrentHealth -= Damage;
			if (CurrentHealth <= 0) {
				CurrentHealth = 0;
				EnemyOwner->DieProcess();
			}
		}
	}
            bool bApplyToHP = true;
            int32 DamageToApply = IntDamage;

            // 쉴드 먼저 적용
            if (UShieldComponent* ShieldComp = Owner->FindComponentByClass<UShieldComponent>())
            {
                if (IShieldInterface::Execute_IsShieldActive(ShieldComp))
                {
                    const int32 RemainingDamage = IShieldInterface::Execute_ApplyDamageToShield(ShieldComp, float(IntDamage));

                    // 쉴드가 전부 흡수
                    if (RemainingDamage <= 0)
                    {
                        // 무적 시작 (인터페이스 이벤트는 반드시 Execute_ 로!)
                        PlayerState->bIsAttacked = true;
                        if (PlayerState->GetClass()->ImplementsInterface(UStateInterface::StaticClass()))
                        {
                            IStateInterface::Execute_Invincibility(PlayerState);
                        }

                        Broadcast(); // UI 갱신(피격 반응 등)
                        UE_LOG(LogTemp, Log, TEXT("[HP] %d / %d (shield absorbed)"), CurrentHealth, MaxHealth);
                        return;
                    }

                    // 일부만 흡수 → 남은 데미지 체력에 적용
                    DamageToApply = RemainingDamage;
                }
            }

            // 체력 감소
            if (bApplyToHP)
            {
                CurrentHealth = FMath::Clamp(CurrentHealth - DamageToApply, 0, MaxHealth);
            }

            // 무적 시작 (인터페이스 이벤트로 호출!)
            PlayerState->bIsAttacked = true;
            if (PlayerState->GetClass()->ImplementsInterface(UStateInterface::StaticClass()))
            {
                IStateInterface::Execute_Invincibility(PlayerState);
            }

            Broadcast();
            UE_LOG(LogTemp, Log, TEXT("[HP] %d / %d"), CurrentHealth, MaxHealth);
            return;
        }

        // PlayerStateComponent가 없으면 일반 적용
        CurrentHealth = FMath::Clamp(CurrentHealth - IntDamage, 0, MaxHealth);
        Broadcast();
        UE_LOG(LogTemp, Log, TEXT("[HP] %d / %d (no PlayerState)"), CurrentHealth, MaxHealth);
        return;
    }

    // ───────── 적/기타 액터 ─────────
    if (Owner)
    {
        if (USkeletalMeshComponent* Skel = Owner->FindComponentByClass<USkeletalMeshComponent>())
        {
            if (UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(Skel->GetAnimInstance()))
            {
                EnemyAnimInst->State = EAnimState::Hit;
            }
        }
    }

    CurrentHealth = FMath::Clamp(CurrentHealth - IntDamage, 0, MaxHealth);
    Broadcast();
    UE_LOG(LogTemp, Log, TEXT("[HP] %d / %d (non-player)"), CurrentHealth, MaxHealth);
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
