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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyMoveStat", meta = (AllowPrivateAccess = "true"))
	float traceRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyMoveStat", meta = (AllowPrivateAccess = "true"))
	float movementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyMoveStat", meta = (AllowPrivateAccess = "true"))
	float jumpSpeed;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual float GettraceRange_Implementation();
	virtual float GetmovementSpeed_Implementation();
	virtual float GetjumpSpeed_Implementation();
		
};
