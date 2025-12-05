// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "HealthComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameViewportClient.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GEB_ProjectCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h" // 무브먼트 설정을 위해 추가

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
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
		ShootingCoolTime,                // 쿨타임 시간
		false
	);

	// 11.24 권신혁 추가. 공격 애니메이션 적용
	if (FireMontage)
	{
		// 내 주인(Owner)이 캐릭터인지 확인
		ACharacter* MyCharacter = Cast<ACharacter>(GetOwner());
		if (MyCharacter)
		{
			// 캐릭터의 메시 -> 애님 인스턴스를 가져와서 몽타주 재생
			UAnimInstance* AnimInstance = MyCharacter->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(FireMontage);
			}
		}
	}

	CurrentAmmo--;
	UE_LOG(LogTemp, Warning, TEXT("CurrentAmmo: %d"), CurrentAmmo);

	// ====================================================================================
	// 1단계: 카메라 라인 트레이스 (목표 지점 구하기)
	// ====================================================================================
	FHitResult CameraTraceResult;
	FVector WorldLocation;
	FVector WorldDirection;

	// 플레이어 컨트롤러를 가져오기
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	int32 ScreenSizeX;
	int32 ScreenSizeY;
	PlayerController->GetViewportSize(ScreenSizeX, ScreenSizeY);

	if (PlayerController && PlayerController->DeprojectScreenPositionToWorld(
		// 뷰포트 중앙 좌표 계산
		ScreenSizeX / 2.0f, ScreenSizeY / 2.0f,
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
			ECollisionChannel::ECC_Visibility, // Pawn보다 Visibility가 더 정확함
			Params
		);

		if (bHit) TargetPoint = CameraTraceResult.ImpactPoint; // 목표 지점 저장 (카메라 트레이스가 맞춘 지점)
		else TargetPoint = TraceEnd; // 맞은 곳이 없으면, 카메라 트레이스의 끝 지점을 목표로 설정
	}

	// =========================================================
	// [추가] 1.5단계: 불렛 마그네티즘 (유도 보정)
	// =========================================================
	FCollisionShape MagnetSphere = FCollisionShape::MakeSphere(BulletMagnetismRadius);
	TArray<FHitResult> MagnetHits;
	FCollisionQueryParams MagnetParams;
	MagnetParams.AddIgnoredActor(GetOwner());

	// 원래 목표점(TargetPoint) 주변을 훑어봄
	bool bFoundEnemy = GetWorld()->SweepMultiByChannel(
		MagnetHits,
		TargetPoint, TargetPoint, // 제자리 스윕
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		MagnetSphere,
		MagnetParams
	);

	if (bFoundEnemy)
	{
		for (const FHitResult& MagHit : MagnetHits)
		{
			// 적 태그가 있거나 HealthComponent가 있는 경우
			AActor* HitActor = MagHit.GetActor();
			if (HitActor && HitActor->ActorHasTag("Enemy"))
			{
				// 목표점을 적의 위치(중심)로 강제 변경!
				// (더 정교하게 하려면 Mesh의 'spine_03'이나 'head' 소켓 위치 사용 추천)
				TargetPoint = HitActor->GetActorLocation();

				// (디버그: 보정되었다는 표시)
				// DrawDebugLine(GetWorld(), MuzzleLocation, TargetPoint, FColor::Red, false, 2.0f);
				break; // 한 명만 잡으면 끝
			}
		}
	}

	// ====================================================================================
	// 2단계: 총알(Projectile) 소환 및 발사
	// ====================================================================================

	// Muzzle(총구) 컴포넌트 찾기
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
		// 총구 위치 및 회전
		FVector MuzzleLocation = MuzzleComponent->GetComponentLocation();

		// 1. 머즐 플래시(총구 화염) 스폰 ---
		if (MuzzleFlashEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(
				MuzzleFlashEffect, // 스폰할 파티클
				MuzzleComponent,   // 부착할 컴포넌트 (총구)
				FName("Muzzle_01"),// 부착할 소켓 이름 (없으면 None)
				FVector(0.f),      // 상대 위치
				FRotator(0.f, -90.f, 0.f), // 회전 보정
				EAttachLocation::SnapToTarget, // 위치 타입
				true               // 자동 파괴
			);
		}

		// 2. 프로젝틸(총알) 생성 ---
		if (ProjectileClass)
		{
			// (1) 발사 방향 계산: [총구]에서 [목표지점]을 바라보는 방향
			FVector LaunchDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();
			FRotator LaunchRotation = LaunchDirection.Rotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = Cast<APawn>(GetOwner()); // 쏜 사람(나) 등록

			// (2) 액터 소환
			APlayerProjectile* SpawnedProjectile = GetWorld()->SpawnActor<APlayerProjectile>(
				ProjectileClass,
				MuzzleLocation,
				LaunchRotation,
				SpawnParams
			);

			// (3) 속성 주입 (속도, 크기, 이펙트 등)
			if (SpawnedProjectile)
			{
				// 크기 설정
				SpawnedProjectile->SetActorScale3D(ProjectileScale);

				// 속도 설정 (ProjectileMovement 컴포넌트 접근)
				if (SpawnedProjectile->ProjectileMovement)
				{
					SpawnedProjectile->ProjectileMovement->InitialSpeed = ProjectileSpeed;
					SpawnedProjectile->ProjectileMovement->MaxSpeed = ProjectileSpeed;
				}

				// 피격 이펙트 전달 (Projectile 내부에서 Hit 시 사용)
				SpawnedProjectile->ImpactEffect = HitImpactEffect;
				SpawnedProjectile->WallImpactEffect = WallImpactEffect;

				// 데미지 값 설정 (필요하다면)
				SpawnedProjectile->DamageValue = ProjectileDamage;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ProjectileClass is NULL! Please assign BP_Bullet in Blueprint."));
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