#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SavePointGISubsystem.generated.h"

/** GameInstance 전역에서 현재 세이브 포인트 Transform을 보관 */
UCLASS()
class GEB_PROJECT_API USavePointGISubsystem : public UGameInstanceSubsystem
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
