// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

protected: // <--- 블루프린트에서 보이도록 protected로 변경

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
