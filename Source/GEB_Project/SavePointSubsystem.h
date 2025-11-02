#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SavePointSubsystem.generated.h"

/** 현재 월드의 '세이브 포인트'를 보관하는 매우 단순한 서브시스템 */
UCLASS()
class GEB_PROJECT_API USavePointSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SavePoint")
	void SetSavePoint(const FTransform& InTransform)
	{
		bHasSavePoint = true;
		CurrentSavePoint = InTransform;
	}

	UFUNCTION(BlueprintPure, Category = "SavePoint")
	bool HasSavePoint() const { return bHasSavePoint; }

	UFUNCTION(BlueprintPure, Category = "SavePoint")
	FTransform GetSavePoint() const { return CurrentSavePoint; }

private:
	UPROPERTY(VisibleAnywhere, Category = "SavePoint")
	bool bHasSavePoint = false;

	UPROPERTY(VisibleAnywhere, Category = "SavePoint")
	FTransform CurrentSavePoint;
};
