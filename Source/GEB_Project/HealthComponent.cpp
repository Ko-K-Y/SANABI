#include "HealthComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);

    if (AActor* Owner = GetOwner())
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnTakeAnyDamage);
    }
}

void UHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                       AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.f) return;
    const int32 IntDamage = FMath::Max(1, static_cast<int32>(FMath::RoundHalfFromZero(Damage)));
    CurrentHealth = FMath::Clamp(CurrentHealth - IntDamage, 0, MaxHealth);
}

void UHealthComponent::SetMaxHealth(int32 InMax, bool bClamp)
{
    MaxHealth = FMath::Max(1, InMax);
    if (bClamp)
    {
        CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
    }
}

void UHealthComponent::Heal(int32 InAmount, bool bOverheal)
{
    if (InAmount <= 0) return;
    if (bOverheal) CurrentHealth += InAmount;
    else CurrentHealth = FMath::Clamp(CurrentHealth + InAmount, 0, MaxHealth);
}

void UHealthComponent::FillMax(bool bOverheal)
{
    CurrentHealth = MaxHealth;
}
