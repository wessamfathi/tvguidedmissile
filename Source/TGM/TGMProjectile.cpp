#include "TGMProjectile.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "TGMCharacter.h"

// Sets default values
ATGMProjectile::ATGMProjectile()
{
	// Set initial input related values
	BaseTurnRate = 1.0f;
	BaseLookUpRate = 1.0f;
	TurnRateMultiplier = 0.06f;
	LookUpRateMultiplier = 0.06f;

	// Input modifiers used when player boosts the projectile
	BoostHandlingMultiplier = 0.333f;
	BoostSpeedMultiplier = 2.0f;
	bIsBoosted = false;

	// Explosion related values
	ImpulseRadius = 300.0f;
	ImpulseMagnitude = 500000.0f;

 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));

	// Use a sphere as a simple collision representation.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// Set the sphere's collision radius.
	CollisionComponent->InitSphereRadius(15.0f);
	// Set the root component to be the collision component.
	RootComponent = CollisionComponent;

	// Use this component to drive this projectile's movement.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 1500.0f;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	// Create the static mesh component for this projectile
	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));

	if (ProjectileMeshComponent != nullptr)
	{
		ProjectileMeshComponent->SetStaticMesh(ProjectileMesh);
		ProjectileMeshComponent->SetMaterial(0, ProjectileMaterial);
	}

	// Create a follow camera with special VFX
	ProjectileCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	ProjectileCamera->SetupAttachment(RootComponent); // Attach the camera to character's root component
	ProjectileCamera->bUsePawnControlRotation = true; // Camera follows pawn controller rotation
	ProjectileCamera->PostProcessSettings.bOverride_ColorSaturation = true;
	ProjectileCamera->PostProcessSettings.ColorSaturation = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	ProjectileCamera->PostProcessSettings.bOverride_GrainIntensity = true;
	ProjectileCamera->PostProcessSettings.bOverride_GrainJitter = true;
	ProjectileCamera->PostProcessSettings.bOverride_VignetteIntensity = true;

	TargetColorSaturation = 0;
	TargetGrainIntensity = 0.6f;
	TargetVignetteIntensity = 0.8f;
	TargetGrainJitter = 1.0f;

	MaxCameraLerpTime = 0.1f;

	// Create the explosion audio component
	ExplosionAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ExplosionAudioComponent"));
	ExplosionAudioComponent->SetupAttachment(RootComponent);
	ExplosionAudioComponent->bAutoActivate = false;
	ExplosionAudioComponent->bStopWhenOwnerDestroyed = false;

	// Projectile should self-destruct after set time
	ProjectileLifeSpan = 7.0f;

	// Set the projectile's collision profile
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

	// Callback when the projectile hits something
	CollisionComponent->OnComponentHit.AddDynamic(this, &ATGMProjectile::OnHit);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATGMProjectile::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATGMProjectile::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATGMProjectile::LookUpAtRate);

	// Custom projectile input controls
	PlayerInputComponent->BindAction("Explode", IE_Pressed, this, &ATGMProjectile::Explode);
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ATGMProjectile::Boost);
}

void ATGMProjectile::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATGMProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Set a timer to explode the projectile after its lifespan is over
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATGMProjectile::Explode, ProjectileLifeSpan, false);
}

void ATGMProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Set the movement component velocity manually every frame so that it follows the rotation
	ProjectileMovementComponent->Velocity = Controller->GetControlRotation().Vector() * ProjectileMovementComponent->MaxSpeed;

	// Interpolate camera post-process settings until finished
	if (CameraLerpTimeLeft > 0.0f)
	{
		CameraLerpTimeLeft -= DeltaTime;

		float alpha = 1.0f - (CameraLerpTimeLeft / MaxCameraLerpTime);
		float colorSaturation = FMath::Lerp(1.0f, TargetColorSaturation, alpha);

		ProjectileCamera->PostProcessSettings.ColorSaturation = FVector4::FVector4(colorSaturation, colorSaturation, colorSaturation, colorSaturation);
		ProjectileCamera->PostProcessSettings.GrainIntensity = FMath::Lerp(0.0f, TargetGrainIntensity, alpha);
		ProjectileCamera->PostProcessSettings.GrainJitter = FMath::Lerp(0.0f, TargetGrainJitter, alpha);
		ProjectileCamera->PostProcessSettings.VignetteIntensity = FMath::Lerp(0.0f, TargetVignetteIntensity, alpha);
	}
}

void ATGMProjectile::AddControllerYawInput(float Val)
{
	// Use a custom TurnRateMultiplier to limit handling
	Val = Val * TurnRateMultiplier;
	Super::AddControllerYawInput(Val);
}

void ATGMProjectile::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATGMProjectile::AddControllerPitchInput(float Val)
{
	// Use a custom LookUpRateMultiplier to limit handling
	Val = Val * LookUpRateMultiplier;
	Super::AddControllerPitchInput(Val);
}

void ATGMProjectile::FireInDirection(const FVector& ShootDirection, ATGMCharacter* pawnOwner)
{
	CameraLerpTimeLeft = MaxCameraLerpTime;
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	ProjectileCamera->SetActive(true);
	PawnOwner = pawnOwner;
	CollisionComponent->IgnoreActorWhenMoving(PawnOwner, true);
}

void ATGMProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void ATGMProjectile::Explode()
{
	// Spawn and activate explosion VFX
	UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, GetActorLocation(), GetActorRotation(), true);
	PSC->ActivateSystem();

	ExplosionAudioComponent->Play();

	// Simulate projectile shockwave
	ApplyRadialImpulse();

	// Let the controller possess the original character
	if (Controller != nullptr)
	{
		PawnOwner->SetActorRotation(PawnOwner->GetOldRotation());
		Controller->Possess(PawnOwner);
	}

	// Finally projectile should be destroyed
	Destroy();
}

void ATGMProjectile::ApplyRadialImpulse()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// Get all components within ImpulseRadius from explosion center
	FVector ActorLocation = GetActorLocation();
	TArray<UPrimitiveComponent*> OutComponents;
	UKismetSystemLibrary::SphereOverlapComponents(this, ActorLocation, ImpulseRadius, ObjectTypes, nullptr, ActorsToIgnore, OutComponents);

	FVector Impulse;
	UPrimitiveComponent* Component;
	for (int32 i = 0; i < OutComponents.Num(); i++)
	{
		Component = OutComponents[i];

		// Add impulse from explosion center to component location
		Impulse = (Component->GetComponentLocation() - ActorLocation).GetSafeNormal() * ImpulseMagnitude;
		OutComponents[i]->AddImpulse(Impulse);
	}
}

void ATGMProjectile::Boost()
{
	// Projectile can only be boosted once
	if (!bIsBoosted)
	{
		bIsBoosted = true;

		// Limit handling even more
		LookUpRateMultiplier *= BoostHandlingMultiplier;
		TurnRateMultiplier *= BoostHandlingMultiplier;

		// Increase projectile velocity and max speed by boost factor
		ProjectileMovementComponent->Velocity *= BoostSpeedMultiplier;
		ProjectileMovementComponent->MaxSpeed *= BoostSpeedMultiplier;
	}
}