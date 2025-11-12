// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "ShooterAttackComponent.h"
#include "ShooterEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GEB_PROJECT_API AShooterEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AShooterEnemy(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UShooterAttackComponent* ShooterAttackComp;

	virtual void BeginPlay() override;

	//ABaseEnemy::Attack()을 오버라이드하여 Shooter만의 공격 로직(Projectile 발사) 실행
	void Attack();
};
