// Fill out your copyright notice in the Description page of Project Settings.


#include "TGMProjectile.h"
#include "Camera/CameraComponent.h"

// Sets default values
ATGMProjectile::ATGMProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (RootComponent == nullptr)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (CollisionComponent == nullptr)
	{
		// Use a sphere as a simple collision representation.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set the sphere's collision radius.
		CollisionComponent->InitSphereRadius(15.0f);
		// Set the root component to be the collision component.
		RootComponent = CollisionComponent;
	}

	if (ProjectileMovementComponent == nullptr)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if (ProjectileMeshComponent == nullptr)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}

		static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Engine/EngineMaterials/NewBaseFlattenMaterial.NewBaseFlattenMaterial'"));

		if (Material.Succeeded())
		{
			ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
		}

		ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
		ProjectileMeshComponent->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
		ProjectileMeshComponent->SetupAttachment(RootComponent);
	}


	// Create a follow camera
	ProjectileCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	ProjectileCamera->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	ProjectileCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	ProjectileCamera->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 0);
	ProjectileCamera->PostProcessSettings.bOverride_ColorSaturation = true;
	ProjectileCamera->PostProcessSettings.GrainIntensity = 0.6f;
	ProjectileCamera->PostProcessSettings.bOverride_GrainIntensity = true;
	ProjectileCamera->PostProcessSettings.bOverride_GrainJitter = true;
	ProjectileCamera->PostProcessSettings.bOverride_VignetteIntensity = true;
	ProjectileCamera->PostProcessSettings.VignetteIntensity = 0.8f;
	ProjectileCamera->PostProcessSettings.GrainJitter = 1.0f;
	ProjectileCamera->SetActive(false);

	InitialLifeSpan = 3.0f;

	// Set the projectile's collision profile
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

	CollisionComponent->OnComponentHit.AddDynamic(this, &ATGMProjectile::OnHit);

	BaseTurnRate = 1.0f;
	BaseLookUpRate = 1.0f;
}

// Called when the game starts or when spawned
void ATGMProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATGMProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	PlayerInputComponent->BindAction("Explode", IE_Pressed, this, &ATGMProjectile::Explode);
}

void ATGMProjectile::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ATGMProjectile::AddControllerYawInput(float Val)
{
	ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity.RotateAngleAxis(Val, FVector(0.0f, 0.0f, 1.0f));
	Super::AddControllerYawInput(Val);
}


void ATGMProjectile::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATGMProjectile::AddControllerPitchInput(float Val)
{
	ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity.RotateAngleAxis(Val, FVector(1.0f, 0.0f, 0.0f));
	Super::AddControllerPitchInput(Val);
}

void ATGMProjectile::FireInDirection(const FVector& ShootDirection, APawn* pawnOwner)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	ProjectileCamera->SetActive(true);
	PawnOwner = pawnOwner;
}

void ATGMProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	}

	Destroy();
}

void ATGMProjectile::Explode()
{
	Destroy();
}

void ATGMProjectile::Destroyed()
{
	ProjectileCamera->SetActive(false);

	if (Controller != nullptr)
	{
		Controller->Possess(PawnOwner);
	}

	Super::Destroyed();
}
