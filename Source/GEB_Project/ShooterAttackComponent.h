// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackComponent.h"
#include "EnemyProjectile.h"
#include "ShooterAttackComponent.generated.h"

class AProjectile;

UCLASS()
class GEB_PROJECT_API UShooterAttackComponent : public UAttackComponent
{
	GENERATED_BODY()

protected:
	// **발사할 Projectile 클래스**를 에디터에서 설정할 수 있도록 합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Shooter Attack")
	TSubclassOf<AEnemyProjectile> ProjectileClass;

	// **Projectile이 생성될 위치** (보통 Enemy의 Mesh 소켓)
	UPROPERTY(EditDefaultsOnly, Category = "Shooter Attack")
	FName MuzzleSocketName;

public:
	// UAttackComponent의 핵심 함수를 오버라이드하여 발사 로직을 구현
	virtual void PerformAttack_Implementation() override;
};
