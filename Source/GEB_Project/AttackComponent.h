// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Attack.h"
#include "AttackComponent.generated.h"

class UCapsuleComponent;
class USoundBase; // 추가: 사운드 타입 전방 선언

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEB_PROJECT_API UAttackComponent : public UActorComponent, public IAttack
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackComponent();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSocket")
	TArray<FName> AttackSocketNames;

	// 소켓에 붙일 캡슐 반경/반높이 (블루프린트 조정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSocket")
	float AttackCapsuleRadius = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackSocket")
	float AttackCapsuleHalfHeight = 40.f;

	// 추가: 공격 시 재생할 사운드 (블루프린트에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Sound")
	USoundBase* AttackSound = nullptr;

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

	// 런타임에 생성한 소켓용 캡슐 컴포넌트들
	UPROPERTY()
	TArray<UCapsuleComponent*> SocketCapsules;

	// 현재 공격 중에 이미 히트한 액터 집합 (중복 방지)
	TSet<TWeakObjectPtr<AActor>> RecentlyHitActors;

public:	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PerformAttack_Implementation();
	virtual bool GetisCoolDown_Implementation();
	virtual float GetattackRange_Implementation();

	void OnAttackHit(AActor* Target);

	// 캡슐 콜라이더 오버랩 콜백
	UFUNCTION()
	void OnSocketOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
