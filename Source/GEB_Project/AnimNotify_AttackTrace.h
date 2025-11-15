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
    // ������ �����ϸ� ���� ��ġ����, ����θ� �޽� ��Ʈ ��ġ���� Ʈ���̽�
    UPROPERTY(EditAnywhere, Category = "Attack")
    FName SocketName = NAME_None;

    UPROPERTY(EditAnywhere, Category = "Attack")
    float Radius = 50.f;

    // Notify�� �߻��� �� ȣ��
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
