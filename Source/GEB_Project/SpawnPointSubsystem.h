#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SpawnPointSubsystem.generated.h"

UCLASS()
class GEB_PROJECT_API USpawnPointSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // 서브시스템 라이프사이클
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // 저장/조회/초기화
    UFUNCTION(BlueprintCallable, Category = "SpawnPoint")
    void SetSpawnPoint(const FTransform& InTransform);

    UFUNCTION(BlueprintPure, Category = "SpawnPoint")
    const FTransform& GetSpawnPoint() const { return LastSpawnTransform; }

    UFUNCTION(BlueprintPure, Category = "SpawnPoint")
    bool HasSpawnPoint() const { return bHasSpawnPoint; }

    UFUNCTION(BlueprintCallable, Category = "SpawnPoint")
    void ClearSpawnPoint();

    // 어디서든 안전하게 받아오는 헬퍼
    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "SpawnPoint")
    static USpawnPointSubsystem* Get(const UObject* WorldContextObject);

private:
    UPROPERTY(VisibleAnywhere, Category = "SpawnPoint")
    FTransform LastSpawnTransform = FTransform::Identity;

    UPROPERTY(VisibleAnywhere, Category = "SpawnPoint")
    bool bHasSpawnPoint = false;
};
