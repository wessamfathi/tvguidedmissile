#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGMProjectile.generated.h"

/**
 * Custom projectile class that can be steered by player
 */
UCLASS()
class TGM_API ATGMProjectile : public APawn
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	ATGMProjectile();

	/**
	 * Add input (affecting Pitch) to the Controller's ControlRotation, if it is a local PlayerController.
	 * This value is multiplied by the PlayerController's InputPitchScale value.
	 * @param Val Amount to add to Pitch. This value is multiplied by the PlayerController's InputPitchScale value.
	 * @see PlayerController::InputPitchScale
	 */
	virtual void AddControllerPitchInput(float Val) override;

	/**
	 * Add input (affecting Yaw) to the Controller's ControlRotation, if it is a local PlayerController.
	 * This value is multiplied by the PlayerController's InputYawScale value.
	 * @param Val Amount to add to Yaw. This value is multiplied by the PlayerController's InputYawScale value.
	 * @see PlayerController::InputYawScale
	 */
	virtual void AddControllerYawInput(float Val) override;

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	// Sphere collision component
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComponent;

	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// Explosion audio component
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	class UAudioComponent* ExplosionAudioComponent;

	UPROPERTY()
	class UStaticMeshComponent* ProjectileMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	class UStaticMesh* ProjectileMesh;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	class UMaterial* ProjectileMaterial;

	// Function that initializes the projectile's velocity in the shoot direction.
	void FireInDirection(const FVector& ShootDirection, class APawn* pawnOwner);

protected:
	
	// Follow camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ProjectileCamera;

	UPROPERTY()
	class APawn* PawnOwner;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* ExplosionFX;

	// Base turn rate, in deg/sec. Other scaling may affect final turn rate.
	UPROPERTY(EditDefaultsOnly, Category = Handling)
	float BaseTurnRate;

	// Base look up/down rate, in deg/sec. Other scaling may affect final rate.
	UPROPERTY(EditDefaultsOnly, Category = Handling)
	float BaseLookUpRate;

	// Turn rate multiplier to limit handling
	UPROPERTY(EditDefaultsOnly, Category = Handling)
	float TurnRateMultiplier;

	// Lookup rate multiplier to limit handling
	UPROPERTY(EditDefaultsOnly, Category = Handling)
	float LookUpRateMultiplier;

	// Handling multiplier used when projectile is boosted
	UPROPERTY(EditDefaultsOnly, Category = Handling)
	float BoostHandlingMultiplier;

	// Speed multiplier used when projectile is boosted
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float BoostSpeedMultiplier;

	// Time after which projectile self-destructs
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float ProjectileLifeSpan;

	// Radius within explosion impulse is used
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float ImpulseRadius;

	// Explosion impulse value
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float ImpulseMagnitude;

	// Whether projectile has already been boosted
	bool bIsBoosted;

	// Called when the projectile hits something
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// Simulate explosion shockwave
	void ApplyRadialImpulse();

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	// Explode the projectile and play all relevant FX
	void Explode();

	// Boost projectile speed on player input
	void Boost();
};
