#include "ADirectionsLighting.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Pawn.h"

AADirectionsLighting::AADirectionsLighting()
{
	PrimaryActorTick.bCanEverTick = true;

	// Trigger (Root)
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->SetBoxExtent(FVector(120.f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);

	// Light
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(RootComponent);
	PointLight->SetIntensity(0.f);
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

	const float Now = GetWorld()->GetTimeSeconds();

	// 최소 On 시간 대기 후 페이드 시작
	if (bPendingFade && Now >= EarliestFadeTime && OverlapCount <= 0)
	{
		bPendingFade = false;
		StartFadeOut(Now);
	}

	// 페이드아웃 진행
	if (bIsFadingOut)
	{
		const float Elapsed = Now - FadeStartTime;
		const float Alpha = FMath::Clamp(Elapsed / FadeOutDuration, 0.f, 1.f);
		const float NewIntensity = FMath::Lerp(OnIntensity, 0.f, Alpha);
		PointLight->SetIntensity(NewIntensity);

		if (Alpha >= 1.f)
		{
			bIsFadingOut = false;
			PointLight->SetVisibility(false);
			PointLight->SetIntensity(0.f);
		}
	}
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
			OverlapCount++;
			CancelFadeOut();      // 진행 중이던 꺼짐/대기 취소
			TurnOnHard();         // 즉시 최대 밝기 켜기

			// 지금부터 최소 MinOnDuration 동안은 꺼지지 않도록 마킹
			const float Now = GetWorld()->GetTimeSeconds();
			EarliestFadeTime = Now + MinOnDuration;
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
			OverlapCount = FMath::Max(0, OverlapCount - 1);

			// 더 이상 겹친 Pawn이 없으면: 최소 On 시간 뒤 페이드 시작
			if (OverlapCount <= 0)
			{
				const float Now = GetWorld()->GetTimeSeconds();
				if (Now >= EarliestFadeTime)
				{
					StartFadeOut(Now);
				}
				else
				{
					// 아직 최소 유지시간이 남았으므로 Tick에서 시간이 되면 페이드 시작
					bPendingFade = true;
				}
			}
		}
	}
}

void AADirectionsLighting::TurnOnHard()
{
	PointLight->SetVisibility(true);
	PointLight->SetIntensity(OnIntensity);
}

void AADirectionsLighting::StartFadeOut(float StartTime)
{
	bIsFadingOut = true;
	bPendingFade = false;
	FadeStartTime = StartTime;

	// 시작 시점에 밝기를 보장 (도중에 수정됐을 수도 있으므로)
	PointLight->SetVisibility(true);
	PointLight->SetIntensity(OnIntensity);
}

void AADirectionsLighting::CancelFadeOut()
{
	bIsFadingOut = false;
	bPendingFade = false;
}
