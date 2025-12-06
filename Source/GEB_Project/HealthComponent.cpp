// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "GEB_ProjectCharacter.h"
#include "PlayerStateComponent.h"
#include "ShieldInterface.h"
#include "ShieldComponent.h"
#include "EnemyBaseAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
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

void UHealthComponent::ResetEnemyInvincibility()
{
	bIsEnemyInvincible = false;
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

    // ───────── 1. 플레이어(Player)일 때 ─────────
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
                        PlayerState->bIsAttacked = true;
                        if (PlayerState->GetClass()->ImplementsInterface(UStateInterface::StaticClass()))
                        {
                            IStateInterface::Execute_Invincibility(PlayerState);
                        }
                        Broadcast();
                        UE_LOG(LogTemp, Log, TEXT("[HP] %d / %d (shield absorbed)"), CurrentHealth, MaxHealth);
                        return;
                    }
                    DamageToApply = RemainingDamage;
                }
            }

            // 체력 감소
            if (bApplyToHP)
            {
                UE_LOG(LogTemp, Warning, TEXT("APPLY DAMAGE (Player)"));

                CurrentHealth = FMath::Clamp(CurrentHealth - DamageToApply, 0, MaxHealth);

                // 11.24 권신혁 추가. 데미지 입으면 방송
                if (CurrentHealth > 0)
                {
                    OnDamaged.Broadcast();
                }

                // 무적 시작
                PlayerState->bIsAttacked = true;
                if (PlayerState->GetClass()->ImplementsInterface(UStateInterface::StaticClass()))
                {
                    IStateInterface::Execute_Invincibility(PlayerState);
                }
            }
        }
    }

    // ───────── 2. 적(Enemy)일 때 (플레이어 체크 밖으로 뺌) ─────────
    else if (ABaseEnemy* EnemyOwner = Cast<ABaseEnemy>(Owner)) 
    {
        // 무적(피격 모션 중)이면 무시
        if (bIsEnemyInvincible) return;

        UShieldComponent* ShieldComp = EnemyOwner->FindComponentByClass<UShieldComponent>();
        if (ShieldComp && IShieldInterface::Execute_IsShieldActive(ShieldComp)) 
        {
            int RemainingShield = IShieldInterface::Execute_ApplyDamageToShield(ShieldComp, Damage);
        }
        else 
        {
            UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(Owner->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());
            if (EnemyAnimInst) 
            {
                // 이미 죽거나 맞는 중이면 패스 (안전장치)
                if (EnemyAnimInst->State == EAnimState::Hit || EnemyAnimInst->State == EAnimState::Die) { return; }
                EnemyAnimInst->SetAnimStateHit();
            }

            CurrentHealth = FMath::Clamp(CurrentHealth - IntDamage, 0, MaxHealth);
            Broadcast();
            UE_LOG(LogTemp, Log, TEXT("[HP] %d / %d (Enemy Damaged)"), CurrentHealth, MaxHealth);

            if (CurrentHealth <= 0) 
            {
                CurrentHealth = 0;
                EnemyOwner->DieProcess();
            }
            else
            {
                // 무적 타이머 시작
                bIsEnemyInvincible = true;
                if (UWorld* World = GetWorld())
                {
                    World->GetTimerManager().SetTimer(TimerHandle_EnemyInvincibility, this, &UHealthComponent::ResetEnemyInvincibility, EnemyInvincibilityDuration, false);
                }
            }
        }
    }
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
