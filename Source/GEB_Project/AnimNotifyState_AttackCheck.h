// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AttackCheck.generated.h"

/**
 * 
 */
UCLASS()
class GEB_PROJECT_API UAnimNotifyState_AttackCheck : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UAnimNotifyState_AttackCheck();

	// NotifyState가 시작될 때 1회 호출
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	// NotifyState가 지속되는 동안 매 프레임 호출
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	// NotifyState가 끝날 때 1회 호출
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	// 공격 판정을 위한 소켓 이름 (예: hand_r)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FName SocketName;

	// 공격 반경 (주먹 크기)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRadius;

	// 데미지 량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	int DamageAmount;

	// 디버그 드로잉 여부 (공격 범위 눈으로 확인)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebugTrace;

private:
	// 이번 공격(스윙)에서 이미 맞은 액터들을 저장 (중복 타격 방지)
	TArray<AActor*> HitActors;

	// 이전 프레임의 소켓 위치 (터널링 방지용)
	FVector PreviousSocketLocation;
};
