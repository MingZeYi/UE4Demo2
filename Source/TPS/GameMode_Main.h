// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyUserWidget_HUD.h"
#include "GameMode_Main.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API AGameMode_Main : public AGameMode
{
	GENERATED_BODY()
	
public:
	AGameMode_Main();

	virtual void BeginPlay() override;

	UMyUserWidget_HUD* pHUD;
};
