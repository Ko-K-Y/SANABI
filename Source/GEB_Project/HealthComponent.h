// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthInterface.h"
#include "HealthComponent.generated.h"

// 11.24 권신혁 추가. 피격 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamagedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, int32, Current, int32, Max);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

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
	int32 MaxHealth = 3;          // 초기 3칸
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Health")
	int32 CurrentHealth = 3;

	UPROPERTY(EditAnywhere, Category = "Health")
	int32 MaxLimit = 5;

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
	
	// 편의 함수들(스킬/회복/초기화용)
	UFUNCTION(BlueprintCallable) void Init(int32 InMax, int32 InCurrent);
	UFUNCTION(BlueprintCallable) void SetMax(int32 InMax, bool bFillToMax = false);
	UFUNCTION(BlueprintCallable) void AddMax(int32 Delta, bool bFillToMax = false);
	UFUNCTION(BlueprintCallable) void Heal(int32 Amount);
	UFUNCTION(BlueprintCallable) void FillToMax();

	// 델리게이트(위젯에서 Bind)
	UPROPERTY(BlueprintAssignable) FOnHealthChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable) FOnDeath         OnDeath;

private:
	void Broadcast(); // 내부 통지
};
