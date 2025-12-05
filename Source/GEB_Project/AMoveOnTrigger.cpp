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
	TargetLocation = StartLocation + MoveOffset;

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AAMoveOnTrigger::OnTriggerBeginOverlap);
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
		// 정확히 목표 지점으로 정렬하고 이동 종료
		SetActorLocation(TargetLocation);
		bMoving = false;
		// (추가 기능 없이 종료)
		return;
	}

	// 속도 기반 선형 이동 (오버슈팅 방지)
	const float Step = MoveSpeed * DeltaTime;
	if (Step >= Dist)
	{
		SetActorLocation(TargetLocation);
		bMoving = false;
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
	bMoving = true;

	// 추가 트리거 방지 (한 번만 동작 보장)
	Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
