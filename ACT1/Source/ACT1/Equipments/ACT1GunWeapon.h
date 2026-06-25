// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipments/ACT1Equipment.h"
#include "ACT1GunWeapon.generated.h"

class AACT1Bullet;
class UACT1GunWeaponData;
class UAnimInstance;
class UAnimSequenceBase;
class UCRRecoilComponent;
class UNiagaraSystem;
class USkeletalMeshComponent;

UCLASS()
class ACT1_API AACT1GunWeapon : public AACT1Equipment
{
	GENERATED_BODY()

public:
	AACT1GunWeapon();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void EquipItem() override;
	virtual void UnquipItem() override;

	void InitializeFromData(UACT1GunWeaponData* InWeaponData);
	void StartFire();
	void StopFire();
	bool Fire();
	void Reload();
	FTransform GetMuzzleWorldTransform() const;

	FORCEINLINE UACT1GunWeaponData* GetWeaponData() const { return WeaponData; }
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmo; }

protected:
	FTransform GetMuzzleTransform() const;
	FRotator GetShotRotation(const FRotator& BaseRotation) const;
	float GetFireInterval() const;
	void UpdateAutomaticFire();
	void ConfigureRecoil();
	void ApplyAnimationData();
	bool PlayFireMontage() const;
	bool FireShot();
	void SpawnTracerEffect(const FVector& StartLocation, const FVector& ShotDirection) const;
	UNiagaraSystem* GetTracerEffect() const;
	UAnimInstance* GetOwnerAnimInstance() const;
	bool IsReloadNotifyForOwner(USkeletalMeshComponent* MeshComp) const;
	void HandleReloadFinishedNotify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
	void FinishReload();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UACT1GunWeaponData> WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName = TEXT("Muzzle");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Recoil")
	TObjectPtr<UCRRecoilComponent> RecoilComponent;

	float LastFireTime = -1000.0f;
	bool bWantsToFire = false;
};
