// Copyright Epic Games, Inc. All Rights Reserved.

#include "TVGuidedMissileGameMode.h"
#include "TVGuidedMissileCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATVGuidedMissileGameMode::ATVGuidedMissileGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
