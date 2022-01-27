// Copyright Epic Games, Inc. All Rights Reserved.

#include "DalosGameMode.h"
#include "DalosCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADalosGameMode::ADalosGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
