// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TGMHUD.generated.h"

UCLASS()
class ATGMHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATGMHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	UPROPERTY(EditDefaultsOnly, Category = HUD)
	float DeltaX;

	UPROPERTY(EditDefaultsOnly, Category = HUD)
	float DeltaY;
};

