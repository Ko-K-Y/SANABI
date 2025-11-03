#include "SpawnPointGameInstance.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

USpawnPointGameInstance::USpawnPointGameInstance()
    : bHasSpawnPoint(false)
{
    LastSpawnTransform = FTransform::Identity;
}

void USpawnPointGameInstance::Init()
{
    Super::Init();
    // 레벨 리로드 간 값을 유지하면 되므로 추가 작업 없음
}

void USpawnPointGameInstance::Shutdown()
{
    Super::Shutdown();
}

void USpawnPointGameInstance::SetSpawnPoint(const FTransform& InTransform)
{
    LastSpawnTransform = InTransform;
    bHasSpawnPoint = true;
}

void USpawnPointGameInstance::ClearSpawnPoint()
{
    LastSpawnTransform = FTransform::Identity;
    bHasSpawnPoint = false;
}

USpawnPointGameInstance* USpawnPointGameInstance::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    const UWorld* World = WorldContextObject->GetWorld();
    if (!World) return nullptr;

    return Cast<USpawnPointGameInstance>(World->GetGameInstance());
}
