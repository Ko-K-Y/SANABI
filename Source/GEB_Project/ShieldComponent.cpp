// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldComponent.h"

// Sets default values for this component's properties
UShieldComponent::UShieldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShieldComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if(CurrentShield > 0)
	{
		bIsShieldActive = true;
	}
	else
	{
		bIsShieldActive = false;
	}
}


// Called every frame
void UShieldComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int UShieldComponent::GetShield_Implementation()
{
	return CurrentShield;
}

int UShieldComponent::ApplyDamageToShield_Implementation(int damage)
{
	if (bIsShieldActive)
	{
		CurrentShield -= damage;
		if (CurrentShield < 0)
		{
			CurrentShield = 0;
			bIsShieldActive = false;
		}
		return CurrentShield;
	}
	else {
		return 0;
	}
}

bool UShieldComponent::IsShieldActive_Implementation()
{
	return bIsShieldActive;
}