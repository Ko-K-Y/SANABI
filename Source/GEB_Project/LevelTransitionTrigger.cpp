// LevelTransitionTrigger.cpp
#include "LevelTransitionTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "ExperienceComponent.h"
#include "PlayerProgressGameInstance.h" 

ALevelTransitionTrigger::ALevelTransitionTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    SetRootComponent(Box);

    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    Box->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionTrigger::OnBeginOverlap);

    NextLevelName = NAME_None;
}

void ALevelTransitionTrigger::OnBeginOverlap(
    UPrimitiveComponent* /*OverlappedComp*/,
    AActor* OtherActor,
    UPrimitiveComponent* /*OtherComp*/,
    int32 /*OtherBodyIndex*/,
    bool /*bFromSweep*/,
    const FHitResult& /*SweepResult*/)
{
    APawn* Pawn = Cast<APawn>(OtherActor);
    if (!Pawn) return;

    // 1) 현재 경험치 상태 저장
    if (UExperienceComponent* XP = Pawn->FindComponentByClass<UExperienceComponent>())
    {

        if (UPlayerProgressGameInstance* GI = GetGameInstance<UPlayerProgressGameInstance>())
        {
            GI->CaptureFrom(XP);
        }
    }

    // 2) 레벨 전환
    if (NextLevelName != NAME_None)
    {
        UGameplayStatics::OpenLevel(this, NextLevelName);
    }
}
