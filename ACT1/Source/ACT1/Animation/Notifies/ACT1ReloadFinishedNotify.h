// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ACT1ReloadFinishedNotify.generated.h"

class UAnimSequenceBase;
class USkeletalMeshComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnACT1ReloadFinishedNotify, USkeletalMeshComponent*, UAnimSequenceBase*);

UCLASS(meta = (DisplayName = "ACT1 Reload Finished"))
class ACT1_API UACT1ReloadFinishedNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	static FOnACT1ReloadFinishedNotify OnReloadFinished;

	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
