// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ACT1CharacterAnimInstance.generated.h"

class UAnimSequence;
class UBlendSpace;

UCLASS()
class ACT1_API UACT1CharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "AimOffset")
	UBlendSpace* GetAimOffset() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
	UAnimSequence* GetBaseAnimation() const;

	UFUNCTION(BlueprintCallable, Category = "AimOffset")
	void SetAimOffset(UBlendSpace* NewAimOffset);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetBaseAnimation(UAnimSequence* NewBaseAnimation);

	UFUNCTION(BlueprintCallable, Category = "AimOffset")
	void ResetAimOffset();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void ResetBaseAnimation();

	UFUNCTION(BlueprintCallable, Category = "AimOffset")
	void SetAOUse(bool bNewAOUse);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float NormalizedSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Forward = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Direction360 = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Right = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float VerticalSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "AimOffset")
	float CameraYaw = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "AimOffset")
	float CameraPitch = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AimOffset")
	TObjectPtr<UBlendSpace> DefaultAimOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimOffset")
	TObjectPtr<UBlendSpace> CurrentAimOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> DefaultBaseAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> CurrentBaseAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimOffset")
	bool bIsAOUse = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFlying = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Velocity = FVector::ZeroVector;

private:
	UPROPERTY(Transient)
	TObjectPtr<class AACT1MainCharacter> OwningCharacter;
};
