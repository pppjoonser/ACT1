// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ACT1MainCharacter.h"
#include "Animation/ACT1CharacterAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "Equipments/ACT1Equipment.h"
#include "Equipments/ACT1GunWeapon.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"

AACT1MainCharacter::AACT1MainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;
	GetCharacterMovement()->BrakingDecelerationFlying = 4096.0f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyMeshFinder(
		TEXT("/Game/Characters/Heroes/Mannequin/Meshes/SKM_Manny.SKM_Manny"));
	static ConstructorHelpers::FClassFinder<UAnimInstance> MannyAnimBlueprintFinder(
		TEXT("/Game/ACT1/Animation/ABP_ACT1_Manny"));
	static ConstructorHelpers::FObjectFinder<UInputAction> DescendActionFinder(
		TEXT("/Game/Input/Actions/IA_Descend.IA_Descend"));
	static ConstructorHelpers::FObjectFinder<UInputAction> FireActionFinder(
		TEXT("/Game/Input/Actions/IA_Weapon_Fire.IA_Weapon_Fire"));
	static ConstructorHelpers::FObjectFinder<UInputAction> ReloadActionFinder(
		TEXT("/Game/Input/Actions/IA_Weapon_Reload.IA_Weapon_Reload"));
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> GroundMappingFinder(
		TEXT("/Game/Input/Mappings/IMC_Default.IMC_Default"));
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> AirMappingFinder(
		TEXT("/Game/Input/Mappings/IMC_Air.IMC_Air"));
	if (MannyMeshFinder.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MannyMeshFinder.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		if (MannyAnimBlueprintFinder.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(MannyAnimBlueprintFinder.Class);
		}
		else
		{
			GetMesh()->SetAnimInstanceClass(UACT1CharacterAnimInstance::StaticClass());
		}
	}

	if (DescendActionFinder.Succeeded())
	{
		FlightDescendAction = DescendActionFinder.Object;
	}

	if (GroundMappingFinder.Succeeded())
	{
		IMCGround = GroundMappingFinder.Object;
	}

	if (AirMappingFinder.Succeeded())
	{
		IMCAir = AirMappingFinder.Object;
	}

	if (FireActionFinder.Succeeded())
	{
		FireAction = FireActionFinder.Object;
	}

	if (ReloadActionFinder.Succeeded())
	{
		ReloadAction = ReloadActionFinder.Object;
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 350.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	RefreshMovementSpeeds();
	UpdateMovementStateFlags();
}

void AACT1MainCharacter::BeginPlay()
{
	Super::BeginPlay();

	RefreshMovementSpeeds();
	ApplyMovementMappingContext();
	UpdateMovementStateFlags();
}

void AACT1MainCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	ApplyMovementMappingContext();
}

void AACT1MainCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (!IsFlightActive())
	{
		VerticalFlightInput = 0.0f;
	}

	ApplyMovementMappingContext();
	RefreshMovementSpeeds();
	UpdateMovementStateFlags();
}

void AACT1MainCharacter::Tick(float DeltaTime)
{
	if (IsFlightActive())
	{
		const float TargetVerticalVelocity = VerticalFlightInput * FlyVerticalSpeed;
		const bool bHasVerticalInput = !FMath::IsNearlyZero(VerticalFlightInput);
		const float VerticalInterpSpeed = bHasVerticalInput ? FlyVerticalAcceleration : FlyVerticalDeceleration;
		CurrentVerticalFlightSpeed = FMath::FInterpTo(
			CurrentVerticalFlightSpeed,
			TargetVerticalVelocity,
			DeltaTime,
			VerticalInterpSpeed);

		FVector CurrentVelocity = GetCharacterMovement()->Velocity;
		CurrentVelocity.Z = CurrentVerticalFlightSpeed;
		GetCharacterMovement()->Velocity = CurrentVelocity;
	}
	else
	{
		CurrentVerticalFlightSpeed = 0.0f;
	}

	Super::Tick(DeltaTime);

	UpdateMovementStateFlags();
}

