// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterEnemy.h"
#include "EnemyBaseAnimInstance.h"
#include "ShooterAttackComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AShooterEnemy::AShooterEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UShooterAttackComponent>(TEXT("AttackComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
    
	// 이제 AttackComp는 UShooterAttackComponent 타입으로 생성됨
	// 필요시 ShooterAttackComponent로 캐스팅해서 사용
	ShooterAttackComp = Cast<UShooterAttackComponent>(AttackComp);
	if (!ShooterAttackComp) UE_LOG(LogTemp, Error, TEXT("ShooterAttackComp Cast Failed."))
}

void AShooterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto animInst = Cast<UEnemyBaseAnimInstance>(GetMesh()->GetAnimInstance());
	if (animInst) {
		animInst->Speed = GetCharacterMovement()->Velocity.Size2D();
	}
}

void AShooterEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShooterEnemy::Attack()
{
	// ShooterAttackComp->FunctionName() 형식으로 사용
	ShooterAttackComp->PerformAttack();
}
