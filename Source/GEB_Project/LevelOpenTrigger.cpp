#include "LevelOpenTrigger.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Camera/PlayerCameraManager.h" 
#include "TimerManager.h" 

#include "SpawnPointSubsystem.h" 

// ALevelTransitionTrigger 클래스에서 가져옴
#include "ExperienceComponent.h"
#include "PlayerProgressGameInstance.h"

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

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

    if (bOnlyPlayer)
    {
        APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
        if (OtherActor != PlayerPawn)
        {
            return;
        }
    }

    if (bOneShot)
    {
        bTriggered = true;
    }

    // 1. 카메라 페이드 아웃 실행
    if (PC && PC->PlayerCameraManager)
    {
        PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, FadeDuration, FLinearColor::Black, true, true);
    }

    // 2. FadeDuration 시간 뒤에 LoadLevel 함수가 실행되도록 타이머 설정
    GetWorldTimerManager().SetTimer(LevelLoadTimerHandle, this, &ALevelOpenTrigger::LoadLevel, FadeDuration, false);
}

void ALevelOpenTrigger::LoadLevel()
{
    // ALevelTransitionTrigger 클래스에서 가져옴

    
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

    if (PlayerPawn)
    {
        // 1) 현재 경험치 상태 저장
        if (UExperienceComponent* XP = PlayerPawn->FindComponentByClass<UExperienceComponent>())
        {
            UPlayerProgressGameInstance* GI = Cast<UPlayerProgressGameInstance>(GetGameInstance());
            if (GI)
            {
                GI->CaptureFrom(XP);
            }
        }
    }


    // 기존 로직: 다른 레벨을 열 때 스폰 포인트 초기화
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

    // 레벨 열기
    UGameplayStatics::OpenLevel(this, LevelToOpen);
}