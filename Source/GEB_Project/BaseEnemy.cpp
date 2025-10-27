// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "GameFramework/Controller.h"
#include "EnemyBaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AttackComp = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComponent"));
	MoveComp = CreateDefaultSubobject<UEnemyMoveComponent>(TEXT("EnemyMoveComponent"));
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto animInst = Cast<UEnemyBaseAnimInstance>(GetMesh()->GetAnimInstance());
	if (animInst) {
		animInst->Speed = GetCharacterMovement()->Velocity.Size2D();
	}
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemy::DieProcess() {
	/*AController* Controller = GetController();
	if (Controller) {
		Controller->UnPossess();
	}*/
	//Controller Unpossess
	//이부분 Controller를 못읽어서 컴파일 에러나는데 아시는분 있나요

	//Do something

	SetLifeSpan(3.0f);
}

