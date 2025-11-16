// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CheckDistance.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTD_CheckDistance::UBTD_CheckDistance()
{
	NodeName = "Check Distance";
}

bool UBTD_CheckDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!ControlledPawn) return false;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(ControlledPawn->GetWorld(), 0);
	if (!PlayerPawn) return false;

	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerPawn->GetActorLocation());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	float BB_Distance = BB->GetValueAsFloat(DistanceKey.SelectedKeyName);

	return (Distance <= BB_Distance);
}