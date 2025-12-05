#include "ABonusExperienceTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ExperienceComponent.h"

AABonusExperienceTrigger::AABonusExperienceTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);

	// 오버랩 트리거로 동작하게 최소 세팅
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AABonusExperienceTrigger::OnTriggerBeginOverlap);
}

void AABonusExperienceTrigger::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (bConsumed) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn || OtherActor != PlayerPawn) return;

	if (UExperienceComponent* ExpComp = PlayerPawn->FindComponentByClass<UExperienceComponent>())
	{
		bConsumed = true;
		ExpComp->AddEXP(ExpAmount);

		Destroy(); // 이후 비활성화(가장 최소)
	}
}
