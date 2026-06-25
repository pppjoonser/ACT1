// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipments/ACT1GunWeapon.h"

#include "Animation/ACT1CharacterAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/Notifies/ACT1ReloadFinishedNotify.h"
#include "CollisionQueryParams.h"
#include "Components/CRRecoilComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Equipments/ACT1GunWeaponData.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Player/ACT1MainCharacter.h"
#include "Projectiles/ACT1Bullet.h"

AACT1GunWeapon::AACT1GunWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	RecoilComponent = CreateDefaultSubobject<UCRRecoilComponent>(TEXT("RecoilComponent"));
}

void AACT1GunWeapon::BeginPlay()
{
	Super::BeginPlay();

	InitializeFromData(WeaponData);
	UACT1ReloadFinishedNotify::OnReloadFinished.AddUObject(this, &AACT1GunWeapon::HandleReloadFinishedNotify);
}

void AACT1GunWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UACT1ReloadFinishedNotify::OnReloadFinished.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

void AACT1GunWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAutomaticFire();
}

void AACT1GunWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	InitializeFromData(WeaponData);
}

void AACT1GunWeapon::EquipItem()
{
	bIsEquipped = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(false);
	ConfigureRecoil();
	ApplyAnimationData();
}

void AACT1GunWeapon::UnquipItem()
{
	bIsEquipped = false;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bWantsToFire = false;
}

void AACT1GunWeapon::InitializeFromData(UACT1GunWeaponData* InWeaponData)
{
	if (!InWeaponData)
	{
		return;
	}

	WeaponData = InWeaponData;
	CurrentAmmo = WeaponData->MagazineSize;
	ConfigureRecoil();
}

void AACT1GunWeapon::StartFire()
{
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACT1GunWeapon fire blocked: WeaponData is null on %s"), *GetName());
		return;
	}

	bWantsToFire = true;
	if (RecoilComponent)
	{
		RecoilComponent->StartShooting();
	}

	Fire();

	if (!WeaponData->bIsAutomatic)
	{
		bWantsToFire = false;
	}
}

void AACT1GunWeapon::StopFire()
{
	bWantsToFire = false;
}

bool AACT1GunWeapon::Fire()
{
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACT1GunWeapon fire blocked: WeaponData is null on %s"), *GetName());
		return false;
	}

	if (bIsReloading)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACT1GunWeapon fire blocked: reloading on %s"), *GetName());
		return false;
	}

	if (CurrentAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACT1GunWeapon fire blocked: no ammo on %s"), *GetName());
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACT1GunWeapon fire blocked: world is null on %s"), *GetName());
		return false;
	}

	const float TimeBetweenShots = GetFireInterval();
	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - LastFireTime < TimeBetweenShots)
	{
		return false;
	}

	return FireShot();
}

bool AACT1GunWeapon::FireShot()
{
	if (!WeaponData || bIsReloading || CurrentAmmo <= 0)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACT1GunWeapon fire blocked: world is null on %s"), *GetName());
		return false;
	}

	const TSubclassOf<AACT1Bullet> BulletClass = WeaponData->BulletClass;
	if (!BulletClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACT1GunWeapon fire blocked: BulletClass is null on %s"), *GetName());
		return false;
	}

	const FTransform MuzzleTransform = GetMuzzleTransform();
	const FRotator ShotRotation = GetShotRotation(MuzzleTransform.Rotator());

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AACT1Bullet* Bullet = World->SpawnActor<AACT1Bullet>(BulletClass, MuzzleTransform.GetLocation(), ShotRotation, SpawnParams);
	if (!Bullet)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACT1GunWeapon fire blocked: SpawnActor failed on %s"), *GetName());
		return false;
	}

	Bullet->IgnoreActor(GetOwner());
	Bullet->IgnoreActor(this);
	Bullet->InitializeProjectile(WeaponData->Damage, WeaponData->BulletSpeed);
	SpawnTracerEffect(MuzzleTransform.GetLocation(), ShotRotation.Vector());
	--CurrentAmmo;
	LastFireTime = World->GetTimeSeconds();

	if (RecoilComponent)
	{
		RecoilComponent->ApplyShot();
	}

	PlayFireMontage();

	UE_LOG(LogTemp, Log, TEXT("ACT1GunWeapon spawned bullet %s from %s"), *Bullet->GetName(), *GetName());
	return true;
}

