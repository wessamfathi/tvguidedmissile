#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "TGMHUD.generated.h"

/**
 * Custom HUD class to render crosshairs
 */
UCLASS()
class TGM_API ATGMHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* CrosshairTexture;

public:
	virtual void DrawHUD() override;
};
