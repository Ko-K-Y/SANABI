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

    // AttackComponent가 있으면 그 컴포넌트의 attackRange를 우선 사용하고,
    // 없으면 Notify에 설정된 Radius를 사용한다.
    float UseRadius = Radius;
    UAttackComponent* AC = Owner->FindComponentByClass<UAttackComponent>();
    if (AC)
    {
        UseRadius = IAttack::Execute_GetattackRange(AC) / 2;
    }

    const FVector OwnerLocation = Owner->GetActorLocation();
    const FVector Center = OwnerLocation;

    // 오버랩으로 Pawn(피격대상) 검색
    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    FCollisionObjectQueryParams ObjQuery;
    ObjQuery.AddObjectTypesToQuery(ECC_Pawn); // Pawn 타입만 검사

    const FCollisionShape Sphere = FCollisionShape::MakeSphere(UseRadius);

    if (World->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity, ObjQuery, Sphere, Params))
    {
        const FVector Forward = Owner->GetActorForwardVector().GetSafeNormal();

        for (const FOverlapResult& Res : Overlaps)
        {
            AActor* HitActor = Res.GetActor();
            if (!HitActor || HitActor == Owner) continue;

            // 앞쪽만 체크: 소유자 전방 벡터와 대상 방향의 내적이 양수이면 전방 반구
            const FVector ToHit = (HitActor->GetActorLocation() - OwnerLocation);
            if (ToHit.IsNearlyZero()) continue;
            const float Dot = FVector::DotProduct(Forward, ToHit.GetSafeNormal());

            // Dot > 0 => 전방 반구만 허용. 필요하면 0.5f 등으로 각도 제한 가능.
            if (Dot <= 0.f) continue;

            // AttackComponent가 있으면 OnAttackHit 호출(컴포넌트 내부에서 중복 방지/데미지 처리)
            if (AC)
            {
                AC->OnAttackHit(HitActor);
            }
        }
    }

#if ENABLE_DRAW_DEBUG
    // 디버그용: 액터 중심에 구 표시 (앞쪽만 시각화하려면 추가 처리 필요)
    DrawDebugSphere(World, Center, UseRadius, 12, FColor::Red, false, 0.5f);
    // 전방 방향 표시
    DrawDebugDirectionalArrow(World, Center, Center + Owner->GetActorForwardVector().GetSafeNormal() * UseRadius, 20.f, FColor::Green, false, 0.5f, 0, 2.f);
#endif
}