void AACT1GunWeapon::Reload()
{
	if (!WeaponData || bIsReloading || CurrentAmmo >= WeaponData->MagazineSize)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (!AnimInstance || !WeaponData->ReloadMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACT1GunWeapon reload blocked: missing anim instance or ReloadMontage on %s"), *GetName());
		return;
	}

	const float MontageLength = AnimInstance->Montage_Play(WeaponData->ReloadMontage);
	if (MontageLength <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACT1GunWeapon reload blocked: failed to play ReloadMontage on %s"), *GetName());
		return;
	}

	bWantsToFire = false;
	bIsReloading = true;

	if (UACT1CharacterAnimInstance* ACT1AnimInstance = Cast<UACT1CharacterAnimInstance>(AnimInstance))
	{
		ACT1AnimInstance->SetAOUse(false);
	}
}

FTransform AACT1GunWeapon::GetMuzzleWorldTransform() const
{
	return GetMuzzleTransform();
}

FTransform AACT1GunWeapon::GetMuzzleTransform() const
{
	if (Mesh && Mesh->DoesSocketExist(MuzzleSocketName))
	{
		return Mesh->GetSocketTransform(MuzzleSocketName);
	}

	return GetActorTransform();
}

FRotator AACT1GunWeapon::GetShotRotation(const FRotator& BaseRotation) const
{
	FRotator ShotRotation = BaseRotation;
	if (const AACT1MainCharacter* OwnerCharacter = Cast<AACT1MainCharacter>(GetOwner()))
	{
		FVector AimTarget = FVector::ZeroVector;
		if (OwnerCharacter->GetCameraAimTarget(AimTarget))
		{
			const FVector MuzzleLocation = GetMuzzleTransform().GetLocation();
			const FVector AimDirection = AimTarget - MuzzleLocation;
			if (!AimDirection.IsNearlyZero())
			{
				ShotRotation = AimDirection.Rotation();
			}
		}
	}

	if (!WeaponData || WeaponData->BulletSpread <= 0.0f)
	{
		return ShotRotation;
	}

	const FVector ShotDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ShotRotation.Vector(), WeaponData->BulletSpread);
	return ShotDirection.Rotation();
}

float AACT1GunWeapon::GetFireInterval() const
{
	if (!WeaponData)
	{
		return 0.0f;
	}

	if (WeaponData->FireInterval > 0.0f)
	{
		return WeaponData->FireInterval;
	}

	const float AttackSpeed = FMath::Max(WeaponData->AttackSpeed, KINDA_SMALL_NUMBER);
	return 1.0f / AttackSpeed;
}

void AACT1GunWeapon::UpdateAutomaticFire()
{
	if (!WeaponData || !bIsEquipped || !WeaponData->bIsAutomatic || !bWantsToFire)
	{
		return;
	}

	Fire();
}

void AACT1GunWeapon::ConfigureRecoil()
{
	if (!WeaponData || !RecoilComponent)
	{
		return;
	}

	RecoilComponent->SetRecoilPattern(WeaponData->RecoilPattern);
	RecoilComponent->SetRecoilStrength(WeaponData->RecoilStrength);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	if (AController* OwnerController = OwnerPawn->GetController())
	{
		RecoilComponent->SetTargetController(OwnerController);
	}
}

