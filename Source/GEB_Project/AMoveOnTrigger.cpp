#include "AMoveOnTrigger.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h" // [중요] 타이머 사용을 위해 필수 포함

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
		ExecuteMoveProcess(); // 공통 처리 함수 호출
	}
}

// [추가] 이동 시작 프로세스 (딜레이 처리 포함)
void AAMoveOnTrigger::ExecuteMoveProcess()
{
	if (bTriggered) return; // 이미 발동되었으면 무시
	bTriggered = true;      // 발동 상태로 변경

	// 추가 트리거 방지 (한 번만 동작 보장)
	Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 지연 시간(StartDelay)이 설정되어 있다면 타이머 사용
	if (StartDelay > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			StartDelayTimerHandle,
			this,
			&AAMoveOnTrigger::StartMovement,
			StartDelay,
			false
		);
	}
	else
	{
		// 지연 시간 없으면 즉시 시작
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

		// 순간이동 + 루프면 삭제 (기존 로직 유지)
		if (bCanLoof)
		{
			TArray<AActor*> AttachedActors;
			GetAttachedActors(AttachedActors); // 내 하위에 붙은 모든 액터 가져오기

			for (AActor* Child : AttachedActors)
			{
				if (Child && !Child->IsPendingKillPending())
				{
					Child->Destroy();
				}
			}

			// 그 다음 나 자신 삭제
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

	// 목표 도달 판정 (오차 범위 1.0f 정도로 여유를 둠)
	if (Dist <= 1.0f)
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
	ExecuteMoveProcess(); // 공통 처리 함수 호출
}