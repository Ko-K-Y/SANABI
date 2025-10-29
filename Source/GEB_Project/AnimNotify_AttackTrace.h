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
    // 소켓을 지정하면 소켓 위치에서, 비워두면 메쉬 루트 위치에서 트레이스
    UPROPERTY(EditAnywhere, Category = "Attack")
    FName SocketName = NAME_None;

    UPROPERTY(EditAnywhere, Category = "Attack")
    float Radius = 50.f;

    // Notify가 발생할 때 호출
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
