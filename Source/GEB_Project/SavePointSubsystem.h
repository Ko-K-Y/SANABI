#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SavePointSubsystem.generated.h"

/** 
미사용
*/
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
