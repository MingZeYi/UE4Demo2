// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget_HUD.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UMyUserWidget_HUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UMyUserWidget_HUD(const FObjectInitializer& objectInitializer);

    UFUNCTION(BlueprintImplementableEvent)
    void Equip();
    UFUNCTION(BlueprintImplementableEvent)
    void UnEquip();

    void AddAmmo(const int32 nAmmo);
    void Reload();
    void Shoot();

    UPROPERTY(BlueprintReadOnly)
    FText currentAmmo;
    UPROPERTY(BlueprintReadOnly)
    FText allAmmo;
};
