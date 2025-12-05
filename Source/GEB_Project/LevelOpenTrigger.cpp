#include "LevelOpenTrigger.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

// 세이브포인트(스폰) 저장값 초기화용
#include "SpawnPointSubsystem.h" // 프로젝트 경로에 맞게 include 경로 조정

ALevelOpenTrigger::ALevelOpenTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
}

void ALevelOpenTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelOpenTrigger::OnTriggerBeginOverlap);
	}
}

void ALevelOpenTrigger::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (bOneShot && bTriggered)
	{
		return;
	}

	if (LevelToOpen.IsNone())
	{
		return;
	}

	if (bOnlyPlayer)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (OtherActor != PlayerPawn)
		{
			return;
		}
	}

	// 원샷 처리(중복 호출 방지)
	if (bOneShot)
	{
		bTriggered = true;
	}

	// ✅ 다른 레벨을 열 때: 저장된 스폰(세이브포인트) 값을 0(Identity)로 초기화
	{
		const FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(this, true);
		const FString TargetLevel = LevelToOpen.ToString();

		if (!CurrentLevel.Equals(TargetLevel, ESearchCase::IgnoreCase))
		{
			if (USpawnPointSubsystem* Sub = USpawnPointSubsystem::Get(this))
			{
				Sub->SetSpawnPoint(FTransform::Identity);
			}
		}
	}

	UGameplayStatics::OpenLevel(this, LevelToOpen);
}
