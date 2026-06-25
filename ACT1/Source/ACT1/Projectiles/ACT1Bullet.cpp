// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectiles/ACT1Bullet.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "UObject/ConstructorHelpers.h"

AACT1Bullet::AACT1Bullet()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(5.0f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionResponseToAllChannels(ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Collision->OnComponentHit.AddDynamic(this, &AACT1Bullet::OnBulletHit);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletVisual"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMesh->SetRelativeScale3D(VisualMeshScale);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultVisualMeshFinder(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (DefaultVisualMeshFinder.Succeeded())
	{
		VisualMeshAsset = DefaultVisualMeshFinder.Object;
		VisualMesh->SetStaticMesh(VisualMeshAsset);
	}

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = Collision;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailComponent"));
	TrailComponent->SetupAttachment(RootComponent);
	TrailComponent->bAutoActivate = true;

	InitialLifeSpan = 5.0f;
}

void AACT1Bullet::InitializeProjectile(float InDamage, float InSpeed)
{
	Damage = InDamage;

	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = InSpeed;
		ProjectileMovement->MaxSpeed = InSpeed;
		ProjectileMovement->Velocity = GetActorForwardVector() * InSpeed;
	}
}

void AACT1Bullet::IgnoreActor(AActor* ActorToIgnore)
{
	if (Collision && ActorToIgnore)
	{
		Collision->IgnoreActorWhenMoving(ActorToIgnore, true);
	}
}

void AACT1Bullet::BeginPlay()
{
	Super::BeginPlay();

	if (TrailComponent && TrailEffect)
	{
		TrailComponent->SetAsset(TrailEffect);
		TrailComponent->Activate(true);
	}

	if (VisualMesh)
	{
		VisualMesh->SetStaticMesh(VisualMeshAsset);
		VisualMesh->SetRelativeScale3D(VisualMeshScale);
		VisualMesh->SetHiddenInGame(!bShowVisualMesh);
	}
}

void AACT1Bullet::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (TrailComponent)
	{
		TrailComponent->SetAsset(TrailEffect);
	}

	if (VisualMesh)
	{
		VisualMesh->SetStaticMesh(VisualMeshAsset);
		VisualMesh->SetRelativeScale3D(VisualMeshScale);
		VisualMesh->SetHiddenInGame(!bShowVisualMesh);
	}
}

void AACT1Bullet::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("ACT1Bullet hit %s at %s"),
		OtherActor ? *OtherActor->GetName() : TEXT("None"),
		*Hit.ImpactPoint.ToString());

	if (OtherActor && OtherActor != GetOwner())
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetActorForwardVector(), Hit, GetInstigatorController(), this, UDamageType::StaticClass());
	}

	Destroy();
}
