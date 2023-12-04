// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget_HUD.h"

UMyUserWidget_HUD::UMyUserWidget_HUD(const FObjectInitializer& objectInitializer) : Super(objectInitializer) {
    currentAmmo = FText::FromString("30");
    allAmmo = FText::FromString("60");
}


void UMyUserWidget_HUD::AddAmmo(const int32 nAmmo) {
    int32 nAllAmmo = FCString::Atoi(*allAmmo.ToString());
    nAllAmmo += nAmmo;
    allAmmo = FText::FromString(FString::FromInt(nAllAmmo));
}

void UMyUserWidget_HUD::Reload() {
    currentAmmo = FText::FromString("30");
}

void UMyUserWidget_HUD::Shoot() {
    int32 nCurrentAmmo = FCString::Atoi(*currentAmmo.ToString());
    nCurrentAmmo -= 1;
    currentAmmo = FText::FromString(FString::FromInt(nCurrentAmmo));
}