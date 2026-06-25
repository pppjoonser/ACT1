// Fill out your copyright notice in the Description page of Project Settings.


#include "ACT1GameMode.h"
#include "UObject/ConstructorHelpers.h"

AACT1GameMode::AACT1GameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> MainCharacterClassFinder(
		TEXT("/Game/Blueprint/BP_MainCharacter"));
	if (MainCharacterClassFinder.Succeeded())
	{
		DefaultPawnClass = MainCharacterClassFinder.Class;
	}
}
