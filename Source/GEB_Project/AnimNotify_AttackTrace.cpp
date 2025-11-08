// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_AttackTrace.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "AttackComponent.h"
#include "Attack.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "Math/UnrealMathUtility.h"

void UAnimNotify_AttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;
    UWorld* World = Owner->GetWorld();
    if (!World) return;

    UAttackComponent* AC = Owner->FindComponentByClass<UAttackComponent>();

    // AttackComponent에 소켓 캡슐을 생성해두었다면 그쪽에서 검사하도록 위임
    if (AC && AC->AttackSocketNames.Num() > 0)
    {
        return;
    }
}

