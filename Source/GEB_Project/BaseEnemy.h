// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AttackComponent.h"
#include "EnemyMoveComponent.h"
#include "HealthComponent.h"
#include "EnemyAIController.h"
#include "BaseEnemy.generated.h"

UCLASS()
class GEB_PROJECT_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemy(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE UAttackComponent* GetAttackComponent() const { return AttackComp; }
	FORCEINLINE UEnemyMoveComponent* GetMoveComponent() const { return MoveComp; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComp; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttackComponent* AttackComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UEnemyMoveComponent* MoveComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExpReward")
	int32 ExpReward = 50;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void DieProcess();

	UFUNCTION()
	virtual void DieProcessEnd();

	// ▼▼▼ [추가] 블루프린트에서 구현할 이벤트 선언 ▼▼▼
	/* C++에서는 호출만 하고, 실제 로직(파티클 재생, 사운드 등)은 블루프린트에서 짭니다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|Event")
	void OnBossDiedEvent();

};
