#include "ASavePointTrigger.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "SavePointSubsystem.h"

AASavePointTrigger::AASavePointTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	// Trigger 박스를 루트로
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	RootComponent = Trigger;

	Trigger->SetBoxExtent(FVector(120.f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);

	// 저장용 기준점 (배치해서 회전/오프셋 조정 가능)
	SavePoint = CreateDefaultSubobject<USceneComponent>(TEXT("SavePoint"));
	SavePoint->SetupAttachment(RootComponent);
}

void AASavePointTrigger::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AASavePointTrigger::OnTriggerBeginOverlap);
}

void AASavePointTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	if (bTriggerOnce && bAlreadyTriggered) return;

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn) return;

	if (bOnlyPlayerControlled && !Pawn->IsPlayerControlled())
	{
		return;
	}

	// 권한 체크: 서버에서만 세이브포인트 갱신 (싱글도 서버 권한임)
	if (!HasAuthority())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (USavePointSubsystem* SP = World->GetSubsystem<USavePointSubsystem>())
		{
			const FTransform SaveXform = GetSaveTransform();
			SP->SetSavePoint(SaveXform);

			if (bDebugLog)
			{
				UE_LOG(LogTemp, Log, TEXT("[SavePoint] Set at %s (Rot=%s)"),
					*SaveXform.GetLocation().ToString(),
					*SaveXform.GetRotation().Rotator().ToString());
			}

			bAlreadyTriggered = true;
			// 블루프린트 후킹 (UI/SFX)
			OnSavePointActivated(SaveXform);
		}
	}
}

FTransform AASavePointTrigger::GetSaveTransform() const
{
	// SavePoint 컴포넌트가 존재하면 그 월드 트랜스폼을 사용
	if (SavePoint)
	{
		return SavePoint->GetComponentTransform();
	}
	// 그렇지 않으면 액터의 월드 트랜스폼
	return GetActorTransform();
}
