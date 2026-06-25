// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ACT1StateComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UACT1StateComponent::UACT1StateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UACT1StateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UACT1StateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UACT1StateComponent::ClearCurrentState()
{
	CurrentState = FGameplayTag();
}

bool UACT1StateComponent::IsCurrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const
{
	return TagsToCheck.HasTagExact(CurrentState);
}

void UACT1StateComponent::ToggleMovementInput(bool bEnabled, float Duration)
{
	bMovementInputEnabled = bEnabled;

	if (!bEnabled && Duration > 0.0f)
	{
		if (UWorld* World = GetWorld())
		{
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(
				TimerHandle,
				this,
				&UACT1StateComponent::MovementInputEnableAction,
				Duration,
				false);
		}
	}
}

void UACT1StateComponent::MovementInputEnableAction()
{
	bMovementInputEnabled = true;
}

