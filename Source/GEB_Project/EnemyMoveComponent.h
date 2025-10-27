// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyMove.h"
#include "EnemyMoveComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEB_PROJECT_API UEnemyMoveComponent : public UActorComponent, public IEnemyMove
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyMoveComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	float traceRange;
	float movementSpeed;
	float jumpSpeed;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual float GettraceRange_Implementation();
	virtual float GetmovementSpeed_Implementation();
	virtual float GetjumpSpeed_Implementation();
		
};
