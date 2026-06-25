// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ACT1GamePlayTag.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ACT1StateComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACT1_API UACT1StateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UACT1StateComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE void SetCurrentState(const FGameplayTag NewState)
	{
		if (CurrentState == ACT1GameplayTags::Character_State_Death)
		{
			return;
		}

		CurrentState = NewState;
	}

	FORCEINLINE FGameplayTag GetCurrentState() const { return CurrentState; }

	void ClearCurrentState();

	bool IsCurrentStateEqualToAny(const FGameplayTagContainer& TagsToCheck) const;

public:
	FORCEINLINE bool MovementInputEnabled() { return bMovementInputEnabled; }

	void ToggleMovementInput(bool bEnabled, float Duration = 0.f);

	UFUNCTION()
	void MovementInputEnableAction();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	FGameplayTag CurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	bool bMovementInputEnabled = true;
};
