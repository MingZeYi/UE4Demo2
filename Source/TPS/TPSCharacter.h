// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"

#include "TPSCharacter.generated.h"

UCLASS()
class TPS_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void StartJump();
	void ToggleCrouch();
	void ToggleEquip();
	void Aim_Start();
	void Aim_End();
	void Shoot();
	void Reload();

	void timerFunction_Jump();
	void timerFunction_AimTransition();
	void timerFunction_Equip();
	void timerFunction_UnEquip();
	void timerFunction_Shoot();

	FTimerHandle timerHandle_Jump;
	FTimerHandle timerHandle_AimTransition;
	FTimerHandle timerHandle_Equip;
	FTimerHandle timerHandle_UnEquip;
	FTimerHandle timerHandle_Shoot;

	class UCurveFloat* pCurveCamera;
	FTimeline* pTimeline_CrouchCamera;
	FTimeline* pTimeline_AimCamera;
	FOnTimelineFloat timelineCallback_CrouchCamera;
	FOnTimelineFloat timelineCallback_AimCamera;

	FVector currentCrouchCameraLocation;
	FVector desiredCrouchCameraLocation;
	FVector currentAimCameraLocation;
	FVector desiredAimCameraLocation;

	float currentFov;
	float desiredFov;

	UAnimationAsset* pPoseARAnim;
	UAnimationAsset* pEquipHolsterAnim;
	UAnimationAsset* pEquipARAnim;
	UAnimSequence* pEquipCharacterAnim;
	UAnimationAsset* pUnEquipHolsterAnim;
	UAnimationAsset* pUnEquipARAnim;
	UAnimSequence* pUnEquipCharacterAnim;
	UAnimationAsset* pReloadARAnim;
	UAnimSequence* pReloadAnim;

	UFUNCTION()
	void timelineCallback_Function_CrouchCamera(float Value);
	UFUNCTION()
	void timelineCallback_Function_AimCamera(float Value);

	float axisMoveForward;
	float axisMoveRight;

	TSubclassOf<UCameraShakeBase> pCameraShake;
	UParticleSystem* pParticleShell;
	UMaterialInterface* pMaterialDecal;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	class USkeletalMeshComponent* pHolsterBelt;
	class USkeletalMeshComponent* pHolster;
	class USkeletalMeshComponent* pGun;
	class USpringArmComponent* pSpring;
	class UCameraComponent* pCamera;

	UPROPERTY(BlueprintReadOnly)
	bool bJump;
	UPROPERTY(BlueprintReadOnly)
	bool bCrouch;
	UPROPERTY(BlueprintReadOnly)
	bool bAim;
	UPROPERTY(BlueprintReadOnly)
	bool bAimTransition;
	UPROPERTY(BlueprintReadOnly)
	bool bEquip;
	UPROPERTY(BlueprintReadOnly)
	bool bShoot;

	UPROPERTY(BlueprintReadOnly)
	float Pitch;
	UPROPERTY(BlueprintReadOnly)
	float Direction;
};
