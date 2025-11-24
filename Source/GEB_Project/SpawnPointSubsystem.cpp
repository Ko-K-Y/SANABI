#include "SpawnPointSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void USpawnPointSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void USpawnPointSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void USpawnPointSubsystem::SetSpawnPoint(const FTransform& InTransform)
{
    LastSpawnTransform = InTransform;
    bHasSpawnPoint = true;
}

void USpawnPointSubsystem::ClearSpawnPoint()
{
    LastSpawnTransform = FTransform::Identity;
    bHasSpawnPoint = false;
}

USpawnPointSubsystem* USpawnPointSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;
    const UWorld* World = WorldContextObject->GetWorld();
    if (!World) return nullptr;

    UGameInstance* GI = World->GetGameInstance();
    if (!GI) return nullptr;

    return GI->GetSubsystem<USpawnPointSubsystem>();
}
