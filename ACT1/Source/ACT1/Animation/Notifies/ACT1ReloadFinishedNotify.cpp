// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notifies/ACT1ReloadFinishedNotify.h"

FOnACT1ReloadFinishedNotify UACT1ReloadFinishedNotify::OnReloadFinished;

FString UACT1ReloadFinishedNotify::GetNotifyName_Implementation() const
{
	return TEXT("ACT1 Reload Finished");
}

void UACT1ReloadFinishedNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	OnReloadFinished.Broadcast(MeshComp, Animation);
}
