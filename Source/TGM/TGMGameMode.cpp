// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGMGameMode.h"
#include "TGMCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATGMGameMode::ATGMGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
