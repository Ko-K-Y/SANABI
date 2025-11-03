// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "HealthComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MaxAmmo;
	
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::Fire()
{
	if (CurrentAmmo <= 0 || bIsShooting) return;
	if (bIsReloading)
	{
		bIsReloading = false;
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	}

	// Shooting 쿨타임 적용
	bIsShooting = true;
	GetWorld()->GetTimerManager().SetTimer(
		ShootingTimer,
		this,
		&UWeaponComponent::FireCooldown, // 쿨타임 종료 후 호출할 함수
		ShootingCoolTime,               // 쿨타임 시간 (0.5f)
		false                           
	);
	
	CurrentAmmo--;
	UE_LOG(LogTemp, Warning, TEXT("CurrentAmmo: %d"), CurrentAmmo);
	
	// *** 1. 카메라로부터 라인 트레이스해서 에임에 맞는 적 위치 구하기 ***
	FHitResult CameraTraceResult;
	FVector WorldLocation;
	FVector WorldDirection;

	// 플레이어 컨트롤러를 가져오기
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController && PlayerController->DeprojectScreenPositionToWorld(
		// 뷰포트 중앙 좌표 (1920x1080에서 960, 540)
		960.0f, 540.0f, 
		WorldLocation, 
		WorldDirection))
	{
		// 트레이스 끝 지점 계산: 카메라 위치(WorldLocation)에서 
		// 카메라 방향(WorldDirection)으로 일정 거리(TraceDistance)만큼 연장
		const float TraceDistance = 100000.0f; // 매우 긴 거리
		FVector TraceEnd = WorldLocation + (WorldDirection * TraceDistance);

		// 카메라 트레이스 수행 (크로스헤어 조준)
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			CameraTraceResult,
			WorldLocation, // 카메라 시작 위치
			TraceEnd,      // 카메라 트레이스 끝 위치
			ECollisionChannel::ECC_Pawn, // 또는 적절한 채널
			Params
		);
		
		
		if (bHit) TargetPoint = CameraTraceResult.ImpactPoint; // 목표 지점 저장 (카메라 트레이스가 맞춘 지점)
		else TargetPoint = TraceEnd; // 맞은 곳이 없으면, 카메라 트레이스의 끝 지점을 목표로 설정

		DrawDebugLine(
			GetWorld(),
			WorldLocation, // 카메라 시작점
			TargetPoint,   // 목표 지점
			FColor::Green,
			false,
			3.0f,
			0,
			3.0f
		);
		
		// *** 2. 저장된 TargetPoint로 부터 Muzzle의 위치까지 line trace
		FHitResult MuzzleTraceResult;
		UStaticMeshComponent* MuzzleComponent = nullptr;
		TArray<UStaticMeshComponent*> StaticMeshComponents;
		GetOwner()->GetComponents(StaticMeshComponents);
		
		for (UStaticMeshComponent* Comp : StaticMeshComponents)
		{
			if (Comp && Comp->GetName().Equals(TEXT("Muzzle")))
			{
				MuzzleComponent = Comp;
				break;
			}
		}

		if (MuzzleComponent)
		{
			// Muzzle 위치
			FVector MuzzleLocation = MuzzleComponent->GetComponentLocation();
    
			// Muzzle 트레이스
			Params.AddIgnoredActor(GetOwner()); 
			// 총이나 총구 이펙트가 트레이스에 걸리는 것을 방지하기 위해 무기 액터도 무시해야하면 생성자에 WeaponActor 추가해서 무시해주기
			// Params.AddIgnoredActor(WeaponActor); 

			bool bHitFinal = GetWorld()->LineTraceSingleByChannel(
				MuzzleTraceResult,
				MuzzleLocation, // 총구 시작 위치
				TargetPoint,    // 1단계에서 찾은 최종 목표 지점
				ECollisionChannel::ECC_Pawn,
				Params
			);
			
			DrawDebugLine(
				GetWorld(),
				MuzzleLocation,
				TargetPoint,
				FColor::Red,
				false,      // 한 번만 그립니다.
				3.0f,       // 3초 동안 표시합니다.
				0,
				3.0f        // 라인 두께
			);
			
			// 데미지 적용
			AActor* HitActor = MuzzleTraceResult.GetActor();
			if (bHitFinal)
			{
				UE_LOG(LogTemp, Warning, TEXT("HIT!"));
				// MuzzleTraceResult.GetActor()를 통해 맞은 대상에 데미지 적용
				// UGameplayStatics::ApplyDamage 등을 사용할 수 있습니다.
				if (HitActor)
				{
					// UHealthComponent를 찾고 데미지 적용
					if (UHealthComponent* HitTarget = HitActor->FindComponentByClass<UHealthComponent>())
					{
						IHealthInterface::Execute_ApplyDamage(HitTarget, 1); // 데미지 값
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Health Component NOT Found on %s"), *HitActor->GetName());
					}
				}
			}
    
			// **선택 사항: 총구 이펙트와 사운드 재생**
			// UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation);
			// UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, MuzzleLocation);
		}
	}
}

void UWeaponComponent::FireCooldown()
{
	bIsShooting = false;
}

void UWeaponComponent::Reload()
{
	if (bIsReloading || (CurrentAmmo == MaxAmmo)) return;
	
	bIsReloading = true;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &UWeaponComponent::ReloadComplete, ReloadTime, false);
}

void UWeaponComponent::ReloadComplete()
{
	CurrentAmmo = MaxAmmo;
	bIsReloading = false;
}

