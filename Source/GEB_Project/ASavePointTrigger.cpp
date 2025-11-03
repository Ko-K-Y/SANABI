#include "ASavePointTrigger.h"

#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"

#include "SpawnPointSubsystem.h" // ★ Subsystem

ASavePointTrigger::ASavePointTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    SpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint"));
    SpawnPoint->SetupAttachment(RootComponent);

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);

    TriggerBox->InitBoxExtent(FVector(64.f, 64.f, 64.f));
}

void ASavePointTrigger::BeginPlay()
{
    Super::BeginPlay();
    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASavePointTrigger::OnTriggerBegin);
    }
}

void ASavePointTrigger::OnTriggerBegin(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (bAlreadyActivated && bOneShot) return;
    if (!OtherActor) return;

    if (bOnlyPlayerPawn)
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
        if (OtherActor != PlayerPawn) return;
    }

    ActivateSavePoint(OtherActor);
}

void ASavePointTrigger::ActivateSavePoint(AActor* Activator)
{
    if (bAlreadyActivated && bOneShot) return;

    if (USpawnPointSubsystem* Sub = USpawnPointSubsystem::Get(this))
    {
        const FTransform SaveT = ComputeSaveTransform();
        Sub->SetSpawnPoint(SaveT);
        bAlreadyActivated = true;

        PrintSavedTransform(SaveT, Activator);
        OnSavePointActivated.Broadcast(Activator);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ASavePointTrigger: SpawnPointSubsystem not found."));
    }
}

FTransform ASavePointTrigger::ComputeSaveTransform() const
{
    if (bUseActorTransform)
        return GetActorTransform();
    return SpawnPoint ? SpawnPoint->GetComponentTransform() : GetActorTransform();
}

void ASavePointTrigger::PrintSavedTransform(const FTransform& T, AActor* Activator) const
{
    const FVector L = T.GetLocation();
    const FRotator R = T.Rotator();

    UE_LOG(LogTemp, Log, TEXT("[SavePoint] Saved -> Loc(%.2f, %.2f, %.2f) Rot(%.1f, %.1f, %.1f)"),
        L.X, L.Y, L.Z, R.Roll, R.Pitch, R.Yaw);

    if (bPrintOnScreen && GEngine)
    {
        FString Who = Activator ? Activator->GetName() : TEXT("Unknown");
        GEngine->AddOnScreenDebugMessage(
            -1, 3.0f, FColor::Green,
            FString::Printf(TEXT("SavePoint %s\nLoc(%.1f, %.1f, %.1f)\nRot(%.1f, %.1f, %.1f)"),
                *Who, L.X, L.Y, L.Z, R.Roll, R.Pitch, R.Yaw));
    }
}
