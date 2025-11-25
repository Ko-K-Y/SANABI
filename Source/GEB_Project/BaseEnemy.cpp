// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "GameFramework/Controller.h"
#include "EnemyBaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/GameplayStatics.h>
#include "ExperienceComponent.h"
#include "BrainComponent.h"
#include "TimerManager.h"


// Sets default values
ABaseEnemy::ABaseEnemy(const FObjectInitializer& ObjectInitializer)
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

	if (MoveComp) {
		if(GetCharacterMovement())
			GetCharacterMovement()->MaxWalkSpeed = MoveComp->GetmovementSpeed_Implementation();
	}
	
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
	if (GetCharacterMovement()) {
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}

	if (UWorld* World = GetWorld())
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
		if (PlayerPawn)
		{
			if (UExperienceComponent* ExpComp = PlayerPawn->FindComponentByClass<UExperienceComponent>())
			{
				ExpComp->AddEXP(ExpReward);
			}
		}
	}
	auto animInst = Cast<UEnemyBaseAnimInstance>(GetMesh()->GetAnimInstance());
	if (animInst) {
		animInst->SetAnimStateDie();
	}

	AAIController* AICon = Cast<AAIController>(GetController());

	if (AICon)
	{
		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			Brain->StopLogic("Turret Died");
		}

		AICon->StopMovement();
		AICon->SetActorTickEnabled(false);
	}

}

void ABaseEnemy::DieProcessEnd() {
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]() {this->Destroy(); }, 1.0f, false);
}