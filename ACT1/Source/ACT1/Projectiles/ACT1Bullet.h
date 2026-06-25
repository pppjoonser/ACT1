// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACT1Bullet.generated.h"

class UProjectileMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UStaticMesh;
class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class ACT1_API AACT1Bullet : public AActor
{
	GENERATED_BODY()

public:
	AACT1Bullet();

	void InitializeProjectile(float InDamage, float InSpeed);
	void IgnoreActor(AActor* ActorToIgnore);

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet|Visual")
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet|VFX")
	TObjectPtr<UNiagaraComponent> TrailComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet|VFX")
	TObjectPtr<UNiagaraSystem> TrailEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet|Visual")
	TObjectPtr<UStaticMesh> VisualMeshAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet|Visual")
	FVector VisualMeshScale = FVector(0.05f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet|Visual")
	bool bShowVisualMesh = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	float Damage = 10.0f;
};
