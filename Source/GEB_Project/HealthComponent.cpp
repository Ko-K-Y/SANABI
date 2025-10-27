// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "GEB_ProjectCharacter.h"
#include "PlayerStateComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
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
	AActor* Owner = GetOwner();

	// 플레이어라면: 피격 상태면 데미지 무시, 아니면 적용
	if (Owner && Owner->IsA(AGEB_ProjectCharacter::StaticClass()))
	{
		UPlayerStateComponent* PlayerState = Owner->FindComponentByClass<UPlayerStateComponent>();
		if (PlayerState->bIsAttacked)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player Attacked"))
			return;
		}
		PlayerState->bIsAttacked = true;
		PlayerState->Invincibility();
		if (CurrentHealth > 0) CurrentHealth -= Damage;
	}
	// 공통 로직
	else if (CurrentHealth > 0) CurrentHealth -= Damage;

	UE_LOG(LogTemp, Error, TEXT("Current Health: %d"), CurrentHealth)
}