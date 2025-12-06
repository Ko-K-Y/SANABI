#include "RotatingItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h" // 플레이어 감지용

ARotatingItem::ARotatingItem()
{
	PrimaryActorTick.bCanEverTick = true; // Tick 함수 켜기

	// 1. 메시 컴포넌트 생성 및 루트 설정
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionProfileName(TEXT("NoCollision")); // 메시는 장식용이므로 충돌 끔

	// 2. 트리거 컴포넌트 생성
	TriggerComp = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerComp"));
	TriggerComp->SetupAttachment(RootComponent);
	TriggerComp->SetSphereRadius(100.0f);
	TriggerComp->SetCollisionProfileName(TEXT("Trigger")); // 트리거 설정
}

void ARotatingItem::BeginPlay()
{
	Super::BeginPlay();

	//  20도 기울어진 상태로 시작 (Pitch축 20도)
	SetActorRotation(FRotator(20.0f, 0.0f, 0.0f));

	// 충돌 델리게이트 연결
	TriggerComp->OnComponentBeginOverlap.AddDynamic(this, &ARotatingItem::OnOverlapBegin);
}

void ARotatingItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsTriggered)
	{
		// 1. 트리거 전: 제자리 회전 (Local Rotation 사용 -> 기울어진 축 유지하며 회전)
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
	}
	else
	{
		// 2. 트리거 후: 크기가 0으로 서서히 작아짐
		FVector CurrentScale = GetActorScale3D();
		FVector TargetScale = FVector::ZeroVector;

		// VInterpTo를 사용하여 부드럽게 줄어듦
		FVector NewScale = FMath::VInterpTo(CurrentScale, TargetScale, DeltaTime, ShrinkSpeed);
		SetActorScale3D(NewScale);

		// 크기가 충분히 작아지면 액터 삭제
		if (NewScale.IsNearlyZero(0.01f))
		{
			Destroy();
		}
	}
}

void ARotatingItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 이미 트리거 되었거나 자기 자신과의 충돌은 무시
	if (bIsTriggered || OtherActor == this) return;

	// 충돌한 대상이 플레이어(Pawn)인지 확인
	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		bIsTriggered = true;

		// 블루프린트 이벤트 호출
		OnHookActivated();

		// 중복 트리거 방지
		TriggerComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}