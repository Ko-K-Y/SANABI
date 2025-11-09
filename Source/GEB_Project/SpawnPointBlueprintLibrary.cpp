#include "SpawnPointBlueprintLibrary.h"
#include "SpawnPointSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

USpawnPointSubsystem* USpawnPointBlueprintLibrary::GetSubsystem(const UObject* WorldContextObject)
{
    return USpawnPointSubsystem::Get(WorldContextObject);
}

bool USpawnPointBlueprintLibrary::GetSpawnPointTransform(const UObject* WorldContextObject, FTransform& OutTransform)
{
    if (USpawnPointSubsystem* Sub = USpawnPointSubsystem::Get(WorldContextObject))
    {
        if (Sub->HasSpawnPoint())
        {
            OutTransform = Sub->GetSpawnPoint();
            return true;
        }
    }
    OutTransform = FTransform::Identity;
    return false;
}

void USpawnPointBlueprintLibrary::SetSpawnPointTransform(const UObject* WorldContextObject, const FTransform& InTransform)
{
    if (USpawnPointSubsystem* Sub = USpawnPointSubsystem::Get(WorldContextObject))
    {
        Sub->SetSpawnPoint(InTransform);
    }
}

void USpawnPointBlueprintLibrary::SetSpawnPointFromActor(const UObject* WorldContextObject, const AActor* Actor)
{
    if (!Actor) return;
    if (USpawnPointSubsystem* Sub = USpawnPointSubsystem::Get(WorldContextObject))
    {
        Sub->SetSpawnPoint(Actor->GetActorTransform());
    }
}

bool USpawnPointBlueprintLibrary::TeleportPlayerToSpawnPoint(const UObject* WorldContextObject, int32 PlayerIndex)
{
    if (USpawnPointSubsystem* Sub = USpawnPointSubsystem::Get(WorldContextObject))
    {
        if (Sub->HasSpawnPoint())
        {
            if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(WorldContextObject, PlayerIndex))
            {
                Pawn->SetActorTransform(Sub->GetSpawnPoint());
                return true;
            }
        }
    }
    return false;
}

void USpawnPointBlueprintLibrary::ClearSpawnPoint(const UObject* WorldContextObject)
{
    if (USpawnPointSubsystem* Sub = USpawnPointSubsystem::Get(WorldContextObject))
    {
        Sub->ClearSpawnPoint();
    }
}
