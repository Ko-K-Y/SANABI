// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackTrace.generated.h"

UCLASS()
class GEB_PROJECT_API UAnimNotify_AttackTrace : public UAnimNotify
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, Category = "Attack")
    float Radius = 50.f;

    // Notify가 발생할 때 호출
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
