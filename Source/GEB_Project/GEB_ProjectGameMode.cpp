// Copyright Epic Games, Inc. All Rights Reserved.

#include "GEB_ProjectGameMode.h"
#include "GEB_ProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGEB_ProjectGameMode::AGEB_ProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
