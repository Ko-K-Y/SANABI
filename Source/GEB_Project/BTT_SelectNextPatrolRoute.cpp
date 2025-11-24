// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SelectNextPatrolRoute.h"
#include "EnemyAIController.h"
#include "Components/SplineComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_SelectNextPatrolRoute::UBTT_SelectNextPatrolRoute() {
	NodeName = TEXT("Select Next Patrol Route");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_SelectNextPatrolRoute::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	AEnemyAIController* AICon = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (!AICon || !AICon->Spline) {
		return EBTNodeResult::Failed;
	}

	USplineComponent* Spline = AICon->Spline;
	int32 NumPoints = Spline->GetNumberOfSplinePoints();
	if(NumPoints == 0) {
		return EBTNodeResult::Failed;
	}

	FVector NextLoc = Spline->GetLocationAtSplinePoint(AICon->CurrentSplinePointIndex, ESplineCoordinateSpace::World);

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(NextLocationKey.SelectedKeyName, NextLoc);

	AICon->CurrentSplinePointIndex = (AICon->CurrentSplinePointIndex + 1) % NumPoints;

	return EBTNodeResult::Succeeded;
}

