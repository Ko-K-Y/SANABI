// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Attack.h"
#include "AttackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEB_PROJECT_API UAttackComponent : public UActorComponent, public IAttack
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackStat", meta = (AllowPrivateAccess = "true"))
	int32 damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackStat", meta = (AllowPrivateAccess = "true"))
	float maxAttackCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackStat", meta = (AllowPrivateAccess = "true"))
	float attackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackStat", meta = (AllowPrivateAccess = "true"))
	bool isCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackStat", meta = (AllowPrivateAccess = "true"))
	float coolTime;


public:	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PerformAttack_Implementation();
	virtual bool GetisCoolDown_Implementation();
	virtual float GetattackRange_Implementation();

	void OnAttackHit(AActor* Target);
};
