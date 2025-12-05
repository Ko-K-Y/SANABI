// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "GameFramework/Controller.h"
#include "EnemyBaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/GameplayStatics.h>
#include "ExperienceComponent.h"
#include "BrainComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
ABaseEnemy::ABaseEnemy(const FObjectInitializer& ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AttackComp = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComponent"));
	MoveComp = CreateDefaultSubobject<UEnemyMoveComponent>(TEXT("EnemyMoveComponent"));
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	// 컨트롤러가 보는 방향으로 캐릭터 몸통도 회전
	bUseControllerRotationYaw = true;

	// 이동하는 방향으로 몸통 회전 끄기 -> 충돌 방지
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (MoveComp) {
		if(GetCharacterMovement())
			GetCharacterMovement()->MaxWalkSpeed = MoveComp->GetmovementSpeed_Implementation();
	}

	UCapsuleComponent* MyCapsule = GetCapsuleComponent();
	UChildActorComponent* ChildActorComp = GetComponentByClass<UChildActorComponent>();
	if (MyCapsule && ChildActorComp)
	{
		AActor* ChildActor = ChildActorComp->GetChildActor();
		if (ChildActor)
		{
			// 내 캡슐이 자식 액터를 무시하도록 설정
			MyCapsule->IgnoreActorWhenMoving(ChildActor, true);

			// 자식 액터의 루트 컴포넌트(캡슐이나 메시)도 나(Enemy)를 무시하도록 설정
			UPrimitiveComponent* ChildRoot = Cast<UPrimitiveComponent>(ChildActor->GetRootComponent());
			if (ChildRoot)
			{
				ChildRoot->IgnoreActorWhenMoving(this, true);
			}
		}
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