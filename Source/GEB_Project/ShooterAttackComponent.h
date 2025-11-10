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
	
public:
	UShooterAttackComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 총구 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	USceneComponent* MuzzleLocation;
	
protected:
	virtual void BeginPlay() override;
	// **발사할 Projectile 클래스**를 에디터에서 설정할 수 있도록 합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Shooter Attack")
	TSubclassOf<AEnemyProjectile> ProjectileClass;

	
public:
	// UAttackComponent의 핵심 함수를 오버라이드하여 발사 로직을 구현
	virtual void PerformAttack_Implementation() override;
	
	virtual bool GetisCoolDown_Implementation() override;
	virtual float GetattackRange_Implementation() override;
};
