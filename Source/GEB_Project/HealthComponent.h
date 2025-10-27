// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthInterface.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEB_PROJECT_API UHealthComponent : public UActorComponent, public IHealthInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Health")
	int MaxHealth; // 블루 프린트에서 초기값 할당
	UPROPERTY(blueprintReadWrite, EditAnywhere, Category = "Health")
	int CurrentHealth;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 인터페이스 함수의 구체 구현 (Implementation)
	virtual int GetCurrentHealth_Implementation() override;
	virtual int GetMaxHealth_Implementation() override;
	virtual void ApplyDamage_Implementation(float Damage) ;
};
