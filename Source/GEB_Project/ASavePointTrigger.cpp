#include "ASavePointTrigger.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "SavePointGISubsystem.h"

AASavePointTrigger::AASavePointTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->SetBoxExtent(FVector(120.f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);

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
	if (bOnlyPlayerControlled && !Pawn->IsPlayerControlled()) return;

	// 서버 권한에서만 기록
	if (!HasAuthority()) return;

	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (USavePointGISubsystem* SP = GI->GetSubsystem<USavePointGISubsystem>())
			{
				const FTransform SaveXform = GetSaveTransform();
				SP->SetSavePoint(SaveXform);

				if (bDebugLog)
				{
					UE_LOG(LogTemp, Warning, TEXT("[SavePoint][GI] Set at %s (Rot=%s) | GI=%s"),
						*SaveXform.GetLocation().ToString(),
						*SaveXform.GetRotation().Rotator().ToString(),
						*GetNameSafe(GI));
				}

				bAlreadyTriggered = true;
				OnSavePointActivated(SaveXform);
			}
		}
	}
}

FTransform AASavePointTrigger::GetSaveTransform() const
{
	return SavePoint ? SavePoint->GetComponentTransform() : GetActorTransform();
}
