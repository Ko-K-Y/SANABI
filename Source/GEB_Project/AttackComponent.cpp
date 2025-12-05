// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBaseAnimInstance.h"
#include "HealthInterface.h"
#include "HealthComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

// Sets default values for this component's properties
UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	damage = 1;
	maxAttackCoolTime = 2.f;
	attackRange = 300.f;
	isCooldown = false;
	coolTime = 0.f;
	AttackCapsuleRadius = 20.f;
	AttackCapsuleHalfHeight = 40.f;
}


// Called when the game starts
void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshComp) return;

	// ������ ������ ���� ������ ����
	for (UCapsuleComponent* C : SocketCapsules)
	{
		if (C) C->DestroyComponent();
	}
	SocketCapsules.Empty();

	// �� ���ϸ��� ĸ�� ������Ʈ ���� �� ������ �̺�Ʈ ���ε�
	for (const FName& SocketName : AttackSocketNames)
	{
		if (SocketName == NAME_None) continue;
		if (!MeshComp->DoesSocketExist(SocketName)) continue;

		UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>(Owner);
		if (!Capsule) continue;

		Capsule->RegisterComponent();
		Capsule->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
		Capsule->SetRelativeLocation(FVector::ZeroVector);
		Capsule->SetCapsuleSize(AttackCapsuleRadius, AttackCapsuleHalfHeight);

		// ������ �̺�Ʈ Ȱ��ȭ: �ִϸ��̼� ���¸� Ȯ���� �� ó��
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Capsule->SetCollisionObjectType(ECC_WorldDynamic);
		Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Capsule->SetGenerateOverlapEvents(true);
		Capsule->SetHiddenInGame(true);

		// ������ ���ε�
		Capsule->OnComponentBeginOverlap.AddDynamic(this, &UAttackComponent::OnSocketOverlapBegin);

		SocketCapsules.Add(Capsule);
	}
}


// Called every frame
void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(GEngine)
	{
		FString CooldownStatus = isCooldown ? TEXT("On Cooldown") : TEXT("Ready to Attack");
		GEngine->AddOnScreenDebugMessage(22, DeltaTime, FColor::Green, FString::Printf(TEXT("Attack Component Status: %s"), *CooldownStatus));
	}

	if (isCooldown) {
		coolTime -= DeltaTime;
		if (coolTime <= 0.f) {
			coolTime = 0.f;
			isCooldown = false;
		}
	}

	// �ִϸ��̼� ���¸� Ȯ���ؼ� Attack ���°� �ƴϸ� �ֱ� ��Ʈ ���� �ʱ�ȭ
	AActor* Owner = GetOwner();
	if (Owner)
	{
		USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
		if (MeshComp)
		{
			if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
			{
				if (UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(AnimInst))
				{
					if (EnemyAnimInst->State != EAnimState::Attack)
					{
						RecentlyHitActors.Empty();
					}
#if ENABLE_DRAW_DEBUG
					else {
						if (!this) return;
						UWorld* World = this->GetWorld();
						if (!World) return;

						for (UCapsuleComponent* Cap : this->SocketCapsules)
						{
							if (!IsValid(Cap)) {
								continue;
							}

							const FVector Center = Cap->GetComponentLocation();
							const FQuat Rotation = Cap->GetComponentQuat();
							const float HalfHeight = Cap->GetScaledCapsuleHalfHeight();
							const float Radius = Cap->GetScaledCapsuleRadius();

							// 색상, 지속시간, 두께 등은 필요에 따라 조정하세요.
							DrawDebugCapsule(World, Center, HalfHeight, Radius, Rotation, FColor::Green,/*bPersistentLines=*/false, 0.f, /*DepthPriority=*/0, /*Thickness=*/2.0f);
						}
					}
#endif
				}
			}
		}
	}
}

