// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateInterface.h"
#include "PlayerStateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEB_PROJECT_API UPlayerStateComponent : public UActorComponent, public IStateInterface
{
	GENERATED_BODY()
	
	UPROPERTY()
	class ACharacter* Character;

public:	
	// Sets default values for this component's properties
	UPlayerStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// *** 무적 시간 ***
public:
	bool bIsAttacked; // 플레이어가 공격 당한 상태인지 확인, 공격 당하면 일정 시간 동안 무적 상태
protected:
	FTimerHandle InvincibilityTimerHandle;
	const float InvincibilityDuration = 1.0f; // 무적 지속 시간
public:
	virtual void Invincibility_Implementation() override;
	virtual void ResetInvincibility_Implementation() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
