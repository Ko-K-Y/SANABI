// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthInterface.h"
#include "HealthComponent.generated.h"

// 11.24 권신혁 추가. 피격 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamagedSignature);

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

	// 11.24 권신혁 추가. 데미지 입었다는 것을 저장할 변수
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamagedSignature OnDamaged;
};
