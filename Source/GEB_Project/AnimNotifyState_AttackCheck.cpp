// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_AttackCheck.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Actor.h"
#include "HealthComponent.h"


UAnimNotifyState_AttackCheck::UAnimNotifyState_AttackCheck()
{
	// Default Value
	SocketName = FName("Socket");
	AttackRadius = 40.0f;
	DamageAmount = 1;
	bShowDebugTrace = true;
}

void UAnimNotifyState_AttackCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		// 중복 타격 방지 배열 초기화
		HitActors.Empty();

		// 시작 시점의 소켓 위치 저장
		PreviousSocketLocation = MeshComp->GetSocketLocation(SocketName);
	}
}

void UAnimNotifyState_AttackCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (!MeshComp || !MeshComp->GetWorld()) return;

	// 1. 현재 소켓 위치 가져오기
	FVector CurrentSocketLocation = MeshComp->GetSocketLocation(SocketName);

	// 2. 충돌 검사 (Sphere Trace)
	// 이전 위치(Previous)에서 현재 위치(Current)까지 훑습니다.
	TArray<FHitResult> HitResults;
	
	// Trace 채널은 상황에 맞게 변경하세요 (예: UEngineTypes::ConvertToTraceType(ECC_Pawn))
	// 여기서는 Camera 채널을 예시로 썼지만, 보통 'Pawn'이나 커스텀 채널을 씁니다.
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		MeshComp->GetWorld(),
		PreviousSocketLocation,
		CurrentSocketLocation,
		AttackRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn), // 플레이어를 감지할 채널
		false,
		TArray<AActor*>(), // 무시할 액터(자신을 넣어도 됨)
		bShowDebugTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		HitResults,
		true
	);

	// 3. 충돌한 대상 처리
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			
			// 유효한 액터이고, 이번 스윙에서 아직 안 맞았다면
			if (HitActor && !HitActors.Contains(HitActor))
			{
				// 자신(보스)은 때리면 안 되므로 제외
				if (HitActor == MeshComp->GetOwner()) continue;

				// HealthInterface 호출
				UHealthComponent* HealthComp = HitActor->FindComponentByClass<UHealthComponent>();
				if (HealthComp)
				{
					IHealthInterface::Execute_ApplyDamage(HealthComp, DamageAmount);

					GEngine->AddOnScreenDebugMessage(
						-1, 3.f, FColor::Purple,
						FString::Printf(TEXT("HP: %d"), IHealthInterface::Execute_GetCurrentHealth(HealthComp)));
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("No HealthComponent!"));
					UE_LOG(LogTemp, Warning, TEXT("OtherActor does not have HealthComponent!"));
				}

				// 타격 성공으로 기록 (다단 히트 방지)
				HitActors.Add(HitActor);
				
				// (선택사항) 타격 이펙트나 사운드를 여기서 재생하면 타이밍이 정확합니다.
			}
		}
	}

	// 4. 다음 프레임을 위해 현재 위치를 '이전 위치'로 갱신
	PreviousSocketLocation = CurrentSocketLocation;
}

void UAnimNotifyState_AttackCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	// 배열 정리
	HitActors.Empty();
}



