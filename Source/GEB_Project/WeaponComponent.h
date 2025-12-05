// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "PlayerProjectile.h"
#include "WeaponComponent.generated.h"


class UParticleSystem;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEB_PROJECT_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// *** Fire ***
	UFUNCTION()
	void Fire();
	void FireCooldown();
private:
	FVector TargetPoint; // 라인 트레이스 맞은 지점
	bool bIsShooting; //
	FTimerHandle ShootingTimer;
	float ShootingCoolTime = 0.5f;

protected:

	// *** 이펙트 (VFX) ***
	// <--- 머즐 플래시 (총구 화염) ---
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* MuzzleFlashEffect;

	// <--- 총알 궤적 (트레이서) ---
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* TracerEffect;

	// <--- 피격 효과 (임팩트) ---
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* HitImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* WallImpactEffect;

	// 11.24 권신혁 추가. 공격 애니메이션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* FireMontage;

	// 12.03 권신혁 추가. 발사체
	// 발사할 발사체 클래스 (블루프린트에서 BP_MyProjectile 할당)
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class APlayerProjectile> ProjectileClass;

	// 발사체 속도 조절
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ProjectileSpeed = 3000.0f;

	// 발사체 크기 조절
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FVector ProjectileScale = FVector(1.0f, 1.0f, 1.0f);
	
	// 발사체 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ProjectileDamage = 10.0f;
public:
	// *** Reload ***
	UFUNCTION()
	void Reload();
	UFUNCTION()
	void ReloadComplete();
private:
	bool bIsReloading = false; // 재장전 중인지 확인
	FTimerHandle ReloadTimer;
	float ReloadTime = 1.f;
	
	// Ammo
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	int MaxAmmo = 10;
	int CurrentAmmo;

};