void UAttackComponent::PerformAttack_Implementation() {
	if (isCooldown) { return; }

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return;
	//Owner�� ���� ���� ���, �ִϸ��̼�, ����Ʈ ȣ��
	// �ִϸ��̼� Notify���� �ǰ���ġ ����� OnAttackHit ȣ��
	//Owner�� ���� ���� ���, �ִϸ��̼�, ����Ʈ ȣ��
	UAnimInstance* AnimInst = Owner->GetMesh()->GetAnimInstance();
	UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(AnimInst);
	if (EnemyAnimInst) {
		EnemyAnimInst->SetAnimStateAttack();
	}
	RecentlyHitActors.Empty();

	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Attack!")); }

	isCooldown = true;
	coolTime = maxAttackCoolTime;
}

bool UAttackComponent::GetisCoolDown_Implementation() {
	return isCooldown;
}

float UAttackComponent::GetattackRange_Implementation() {
	return attackRange;
}

void UAttackComponent::OnAttackHit(AActor* Target) {
	if (!Target) { return; }
	// Target HealthComponent ApplyDamage ȣ�� ��
	
	// �����ڿ� ����� ��Ʈ�ѷ��� Ȯ���Ͽ� �÷��̾����÷��̾�(Enemy) ���踸 ���

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APawn* TargetPawn = Cast<APawn>(Target);

	AController* OwnerController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
	AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;

	const bool OwnerIsPlayer = OwnerController ? OwnerController->IsPlayerController() : false;
	const bool TargetIsPlayer = TargetController ? TargetController->IsPlayerController() : false;

	// ���� ����(�� �� �÷��̾��̰ų� �� �� ���÷��̾�)�� ��� ����
	// ���� ����(�� �� �÷��̾��̰ų� �� �� ���÷��̾�)�� ��� ����
	if (OwnerIsPlayer == TargetIsPlayer) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow,
				FString::Printf(TEXT("Ignored hit (same side): %s -> %s"), *GetOwner()->GetName(), *Target->GetName()));
		}
		return;
	}

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(33, 1.f, FColor::Yellow, FString::Printf(TEXT("Attack Hit Target: %s for %d damage"), *Target->GetName(), damage));
		UE_LOG(LogTemp, Log, TEXT("%s hit %s for %d"), *GetOwner()->GetName(), *Target->GetName(), damage);
	}

	if (UHealthComponent* HealthComp = Target->FindComponentByClass<UHealthComponent>()) {
		HealthComp->ApplyDamage_Implementation(static_cast<float>(damage));
	}
}

// ���� ĸ�� ������ �ݹ�: �ִϸ��̼� �ν��Ͻ��� Attack ������ ���� �ǰ� ó��
void UAttackComponent::OnSocketOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	AActor* Owner = GetOwner();
	if (!Owner) return;
	if (OtherActor == Owner) return;

	// �ִϸ��̼� �ν��Ͻ��� ���� Ȯ��
	USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshComp) return;
	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst) return;
	UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(AnimInst);
	if (!EnemyAnimInst) return;

	// Attack ���°� �ƴϸ� ����
	if (EnemyAnimInst->State != EAnimState::Attack) return;

	// �ߺ� ��Ʈ ����: �̹� �̹� ���ݿ��� ��Ʈ�� ���͸� ����
	if (RecentlyHitActors.Contains(OtherActor)) return;

	// ���� ����(ĸ�� ����)
	const FVector Center = OverlappedComp->GetComponentLocation();
	const FVector Forward = Owner->GetActorForwardVector().GetSafeNormal();
	const FVector ToHit = (OtherActor->GetActorLocation() - Center);
	if (ToHit.IsNearlyZero()) return;
	const float Dot = FVector::DotProduct(Forward, ToHit.GetSafeNormal());
	if (Dot <= 0.f) return;

	// ���� ��Ʈ ó��
	OnAttackHit(OtherActor);
	RecentlyHitActors.Add(OtherActor);

#if ENABLE_DRAW_DEBUG
	// �ð�ȭ: ��Ʈ �߻� ���� ǥ��
	if (UWorld* World = Owner->GetWorld())
	{
		DrawDebugSphere(World, OtherActor->GetActorLocation(), 8.f, 6, FColor::Yellow, false, 0.5f);
	}
#endif
}
