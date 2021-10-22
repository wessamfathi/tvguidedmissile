// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "TGMProjectile.generated.h"

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

protected:
	
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ProjectileCamera;

	UPROPERTY()
	class APawn* PawnOwner;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* ExplosionFX;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float TurnRateMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float LookUpRateMultiplier;

	float BoostMultiplier;

	float BoostAccelerationFactor;

	float ProjectileLifeSpan;

	float ImpulseRadius;

	float ImpulseMagnitude;

	bool bIsBoosted;

	// Called when the projectile hits something
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

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

	void Explode();

	void Boost();
	
public:	

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	// Sphere collision component
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComponent;

	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

	// Projectile mesh
	UPROPERTY()
	UStaticMeshComponent* ProjectileMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	UStaticMesh* ProjectileMesh;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	UMaterial* ProjectileMaterial;

	// Function that initializes the projectile's velocity in the shoot direction.
	void FireInDirection(const FVector& ShootDirection, class APawn* pawnOwner);
};