void AACT1MainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

#if WITH_EDITOR
	PlayerInputComponent->BindKey(EKeys::F6, IE_Pressed, this, &AACT1MainCharacter::Editor_ToggleFlightMode);
#endif
	PlayerInputComponent->BindKey(EKeys::LeftControl, IE_Pressed, this, &AACT1MainCharacter::Input_DescendKeyPressed);
	PlayerInputComponent->BindKey(EKeys::LeftControl, IE_Released, this, &AACT1MainCharacter::Input_DescendKeyReleased);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AACT1MainCharacter::Input_Move);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AACT1MainCharacter::Input_Look);
		}

		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AACT1MainCharacter::Input_SprintStarted);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AACT1MainCharacter::Input_SprintStopped);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, this, &AACT1MainCharacter::Input_SprintStopped);
		}

		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AACT1MainCharacter::Input_JumpOrAscendStarted);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AACT1MainCharacter::Input_JumpOrAscendStopped);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &AACT1MainCharacter::Input_JumpOrAscendStopped);
		}

		if (FlightDescendAction)
		{
			EnhancedInputComponent->BindAction(FlightDescendAction, ETriggerEvent::Started, this, &AACT1MainCharacter::Input_DescendStarted);
			EnhancedInputComponent->BindAction(FlightDescendAction, ETriggerEvent::Triggered, this, &AACT1MainCharacter::Input_DescendStarted);
			EnhancedInputComponent->BindAction(FlightDescendAction, ETriggerEvent::Completed, this, &AACT1MainCharacter::Input_DescendStopped);
			EnhancedInputComponent->BindAction(FlightDescendAction, ETriggerEvent::Canceled, this, &AACT1MainCharacter::Input_DescendStopped);
		}

		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AACT1MainCharacter::Input_FireStarted);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AACT1MainCharacter::Input_FireStopped);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &AACT1MainCharacter::Input_FireStopped);
		}

		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AACT1MainCharacter::Input_Reload);
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AACT1MainCharacter::Input_Interact);
		}

		if (WeaponScrollAction)
		{
			EnhancedInputComponent->BindAction(WeaponScrollAction, ETriggerEvent::Triggered, this, &AACT1MainCharacter::Input_WeaponScroll);
		}
	}
}

#if WITH_EDITOR
void AACT1MainCharacter::Editor_ToggleFlightMode()
{
	if (IsFlightActive())
	{
		ExitFlightMode();
		return;
	}

	SetFlightAllowed(true);
	EnterFlightMode();
}
#endif

void AACT1MainCharacter::SetFlightAllowed(bool bEnabled)
{
	bCanFly = bEnabled;

	if (!bCanFly && IsFlightActive())
	{
		ExitFlightMode();
	}
}

bool AACT1MainCharacter::EnterFlightMode()
{
	if (!bCanFly)
	{
		return false;
	}

	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	return true;
}

void AACT1MainCharacter::ExitFlightMode()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

bool AACT1MainCharacter::IsFlightActive() const
{
	return bIsFlying;
}

bool AACT1MainCharacter::GetCameraAimTarget(FVector& OutAimTarget) const
{
	if (!Camera)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector CameraLocation = Camera->GetComponentLocation();
	const FVector CameraDirection = Camera->GetForwardVector();
	const FVector TraceEnd = CameraLocation + CameraDirection * CameraAimTraceDistance;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CameraAimTrace), false, this);
	QueryParams.AddIgnoredActor(this);
	if (CurrentWeapon)
	{
		QueryParams.AddIgnoredActor(CurrentWeapon);
	}
	for (AACT1GunWeapon* Weapon : WeaponInventory)
	{
		if (Weapon)
		{
			QueryParams.AddIgnoredActor(Weapon);
		}
	}

	const bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		CameraLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams);

	OutAimTarget = bHit ? HitResult.ImpactPoint : TraceEnd;
	return true;
}

