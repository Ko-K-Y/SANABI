// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HealthInterface.h"
#include "HealthComponent.h"
#include "GEB_ProjectCharacter.h"

// Sets default values
APlayerProjectile::APlayerProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 충돌체 설정 
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f); // 크기 살짝 키움
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &APlayerProjectile::OnHit);

	// 플레이어가 총알을 밟고 올라서지 못하게 설정
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComp;

	// 2. 총알 외형 (메쉬) 설정
	// (블루프린트에서 Sphere Mesh 등을 넣어서 눈에 보이게 만듭니다)
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(CollisionComp);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌은 Sphere가 하므로 메쉬는 끔

	// 3. 움직임 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 10000.f; // 기본 속도 (나중에 덮어씌워짐)
	ProjectileMovement->MaxSpeed = 10000.f;
	ProjectileMovement->bRotationFollowsVelocity = true; // 날아가는 방향으로 회전
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력 0 (직사)
	ProjectileMovement->Friction = 0.0f; // 마찰력 0

	// 4. 업데이트 안돼서 느려지는 경우 방지
	ProjectileMovement->bInterpMovement = true;
	ProjectileMovement->bSweepCollision = true;

	InitialLifeSpan = 3.0f; // 3초 뒤 삭제
}

// Called when the game starts or when spawned
void APlayerProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetInstigator())
	{
		CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
	}
}

// Called every frame
void APlayerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 유효성 검사 (나 자신 무시)
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		// 혹시 몰라 Instigator 체크 한번 더
		if (OtherActor == GetInstigator()) return;

		// 1. 적(HealthComponent 보유)인지 확인
		UHealthComponent* HealthComp = OtherActor->FindComponentByClass<UHealthComponent>();

		if (HealthComp)
		{
			// ==========================================
			// [상황 A] 적을 맞춤! -> EnemyImpactEffect 재생
			// ==========================================
			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			// 데미지 적용
			IHealthInterface::Execute_ApplyDamage(HealthComp, DamageValue);

			// ==========================================
			// [추가] 킬 확인 및 마커 표시 로직
			// ==========================================
			// 현재 체력을 확인해서 0 이하라면(죽었다면)
			int32 CurrentHP = IHealthInterface::Execute_GetCurrentHealth(HealthComp);

			if (CurrentHP <= 0)
			{
				// 총을 쏜 사람(Instigator)이 내 캐릭터라면 -> 킬 마커 함수 호출
				if (AGEB_ProjectCharacter* MyPlayer = Cast<AGEB_ProjectCharacter>(GetInstigator()))
				{
					MyPlayer->ShowKillMarker(); // 블루프린트 이벤트 호출
					// UE_LOG(LogTemp, Log, TEXT("Enemy Eliminated!"));
				}
			}
		}
		else
		{
			// ==========================================
			// [상황 B] 벽/바닥을 맞춤! -> WallImpactEffect 재생
			// ==========================================
			if (WallImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WallImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			// (선택 사항) 벽 맞췄을 때 데칼(총탄 자국)을 남기고 싶다면 여기에 추가
		}

		// 총알 삭제
		Destroy();
	}
}