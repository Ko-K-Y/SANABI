// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "GEB_ProjectCharacter.h"
#include "PlayerStateComponent.h"
#include "ShieldInterface.h"
#include "ShieldComponent.h"
#include "EnemyBaseAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "BaseEnemy.h"
#include "StateInterface.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	MaxHealth = FMath::Clamp(MaxHealth, 1, MaxLimit); // 기본 3, 한도 5
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
	Broadcast();
}

void UHealthComponent::Broadcast()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	if (CurrentHealth <= 0) { OnDeath.Broadcast(); }
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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
    const int32 IntDamage = FMath::Max(1, FMath::FloorToInt(Damage)); // 최소 1, 정수화
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 플레이어라면: 피격 상태면 데미지 무시, 아니면 적용
	if (Owner->IsA(AGEB_ProjectCharacter::StaticClass()))
	{
		UPlayerStateComponent* PlayerState = Owner->FindComponentByClass<UPlayerStateComponent>();
		if (PlayerState->bIsAttacked)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Attacked"))
				return;
		}
    // 플레이어인 경우: 무적/쉴드 체크
    if (Owner && Owner->IsA(AGEB_ProjectCharacter::StaticClass()))
    {
        // PlayerState 널체크
        if (UPlayerStateComponent* PlayerState = Owner->FindComponentByClass<UPlayerStateComponent>())
        {
            if (PlayerState->bIsAttacked)
            {
                UE_LOG(LogTemp, Warning, TEXT("Player Invincible - damage ignored"));
                return;
            }

            // 실드 활성 시 실드에 먼저 적용 (필요 시 로직 보강)
            if (UShieldComponent* ShieldComp = Owner->FindComponentByClass<UShieldComponent>())
            {
                if (IShieldInterface::Execute_IsShieldActive(ShieldComp))
                {
                    const int RemainingShield = IShieldInterface::Execute_ApplyDamageToShield(ShieldComp, Damage);
                    // TODO: 실드가 남아있으면 체력 미감소 처리 유지할지 말지?
                    // 일단은 실드로 모두 흡수되더라도 무적 시간만 주고 체력은 이후에 한 번만 계산
                }
            }

            // 피격 처리(무적 시작)
            PlayerState->bIsAttacked = true;
            PlayerState->Invincibility();
        }
    }
    else
    {
        // 적일 때 피격 애니메이션 트리거 
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
    }

    // 체력은 여기서 딱 한 번만 감소
    CurrentHealth = FMath::Clamp(CurrentHealth - IntDamage, 0, MaxHealth);

    // 항상 브로드캐스트 (0이어도 OnDeath 안에서 처리)
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