bool AACT1MainCharacter::GetMuzzleAimOffset(float& OutYaw, float& OutPitch) const
{
	if (!CurrentWeapon)
	{
		return false;
	}

	FVector AimTarget = FVector::ZeroVector;
	if (!GetCameraAimTarget(AimTarget))
	{
		return false;
	}

	const FTransform MuzzleTransform = CurrentWeapon->GetMuzzleWorldTransform();
	const FVector MuzzleLocation = MuzzleTransform.GetLocation();
	const FVector AimDirection = AimTarget - MuzzleLocation;
	if (AimDirection.IsNearlyZero())
	{
		return false;
	}

	const FRotator MuzzleAimRotation = AimDirection.Rotation();
	const FRotator ActorRotation = GetActorRotation();
	OutYaw = FRotator::NormalizeAxis(MuzzleAimRotation.Yaw - ActorRotation.Yaw);
	OutPitch = FMath::Clamp(FRotator::NormalizeAxis(MuzzleAimRotation.Pitch), -90.0f, 90.0f);
	return true;
}

void AACT1MainCharacter::ApplyMovementMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (IMCGround)
				{
					InputSubsystem->RemoveMappingContext(IMCGround);
				}

				if (IMCAir)
				{
					InputSubsystem->RemoveMappingContext(IMCAir);
				}

				if (IMCGround)
				{
					InputSubsystem->AddMappingContext(IMCGround, 0);
				}

				if (IsFlightActive() && IMCAir)
				{
					InputSubsystem->AddMappingContext(IMCAir, 1);
				}
			}
		}
	}
}

void AACT1MainCharacter::RefreshMovementSpeeds()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	MovementComponent->MaxWalkSpeed = bIsSprinting ? RunSpeed : WalkSpeed;
	MovementComponent->MaxFlySpeed = bIsSprinting ? FlySprintSpeed : FlySpeed;
}

void AACT1MainCharacter::UpdateMovementStateFlags()
{
	const UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		bIsFlying = false;
		bIsFalling = false;
		return;
	}

	bIsFlying = MovementComponent->MovementMode == MOVE_Flying;
	bIsFalling = MovementComponent->IsFalling();
}

