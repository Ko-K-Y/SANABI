// UIBootSubsystem.h
#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIBootSubsystem.generated.h"

UCLASS()
class GEB_PROJECT_API UUIBootSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:
    void OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS);
    void TrySpawnHUD(UWorld* World);

    TSubclassOf<class UUserWidget> StatusHUDClass = nullptr;
    TWeakObjectPtr<class UUserWidget> SpawnedHUD;
    bool bTriedOnce = false;
};
