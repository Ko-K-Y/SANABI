// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GEB_ProjectCharacter.h"
#include "HealthComponent.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicatingMovement(true);
	bAlwaysRelevant = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false; 
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력 영향 X
	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red, FString::Printf(TEXT("Hit Actor: %s"), *OtherActor->GetName()));

	// 자신의 충돌과 무시할 액터는 무시
	if (OtherActor && OtherActor != this && OtherComp)
	{
		// HealthComponent가 부착된 경우
		if (OtherActor->Implements<UHealthInterface>()) {
			IHealthInterface::Execute_ApplyDamage(OtherActor, ProjectileDamage);
			GEngine->AddOnScreenDebugMessage(
				-1, 3.f, FColor::Purple,
				FString::Printf(TEXT("HP: %d"), IHealthInterface::Execute_GetCurrentHealth(OtherActor)));
		}

		/*UHealthComponent* HealthComp = OtherActor->FindComponentByClass<UHealthComponent>();
		if (HealthComp)
		{
			IHealthInterface::Execute_ApplyDamage(HealthComp, ProjectileDamage);

			GEngine->AddOnScreenDebugMessage(
				-1, 3.f, FColor::Purple,
				FString::Printf(TEXT("HP: %d"), IHealthInterface::Execute_GetCurrentHealth(HealthComp)));
		}*/
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("No HealthComponent!"));
			UE_LOG(LogTemp, Warning, TEXT("OtherActor does not have HealthComponent!"));
		}
	}
	Destroy();
}