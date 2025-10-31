// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAttackComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h" // 회전 계산을 위해 포함
#include "Slate/SGameLayerManager.h"
#include "EnemyBaseAnimInstance.h"


UShooterAttackComponent::UShooterAttackComponent()
{
	
}

void UShooterAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (isCooldown) {
		coolTime -= DeltaTime;
		if (coolTime <= 0.f) {
			coolTime = 0.f;
			isCooldown = false;
		}
	}
}


void UShooterAttackComponent::PerformAttack_Implementation()
{
	if (isCooldown) { return; }

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner || !ProjectileClass || !MuzzleLocation) return;

	// BP에서 추가한 MuzzleLocation Scene Component 찾기
	MuzzleLocation = nullptr;
	TArray<USceneComponent*> SceneComponents;
	Owner->GetComponents<USceneComponent>(SceneComponents);

	for (USceneComponent* Component : SceneComponents)
	{
		if (Component->GetName() == "MuzzleLocation") // 이름으로 찾음
		{
			MuzzleLocation = Component;
			break;
		}
	}
	if (!MuzzleLocation)
	{
		UE_LOG(LogTemp, Error, TEXT("MuzzleLocation Not Found."))
	}

	FVector SpawnLocation = MuzzleLocation->GetComponentLocation();
	FRotator SpawnRotation = MuzzleLocation->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Owner;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEnemyProjectile* Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	UAnimInstance* AnimInst = Owner->GetMesh()->GetAnimInstance();
	UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(AnimInst); 
	if (EnemyAnimInst) {
		EnemyAnimInst->SetAnimStateAttack();
	}
	
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("ShooterAttack!")); }
	
	if (Projectile)
	{
		// 쿨다운 시작
		isCooldown = true;

		// 부모 클래스인 AttackComponent에서 Tick이 계속 호출됨.
	}
	
}
