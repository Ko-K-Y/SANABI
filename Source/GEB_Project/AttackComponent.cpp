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

	// 기존에 생성된 것이 있으면 정리
	for (UCapsuleComponent* C : SocketCapsules)
	{
		if (C) C->DestroyComponent();
	}
	SocketCapsules.Empty();

	// 각 소켓마다 캡슐 컴포넌트 생성 및 오버랩 이벤트 바인딩
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

		// 오버랩 이벤트 활성화: 애니메이션 상태를 확인한 후 처리
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Capsule->SetCollisionObjectType(ECC_WorldDynamic);
		Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Capsule->SetGenerateOverlapEvents(true);
		Capsule->SetHiddenInGame(true);

		// 오버랩 바인딩
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

	// 애니메이션 상태를 확인해서 Attack 상태가 아니면 최근 히트 집합 초기화
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
				}
			}
		}
	}
}

void UAttackComponent::PerformAttack_Implementation() {
	if (isCooldown) { return; }

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return;
	//Owner의 고유 공격 사용, 애니메이션, 이펙트 호출
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

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APawn* TargetPawn = Cast<APawn>(Target);

	AController* OwnerController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
	AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;

	const bool OwnerIsPlayer = OwnerController ? OwnerController->IsPlayerController() : false;
	const bool TargetIsPlayer = TargetController ? TargetController->IsPlayerController() : false;

	// 동일 진영(둘 다 플레이어이거나 둘 다 비플레이어)인 경우 무시
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

// 소켓 캡슐 오버랩 콜백: 애니메이션 인스턴스가 Attack 상태일 때만 피격 처리
void UAttackComponent::OnSocketOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	AActor* Owner = GetOwner();
	if (!Owner) return;
	if (OtherActor == Owner) return;

	// 애니메이션 인스턴스의 상태 확인
	USkeletalMeshComponent* MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshComp) return;
	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst) return;
	UEnemyBaseAnimInstance* EnemyAnimInst = Cast<UEnemyBaseAnimInstance>(AnimInst);
	if (!EnemyAnimInst) return;

	// Attack 상태가 아니면 무시
	if (EnemyAnimInst->State != EAnimState::Attack) return;

	// 중복 히트 방지: 이미 이번 공격에서 히트한 액터면 무시
	if (RecentlyHitActors.Contains(OtherActor)) return;

	// 앞쪽 판정(캡슐 기준)
	const FVector Center = OverlappedComp->GetComponentLocation();
	const FVector Forward = Owner->GetActorForwardVector().GetSafeNormal();
	const FVector ToHit = (OtherActor->GetActorLocation() - Center);
	if (ToHit.IsNearlyZero()) return;
	const float Dot = FVector::DotProduct(Forward, ToHit.GetSafeNormal());
	if (Dot <= 0.f) return;

	// 실제 히트 처리
	OnAttackHit(OtherActor);
	RecentlyHitActors.Add(OtherActor);

#if ENABLE_DRAW_DEBUG
	// 시각화: 히트 발생 지점 표시
	if (UWorld* World = Owner->GetWorld())
	{
		DrawDebugSphere(World, OtherActor->GetActorLocation(), 8.f, 6, FColor::Yellow, false, 0.5f);
	}
#endif
}
