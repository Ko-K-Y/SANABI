#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASavePointTrigger.generated.h"

class UBoxComponent;
class UArrowComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSavePointActivated, AActor*, Activator);

UCLASS()
class GEB_PROJECT_API ASavePointTrigger : public AActor
{
    GENERATED_BODY()

public:
    ASavePointTrigger();

    UFUNCTION(BlueprintCallable, Category = "SavePoint")
    void ActivateSavePoint(AActor* Activator = nullptr);

    UPROPERTY(BlueprintAssignable, Category = "SavePoint")
    FOnSavePointActivated OnSavePointActivated;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnTriggerBegin(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

private:
    UPROPERTY(VisibleAnywhere, Category = "SavePoint", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* TriggerBox;

    UPROPERTY(VisibleAnywhere, Category = "SavePoint", meta = (AllowPrivateAccess = "true"))
    UArrowComponent* SpawnPoint;

    UPROPERTY(EditAnywhere, Category = "SavePoint")
    bool bOneShot = true;

    UPROPERTY(EditAnywhere, Category = "SavePoint")
    bool bUseActorTransform = false;

    UPROPERTY(EditAnywhere, Category = "SavePoint")
    bool bOnlyPlayerPawn = true;

    UPROPERTY(EditAnywhere, Category = "SavePoint")
    bool bPrintOnScreen = true;

    bool bAlreadyActivated = false;

    FTransform ComputeSaveTransform() const;
    void PrintSavedTransform(const FTransform& T, AActor* Activator) const;
};
