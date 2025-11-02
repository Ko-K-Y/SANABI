#include "ADirectionsLighting.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Pawn.h"

AADirectionsLighting::AADirectionsLighting()
{
	PrimaryActorTick.bCanEverTick = false;

	// Trigger (Root)
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->SetBoxExtent(FVector(100.f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);

	// Light
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(RootComponent);
	PointLight->SetIntensity(0.f);     // 시작은 꺼진 상태
	PointLight->SetVisibility(false);
}

void AADirectionsLighting::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AADirectionsLighting::OnTriggerBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AADirectionsLighting::OnTriggerEndOverlap);
}

void AADirectionsLighting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AADirectionsLighting::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (!bOnlyPlayerControlled || Pawn->IsPlayerControlled())
		{
			++PlayerOverlapCount;
			UpdateLightState();
		}
	}
}

void AADirectionsLighting::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;

	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (!bOnlyPlayerControlled || Pawn->IsPlayerControlled())
		{
			PlayerOverlapCount = FMath::Max(0, PlayerOverlapCount - 1);
			UpdateLightState();
		}
	}
}

void AADirectionsLighting::UpdateLightState()
{
	if (PlayerOverlapCount > 0)
	{
		PointLight->SetVisibility(true);
		PointLight->SetIntensity(OnIntensity);
	}
	else
	{
		PointLight->SetIntensity(0.f);
		PointLight->SetVisibility(false);
	}
}
