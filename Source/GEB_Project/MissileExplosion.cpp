// Fill out your copyright notice in the Description page of Project Settings.


#include "MissileExplosion.h"

#include "HealthComponent.h"
#include "Components/BoxComponent.h"
#include "HealthInterface.h"

// Sets default values
AMissileExplosion::AMissileExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
}

// Called when the game starts or when spawned
void AMissileExplosion::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMissileExplosion::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("MissileAttackOverlap"))
	if (OtherActor && OtherActor != this)
	{
		// 1. 먼저 컴포넌트가 있는지 찾습니다.
		UHealthComponent* HealthComp = OtherActor->FindComponentByClass<UHealthComponent>();
        
		if (HealthComp)
		{
			// 2. '액터'가 아니라 찾은 '컴포넌트'가 인터페이스를 구현했는지 확인해야 합니다.
			if (HealthComp->GetClass()->ImplementsInterface(UHealthInterface::StaticClass()))
			{
				// 3. 컴포넌트에게 데미지 함수 호출
				IHealthInterface::Execute_ApplyDamage(HealthComp, DamageAmount);

				// 체력 확인 로그
				int32 CurrentHP = IHealthInterface::Execute_GetCurrentHealth(HealthComp);
				GEngine->AddOnScreenDebugMessage(
					-1, 3.f, FColor::Purple,
					FString::Printf(TEXT("HP: %d"), CurrentHP));
			}
			else
			{
				// 컴포넌트는 있지만 인터페이스 상속을 안 받은 경우
				UE_LOG(LogTemp, Warning, TEXT("HealthComponent found but Interface NOT implemented!"));
			}
		}
		else
		{
			// 컴포넌트가 없는 경우
			UE_LOG(LogTemp, Warning, TEXT("No HealthComponent on Actor: %s"), *OtherActor->GetName());
		}
	}
}