void AACT1GunWeapon::ApplyAnimationData()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !OwnerCharacter->GetMesh())
	{
		return;
	}

	if (UACT1CharacterAnimInstance* AnimInstance = Cast<UACT1CharacterAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance()))
	{
		AnimInstance->SetAimOffset(WeaponData ? WeaponData->AimOffset : nullptr);
		AnimInstance->SetBaseAnimation(WeaponData ? WeaponData->BaseAnimation : nullptr);
		AnimInstance->SetAOUse(true);
	}
}

bool AACT1GunWeapon::PlayFireMontage() const
{
	if (!WeaponData || !WeaponData->FireMontage)
	{
		return false;
	}

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (!AnimInstance)
	{
		return false;
	}

	const float MontageLength = WeaponData->FireMontage->GetPlayLength();
	const float TargetDuration = FMath::Max(GetFireInterval(), KINDA_SMALL_NUMBER);
	const float PlayRate = FMath::Clamp(MontageLength / TargetDuration, 0.1f, 10.0f);

	return AnimInstance->Montage_Play(WeaponData->FireMontage, PlayRate) > 0.0f;
}

void AACT1GunWeapon::SpawnTracerEffect(const FVector& StartLocation, const FVector& ShotDirection) const
{
	UWorld* World = GetWorld();
	UNiagaraSystem* TracerEffect = GetTracerEffect();
	if (!World || !TracerEffect)
	{
		return;
	}

	const float TraceDistance = WeaponData ? FMath::Max(WeaponData->TracerDistance, 100.0f) : 10000.0f;
	const FVector EndLocation = StartLocation + ShotDirection.GetSafeNormal() * TraceDistance;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ACT1WeaponTracer), false, GetOwner());
	QueryParams.AddIgnoredActor(this);

	const bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams);

	const FVector ImpactLocation = bHit ? HitResult.ImpactPoint : EndLocation;
	UNiagaraComponent* TracerComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		TracerEffect,
		StartLocation,
		ShotDirection.Rotation(),
		FVector::OneVector,
		true,
		false);

	if (!TracerComponent)
	{
		return;
	}

	TArray<FVector> ImpactPositions;
	ImpactPositions.Add(ImpactLocation);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
		TracerComponent,
		TEXT("User.ImpactPositions"),
		ImpactPositions);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
		TracerComponent,
		TEXT("User.ImpactPositions"),
		ImpactPositions);

	TracerComponent->Activate(true);
}

UNiagaraSystem* AACT1GunWeapon::GetTracerEffect() const
{
	if (WeaponData && WeaponData->TracerEffect)
	{
		return WeaponData->TracerEffect;
	}

	static TWeakObjectPtr<UNiagaraSystem> CachedDefaultTracer;
	if (!CachedDefaultTracer.IsValid())
	{
		CachedDefaultTracer = LoadObject<UNiagaraSystem>(
			nullptr,
			TEXT("/Game/Effects/Particles/Weapons/NS_WeaponFire_Tracer.NS_WeaponFire_Tracer"));
	}

	return CachedDefaultTracer.Get();
}

UAnimInstance* AACT1GunWeapon::GetOwnerAnimInstance() const
{
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !OwnerCharacter->GetMesh())
	{
		return nullptr;
	}

	return OwnerCharacter->GetMesh()->GetAnimInstance();
}

bool AACT1GunWeapon::IsReloadNotifyForOwner(USkeletalMeshComponent* MeshComp) const
{
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	return OwnerCharacter && OwnerCharacter->GetMesh() == MeshComp;
}

void AACT1GunWeapon::HandleReloadFinishedNotify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!bIsReloading || !IsReloadNotifyForOwner(MeshComp))
	{
		return;
	}

	FinishReload();
}

void AACT1GunWeapon::FinishReload()
{
	if (!WeaponData)
	{
		bIsReloading = false;
		return;
	}

	CurrentAmmo = WeaponData->MagazineSize;
	bIsReloading = false;

	if (bIsEquipped)
	{
		ApplyAnimationData();
	}

	UE_LOG(LogTemp, Log, TEXT("ACT1GunWeapon reload finished on %s"), *GetName());
}
