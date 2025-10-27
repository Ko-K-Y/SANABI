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
	ABaseEnemy();

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION()
	virtual void DieProcess();


};
