// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACT1Equipment.generated.h"

class USkeletalMesh;
class USkeletalMeshComponent;
class USphereComponent;
class UTexture2D;

UCLASS()
class ACT1_API AACT1Equipment : public AActor
{
	GENERATED_BODY()

public:
	AACT1Equipment();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	FORCEINLINE USkeletalMesh* GetMeshAsset() const { return MeshAsset; }
	FORCEINLINE UTexture2D* GetEquipmentIcon() const { return EquipmentIcon; }
	FORCEINLINE bool IsEquipped() const { return bIsEquipped; }

public:
	virtual void EquipItem();
	virtual void UnquipItem();
	virtual void AttachToOwner(FName SocketName);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Mesh")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Pickup")
	TObjectPtr<USphereComponent> PickupCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Mesh")
	TObjectPtr<USkeletalMesh> MeshAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Icon")
	TObjectPtr<UTexture2D> EquipmentIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Pickup")
	float PickupColliderRadius = 32.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|State")
	bool bIsEquipped = false;
};
