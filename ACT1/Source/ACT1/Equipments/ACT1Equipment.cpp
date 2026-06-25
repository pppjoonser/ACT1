// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipments/ACT1Equipment.h"

#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/Character.h"

AACT1Equipment::AACT1Equipment()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentMesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	PickupCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollider"));
	PickupCollider->SetupAttachment(Mesh);
	PickupCollider->SetSphereRadius(PickupColliderRadius);
	PickupCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupCollider->SetGenerateOverlapEvents(true);
}

void AACT1Equipment::BeginPlay()
{
	Super::BeginPlay();
}

void AACT1Equipment::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MeshAsset)
	{
		Mesh->SetSkeletalMesh(MeshAsset);
	}

	if (PickupCollider)
	{
		PickupCollider->SetSphereRadius(PickupColliderRadius);
	}
}

void AACT1Equipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AACT1Equipment::EquipItem()
{
	bIsEquipped = true;
	SetActorEnableCollision(false);
}

void AACT1Equipment::UnquipItem()
{
	bIsEquipped = false;
	SetActorEnableCollision(true);
}

void AACT1Equipment::AttachToOwner(FName SocketName)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	USkeletalMeshComponent* CharacterMesh = OwnerCharacter->GetMesh();
	if (!CharacterMesh)
	{
		return;
	}

	AttachToComponent(CharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
}
