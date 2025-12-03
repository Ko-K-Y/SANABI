// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileExplosion.generated.h"

class UBoxComponent;

UCLASS()
class GEB_PROJECT_API AMissileExplosion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissileExplosion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	int DamageAmount = 1;

	// 액터 전체의 오버랩 이벤트를 감지하는 가상 함수 오버라이드
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