void AACT1MainCharacter::Input_Move(const FInputActionValue& InputValue)
{
	const FVector2D MovementVector = InputValue.Get<FVector2D>();
	if (!Controller || MovementVector.IsNearlyZero())
	{
		return;
	}

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AACT1MainCharacter::Input_Look(const FInputActionValue& InputValue)
{
	const FVector2D LookAxisVector = InputValue.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AACT1MainCharacter::Input_SprintStarted(const FInputActionValue& InputValue)
{
	bIsSprinting = true;
	RefreshMovementSpeeds();
}

void AACT1MainCharacter::Input_SprintStopped(const FInputActionValue& InputValue)
{
	bIsSprinting = false;
	RefreshMovementSpeeds();
}

void AACT1MainCharacter::Input_JumpOrAscendStarted(const FInputActionValue& InputValue)
{
	if (IsFlightActive())
	{
		VerticalFlightInput = 1.0f;
		return;
	}

	Jump();
}

void AACT1MainCharacter::Input_JumpOrAscendStopped(const FInputActionValue& InputValue)
{
	if (IsFlightActive())
	{
		VerticalFlightInput = 0.0f;
		return;
	}

	StopJumping();
}

void AACT1MainCharacter::Input_DescendStarted(const FInputActionValue& InputValue)
{
	if (IsFlightActive())
	{
		VerticalFlightInput = -1.0f;
	}
}

void AACT1MainCharacter::Input_DescendStopped(const FInputActionValue& InputValue)
{
	if (IsFlightActive() && VerticalFlightInput < 0.0f)
	{
		VerticalFlightInput = 0.0f;
	}
}

void AACT1MainCharacter::Input_DescendKeyPressed()
{
	if (IsFlightActive())
	{
		VerticalFlightInput = -1.0f;
	}
}

void AACT1MainCharacter::Input_DescendKeyReleased()
{
	if (IsFlightActive() && VerticalFlightInput < 0.0f)
	{
		VerticalFlightInput = 0.0f;
	}
}

void AACT1MainCharacter::Input_FireStarted(const FInputActionValue& InputValue)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void AACT1MainCharacter::Input_FireStopped(const FInputActionValue& InputValue)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void AACT1MainCharacter::Input_Reload(const FInputActionValue& InputValue)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

void AACT1MainCharacter::Input_Interact(const FInputActionValue& InputValue)
{
	TryPickupNearbyEquipment();
}

void AACT1MainCharacter::Input_WeaponScroll(const FInputActionValue& InputValue)
{
	const float ScrollValue = InputValue.Get<float>();
	if (ScrollValue > 0.0f)
	{
		EquipNextWeapon();
	}
	else if (ScrollValue < 0.0f)
	{
		EquipPreviousWeapon();
	}
}

void AACT1MainCharacter::TryPickupNearbyEquipment()
{
	AACT1Equipment* NearbyEquipment = FindNearbyEquipment();
	if (!NearbyEquipment)
	{
		return;
	}

	AACT1GunWeapon* NearbyWeapon = Cast<AACT1GunWeapon>(NearbyEquipment);
	if (!NearbyWeapon)
	{
		return;
	}

	AddWeaponToInventory(NearbyWeapon);
}

AACT1Equipment* AACT1MainCharacter::FindNearbyEquipment() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EquipmentPickupScan), false, this);

	const bool bHasOverlap = World->OverlapMultiByObjectType(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(EquipmentPickupRadius),
		QueryParams);

	if (!bHasOverlap)
	{
		return nullptr;
	}

	AACT1Equipment* NearestEquipment = nullptr;
	float BestDistanceSquared = TNumericLimits<float>::Max();

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AACT1Equipment* Equipment = Cast<AACT1Equipment>(OverlapResult.GetActor());
		if (!Equipment || Equipment->IsEquipped())
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(GetActorLocation(), Equipment->GetActorLocation());
		if (DistanceSquared < BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			NearestEquipment = Equipment;
		}
	}

	return NearestEquipment;
}

void AACT1MainCharacter::AddWeaponToInventory(AACT1GunWeapon* Weapon)
{
	if (!Weapon || WeaponInventory.Contains(Weapon))
	{
		return;
	}

	Weapon->SetOwner(this);
	Weapon->AttachToOwner(EquipmentAttachSocketName);
	WeaponInventory.Add(Weapon);

	const int32 NewWeaponIndex = WeaponInventory.Num() - 1;
	if (!CurrentWeapon)
	{
		EquipWeaponByIndex(NewWeaponIndex);
	}
	else
	{
		Weapon->UnquipItem();
	}
}

void AACT1MainCharacter::EquipWeaponByIndex(int32 NewWeaponIndex)
{
	if (!WeaponInventory.IsValidIndex(NewWeaponIndex))
	{
		return;
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->UnquipItem();
	}

	CurrentWeaponIndex = NewWeaponIndex;
	CurrentWeapon = WeaponInventory[CurrentWeaponIndex];

	if (CurrentWeapon)
	{
		CurrentWeapon->EquipItem();
		CurrentWeapon->AttachToOwner(EquipmentAttachSocketName);
	}
}

void AACT1MainCharacter::EquipNextWeapon()
{
	const int32 WeaponCount = WeaponInventory.Num();
	if (WeaponCount <= 1)
	{
		return;
	}

	const int32 NewWeaponIndex = (CurrentWeaponIndex + 1 + WeaponCount) % WeaponCount;
	EquipWeaponByIndex(NewWeaponIndex);
}

void AACT1MainCharacter::EquipPreviousWeapon()
{
	const int32 WeaponCount = WeaponInventory.Num();
	if (WeaponCount <= 1)
	{
		return;
	}

	const int32 NewWeaponIndex = (CurrentWeaponIndex - 1 + WeaponCount) % WeaponCount;
	EquipWeaponByIndex(NewWeaponIndex);
}
