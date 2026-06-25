// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/ACT1CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ACT1MainCharacter.h"

void UACT1CharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningCharacter = Cast<AACT1MainCharacter>(TryGetPawnOwner());
	ResetAimOffset();
	ResetBaseAnimation();
}

void UACT1CharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningCharacter)
	{
		OwningCharacter = Cast<AACT1MainCharacter>(TryGetPawnOwner());
	}

	if (!OwningCharacter)
	{
		return;
	}

	Velocity = OwningCharacter->GetVelocity();
	VerticalSpeed = Velocity.Z;
	const FRotator ActorRotation = OwningCharacter->GetActorRotation();
	float MuzzleAimYaw = 0.0f;
	float MuzzleAimPitch = 0.0f;
	if (OwningCharacter->GetMuzzleAimOffset(MuzzleAimYaw, MuzzleAimPitch))
	{
		CameraYaw = MuzzleAimYaw;
		CameraPitch = MuzzleAimPitch;
	}
	else
	{
		const FRotator AimRotation = OwningCharacter->GetBaseAimRotation();
		CameraYaw = FRotator::NormalizeAxis(AimRotation.Yaw - ActorRotation.Yaw);
		CameraPitch = FMath::Clamp(FRotator::NormalizeAxis(AimRotation.Pitch), -90.0f, 90.0f);
	}

	FVector HorizontalVelocity = Velocity;
	HorizontalVelocity.Z = 0.0f;
	const FVector ActorForward = OwningCharacter->GetActorForwardVector();
	const FVector ActorRight = OwningCharacter->GetActorRightVector();
	const float ForwardVelocity = FVector::DotProduct(HorizontalVelocity, ActorForward);
	const float RightVelocity = FVector::DotProduct(HorizontalVelocity, ActorRight);
	Speed = ForwardVelocity;
	Right = RightVelocity;
	Direction360 = 0.0f;

	if (!FMath::IsNearlyZero(ForwardVelocity) || !FMath::IsNearlyZero(RightVelocity))
	{
		Direction360 = FMath::RadiansToDegrees(FMath::Atan2(RightVelocity, ForwardVelocity));
	}

	if (const UCharacterMovementComponent* CharacterMovement = OwningCharacter->GetCharacterMovement())
	{
		bIsFalling = CharacterMovement->IsFalling();
		bIsInAir = bIsFalling;
		bIsFlying = OwningCharacter->IsFlyingState();
	}
	else
	{
		bIsFalling = false;
		bIsInAir = false;
		bIsFlying = false;
	}

	bIsSprinting = OwningCharacter->IsSprinting();
}

UBlendSpace* UACT1CharacterAnimInstance::GetAimOffset() const
{
	return CurrentAimOffset ? CurrentAimOffset.Get() : DefaultAimOffset.Get();
}

UAnimSequence* UACT1CharacterAnimInstance::GetBaseAnimation() const
{
	return CurrentBaseAnimation ? CurrentBaseAnimation.Get() : DefaultBaseAnimation.Get();
}

void UACT1CharacterAnimInstance::SetAimOffset(UBlendSpace* NewAimOffset)
{
	CurrentAimOffset = NewAimOffset ? NewAimOffset : DefaultAimOffset.Get();
}

void UACT1CharacterAnimInstance::SetBaseAnimation(UAnimSequence* NewBaseAnimation)
{
	CurrentBaseAnimation = NewBaseAnimation ? NewBaseAnimation : DefaultBaseAnimation.Get();
}

void UACT1CharacterAnimInstance::ResetAimOffset()
{
	CurrentAimOffset = DefaultAimOffset;
	bIsAOUse = false;
}

void UACT1CharacterAnimInstance::ResetBaseAnimation()
{
	CurrentBaseAnimation = DefaultBaseAnimation;
}

void UACT1CharacterAnimInstance::SetAOUse(bool bNewAOUse)
{
	bIsAOUse = bNewAOUse;
}
