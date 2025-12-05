#include "AMoveOnTrigger.h"
#include "Components/BoxComponent.h"

AAMoveOnTrigger::AAMoveOnTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(Root);

	// 단순 오버랩 설정 (플레이어/폰과 겹치면 발동하도록 최소 설정)
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionObjectType(ECC_WorldDynamic);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAMoveOnTrigger::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	bForward = true;
	TargetLocation = StartLocation + MoveOffset;

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AAMoveOnTrigger::OnTriggerBeginOverlap);

	// 2) 자동 시작
	if (bAutoStart)
	{
		bTriggered = true;   // 트리거 없이 시작하므로 "이미 발동된 것"으로 처리
		StartMovement();
	}
}

void AAMoveOnTrigger::StartMovement()
{
	// 3) 순간 이동
	if (MoveSpeed <= 0.f)
	{
		SetActorLocation(TargetLocation);
		bMoving = false;

		// 순간이동 + 루프면 삭제
		if (bCanLoof)
		{
			Destroy();
		}
		return;
	}

	bMoving = true;
}

void AAMoveOnTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bMoving) return;

	const FVector Current = GetActorLocation();
	const FVector ToTarget = TargetLocation - Current;
	const float Dist = ToTarget.Size();

	if (Dist <= KINDA_SMALL_NUMBER)
	{
		SetActorLocation(TargetLocation);

		// 1) 왕복(반복)
		if (bCanLoof)
		{
			bForward = !bForward;
			TargetLocation = bForward ? (StartLocation + MoveOffset) : StartLocation;
			// bMoving 유지
		}
		else
		{
			bMoving = false;
		}
		return;
	}

	const float Step = MoveSpeed * DeltaTime;
	if (Step >= Dist)
	{
		SetActorLocation(TargetLocation);

		// 1) 왕복(반복)
		if (bCanLoof)
		{
			bForward = !bForward;
			TargetLocation = bForward ? (StartLocation + MoveOffset) : StartLocation;
			// bMoving 유지
		}
		else
		{
			bMoving = false;
		}
		return;
	}

	const FVector DeltaMove = ToTarget.GetSafeNormal() * Step;
	SetActorLocation(Current + DeltaMove);
}

void AAMoveOnTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bTriggered) return;        // 한 번만 동작
	bTriggered = true;

	// 추가 트리거 방지 (한 번만 동작 보장)
	Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StartMovement();
}
