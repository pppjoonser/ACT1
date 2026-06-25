// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ACT1MainCharacter.generated.h"

class AACT1Equipment;
class AACT1GunWeapon;
struct FInputActionValue;

UCLASS()
class ACT1_API AACT1MainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AACT1MainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Flight")
	void SetFlightAllowed(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Flight")
	bool EnterFlightMode();

	UFUNCTION(BlueprintCallable, Category = "Flight")
	void ExitFlightMode();

	UFUNCTION(BlueprintPure, Category = "Flight")
	bool IsFlightActive() const;

	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsSprinting() const { return bIsSprinting; }

	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsFlyingState() const { return bIsFlying; }

	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsFallingState() const { return bIsFalling; }

	UFUNCTION(BlueprintPure, Category = "Aim")
	bool GetCameraAimTarget(FVector& OutAimTarget) const;

	UFUNCTION(BlueprintPure, Category = "Aim")
	bool GetMuzzleAimOffset(float& OutYaw, float& OutPitch) const;

private:
	void ApplyMovementMappingContext();
	void RefreshMovementSpeeds();
	void UpdateMovementStateFlags();
#if WITH_EDITOR
	void Editor_ToggleFlightMode();
#endif
	void Input_Move(const FInputActionValue& InputValue);
	void Input_Look(const FInputActionValue& InputValue);
	void Input_SprintStarted(const FInputActionValue& InputValue);
	void Input_SprintStopped(const FInputActionValue& InputValue);
	void Input_JumpOrAscendStarted(const FInputActionValue& InputValue);
	void Input_JumpOrAscendStopped(const FInputActionValue& InputValue);
	void Input_DescendStarted(const FInputActionValue& InputValue);
	void Input_DescendStopped(const FInputActionValue& InputValue);
	void Input_DescendKeyPressed();
	void Input_DescendKeyReleased();
	void Input_FireStarted(const FInputActionValue& InputValue);
	void Input_FireStopped(const FInputActionValue& InputValue);
	void Input_Reload(const FInputActionValue& InputValue);
	void Input_Interact(const FInputActionValue& InputValue);
	void Input_WeaponScroll(const FInputActionValue& InputValue);
	void TryPickupNearbyEquipment();
	AACT1Equipment* FindNearbyEquipment() const;
	void AddWeaponToInventory(AACT1GunWeapon* Weapon);
	void EquipWeaponByIndex(int32 NewWeaponIndex);
	void EquipNextWeapon();
	void EquipPreviousWeapon();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> IMCGround;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> IMCAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> FlightDescendAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> DescendAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> WeaponScrollAction;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> Camera;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	float WalkSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	float RunSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	float FlySpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	float FlySprintSpeed = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	float FlyVerticalSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	float FlyVerticalAcceleration = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStat")
	float FlyVerticalDeceleration = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flight")
	bool bCanFly = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	bool bIsFlying = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	bool bIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	float VerticalFlightInput = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	float CurrentVerticalFlightSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	float EquipmentPickupRadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aim")
	float CameraAimTraceDistance = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FName EquipmentAttachSocketName = TEXT("Weapon_Equip_Socket");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TArray<TObjectPtr<AACT1GunWeapon>> WeaponInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	int32 CurrentWeaponIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<AACT1GunWeapon> CurrentWeapon;
};
