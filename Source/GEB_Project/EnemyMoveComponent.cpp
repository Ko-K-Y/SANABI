// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMoveComponent.h"

// Sets default values for this component's properties
UEnemyMoveComponent::UEnemyMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You 
	// turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	traceRange = 1000.f;
	movementSpeed = 600.f;
	jumpSpeed = 100.f;
	// ...
}


// Called when the game starts
void UEnemyMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UEnemyMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UEnemyMoveComponent::GettraceRange_Implementation() {
	return traceRange;
}

float UEnemyMoveComponent::GetmovementSpeed_Implementation() {
	return movementSpeed;
}

float UEnemyMoveComponent::GetjumpSpeed_Implementation() {
	return jumpSpeed;
}

