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

    // AttackComponent�� ������ �� ������Ʈ�� attackRange�� �켱 ����ϰ�,
    // ������ Notify�� ������ Radius�� ����Ѵ�.
    float UseRadius = Radius;
    UAttackComponent* AC = Owner->FindComponentByClass<UAttackComponent>();
    if (AC)
    {
        UseRadius = IAttack::Execute_GetattackRange(AC) / 2;
    }

    const FVector OwnerLocation = Owner->GetActorLocation();
    const FVector Center = OwnerLocation;

    // ���������� Pawn(�ǰݴ��) �˻�
    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    FCollisionObjectQueryParams ObjQuery;
    ObjQuery.AddObjectTypesToQuery(ECC_Pawn); // Pawn Ÿ�Ը� �˻�

    const FCollisionShape Sphere = FCollisionShape::MakeSphere(UseRadius);

    if (World->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity, ObjQuery, Sphere, Params))
    {
        const FVector Forward = Owner->GetActorForwardVector().GetSafeNormal();

        for (const FOverlapResult& Res : Overlaps)
        {
            AActor* HitActor = Res.GetActor();
            if (!HitActor || HitActor == Owner) continue;

            // ���ʸ� üũ: ������ ���� ���Ϳ� ��� ������ ������ ����̸� ���� �ݱ�
            const FVector ToHit = (HitActor->GetActorLocation() - OwnerLocation);
            if (ToHit.IsNearlyZero()) continue;
            const float Dot = FVector::DotProduct(Forward, ToHit.GetSafeNormal());

            // Dot > 0 => ���� �ݱ��� ���. �ʿ��ϸ� 0.5f ������ ���� ���� ����.
            if (Dot <= 0.f) continue;

            // AttackComponent�� ������ OnAttackHit ȣ��(������Ʈ ���ο��� �ߺ� ����/������ ó��)
            if (AC)
            {
                AC->OnAttackHit(HitActor);
            }
        }
    }

#if ENABLE_DRAW_DEBUG
    // ����׿�: ���� �߽ɿ� �� ǥ�� (���ʸ� �ð�ȭ�Ϸ��� �߰� ó�� �ʿ�)
    DrawDebugSphere(World, Center, UseRadius, 12, FColor::Red, false, 0.5f);
    // ���� ���� ǥ��
    DrawDebugDirectionalArrow(World, Center, Center + Owner->GetActorForwardVector().GetSafeNormal() * UseRadius, 20.f, FColor::Green, false, 0.5f, 0, 2.f);
#endif
}

