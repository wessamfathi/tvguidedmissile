// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGMGameMode.h"
#include "TGMHUD.h"
#include "TGMCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATGMGameMode::ATGMGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATGMHUD::StaticClass();
}
