// LevelTransitionTrigger.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionTrigger.generated.h"

class UBoxComponent;

UCLASS()
class GEB_PROJECT_API ALevelTransitionTrigger : public AActor
{
    GENERATED_BODY()
public:
    ALevelTransitionTrigger();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Trigger")
    UBoxComponent* Box;

    // 열고 싶은 맵 이름 (에디터에서 지정)
    UPROPERTY(EditAnywhere, Category = "Transition")
    FName NextLevelName;

    UFUNCTION()
    void OnBeginOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);
};

