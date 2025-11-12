// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ShieldInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UShieldInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GEB_PROJECT_API IShieldInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int GetShield();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int ApplyDamageToShield(int damage);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsShieldActive();
};
