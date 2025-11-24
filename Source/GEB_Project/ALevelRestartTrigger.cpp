#include "ALevelRestartTrigger.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

AALevelRestartTrigger::AALevelRestartTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	// Trigger Box 생성 및 설정 (Root)
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	RootComponent = Trigger;

	Trigger->SetBoxExtent(FVector(120.f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);
}

void AALevelRestartTrigger::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AALevelRestartTrigger::OnTriggerBeginOverlap);
}

void AALevelRestartTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	if (bTriggerOnce && bAlreadyTriggered) return;

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn) return;

	// 플레이어만?
	if (bOnlyPlayerControlled && !Pawn->IsPlayerControlled())
	{
		return;
	}

	// 중복 실행 방지 플래그
	bAlreadyTriggered = true;

	// 멀티플레이 고려: 서버에서 처리하도록 요청
	if (GetLocalRole() == ROLE_Authority)
	{
		if (RestartDelay <= 0.f)
		{
			DoRestartLevel();
		}
		else
		{
			GetWorldTimerManager().SetTimer(RestartTimerHandle, this, &AALevelRestartTrigger::DoRestartLevel, RestartDelay, false);
		}
	}
	else
	{
		// 클라이언트라면 서버에 요청
		ServerRequestRestart();
	}
}

void AALevelRestartTrigger::ServerRequestRestart_Implementation()
{
	if (RestartDelay <= 0.f)
	{
		DoRestartLevel();
	}
	else
	{
		GetWorldTimerManager().SetTimer(RestartTimerHandle, this, &AALevelRestartTrigger::DoRestartLevel, RestartDelay, false);
	}
}

void AALevelRestartTrigger::DoRestartLevel()
{
	// 현재 레벨 이름을 가져와 OpenLevel로 다시 로드
	UWorld* World = GetWorld();
	if (!World) return;

	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true /* bRemovePrefixString */);
	if (!CurrentLevelName.IsEmpty())
	{
		UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName)); // 레벨 재시작
	}
}
