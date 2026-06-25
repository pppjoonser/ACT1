// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ACT1GunWeaponData.generated.h"

class AACT1Bullet;
class AACT1GunWeapon;
class UAnimMontage;
class UAnimSequence;
class UBlendSpace;
class UCRRecoilPattern;
class UNiagaraSystem;

UCLASS(BlueprintType)
class ACT1_API UACT1GunWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AACT1GunWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UBlendSpace> AimOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> BaseAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
	float FireInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
	bool bIsAutomatic = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 MagazineSize = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
	TObjectPtr<UCRRecoilPattern> RecoilPattern;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
	float RecoilStrength = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float BulletSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float BulletSpread = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AACT1Bullet> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|VFX")
	TObjectPtr<UNiagaraSystem> TracerEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile|VFX")
	float TracerDistance = 10000.0f;
};
