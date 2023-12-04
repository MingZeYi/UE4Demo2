// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode_Main.h"
#include "TPSCharacter.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget_HUD.h"

// Constructor
AGameMode_Main::AGameMode_Main() {
	DefaultPawnClass = ATPSCharacter::StaticClass();
}

void AGameMode_Main::BeginPlay() {
	Super::BeginPlay();

    TSubclassOf<UMyUserWidget_HUD> class_HUD = LoadClass<UMyUserWidget_HUD>(this, TEXT("WidgetBlueprint'/Game/UI/BPH_HUD.BPH_HUD_C'"));
    if (class_HUD) {
        pHUD = CreateWidget<UMyUserWidget_HUD>(GetWorld(), class_HUD);
        pHUD->AddToViewport();
    }
}
