// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "GameMode_Main.h"
#include "particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bJump = false;
	bCrouch = false;
	bAim = false;
	bAimTransition = false;
	bEquip = false;
	bShoot = false;

	pHolsterBelt = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HolsterBelt"));
	pHolster = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Holster"));
	pGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	pSpring = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring"));
	pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	pGun->SetupAttachment(GetMesh(), TEXT("Socket_AR"));
	pHolsterBelt->SetupAttachment(GetMesh());
	pHolster->SetupAttachment(GetMesh(), TEXT("Socket_ARHolster"));
	pSpring->SetupAttachment(GetMesh());
	pCamera->SetupAttachment(pSpring, USpringArmComponent::SocketName);

	pHolsterBelt->SetMasterPoseComponent(GetMesh());
	
	// 加载网格资源
	ConstructorHelpers::FObjectFinder<USkeletalMesh> object_Character(TEXT("SkeletalMesh'/Game/Characters/Meshes/SK_Mannequin.SK_Mannequin'"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> object_HolsterBelt(TEXT("SkeletalMesh'/Game/Weapons/ARHolster/Meshes/AR_HolsterBelts.AR_HolsterBelts'"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> object_Holster(TEXT("SkeletalMesh'/Game/Weapons/ARHolster/Meshes/AR_Holster.AR_Holster'"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> object_Gun(TEXT("SkeletalMesh'/Game/Weapons/AssaultRifle/Meshes/SK_AssaultRifle.SK_AssaultRifle'"));
	
	// 若加载成功，赋值模型
	if (object_Character.Succeeded()) GetMesh()->SetSkeletalMesh(object_Character.Object);
	if (object_HolsterBelt.Succeeded()) pHolsterBelt->SetSkeletalMesh(object_HolsterBelt.Object);
	if (object_Holster.Succeeded()) pHolster->SetSkeletalMesh(object_Holster.Object);
	if (object_Gun.Succeeded()) pGun->SetSkeletalMesh(object_Gun.Object);

	// 调整角色位置和朝向
	GetMesh()->SetWorldLocation(FVector(0, 0, -90.f));
	GetMesh()->SetWorldRotation(FQuat(FRotator(0, -90.f, 0)));

	// 加载动画蓝图
	ConstructorHelpers::FClassFinder<UAnimInstance> class_AnimBlueprint(TEXT("AnimBlueprint'/Game/Blueprints/BPA_Character.BPA_Character_C'"));
	if (class_AnimBlueprint.Succeeded()) GetMesh()->SetAnimInstanceClass(class_AnimBlueprint.Class);

	pSpring->SetRelativeLocation(FVector(0, 0, 150.f));
	pSpring->SetRelativeRotation(FQuat(FRotator(0, 90.f, 0)));
	pSpring->SocketOffset = FVector(0, 0, 40.f);
	// 视角移动时伸缩杆移动
	pSpring->bUsePawnControlRotation = true;

	pCamera->SetRelativeRotation(FQuat(FRotator(-10.f, 0, 0)));

	// 视角移动时，角色不动
	this->bUseControllerRotationYaw = false;

	// 角色实现转身
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 限制角色最高移动速度
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	// 加载平滑曲线资源
	static ConstructorHelpers::FObjectFinder<UCurveFloat> object_CurveFloat(TEXT("CurveFloat'/Game/Blueprints/Curve_Camera.Curve_Camera'"));
	if (object_CurveFloat.Succeeded()) pCurveCamera = object_CurveFloat.Object;

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> object_PoseARAnim(TEXT("AnimSequence'/Game/Weapons/AssaultRifle/Animations/A_W_AR_Pose.A_W_AR_Pose'"));
	if (object_PoseARAnim.Succeeded()) pPoseARAnim = object_PoseARAnim.Object;

	// 装备武器动画资源加载
	static ConstructorHelpers::FObjectFinder<UAnimationAsset> object_EquipHolsterAnim(TEXT("AnimSequence'/Game/Weapons/ARHolster/Animations/A_W_AR_Holster_Equip.A_W_AR_Holster_Equip'"));
	if (object_EquipHolsterAnim.Succeeded()) pEquipHolsterAnim = object_EquipHolsterAnim.Object;
	static ConstructorHelpers::FObjectFinder<UAnimationAsset> object_EquipARAnim(TEXT("AnimSequence'/Game/Weapons/AssaultRifle/Animations/A_W_AR_Equip.A_W_AR_Equip'"));
	if (object_EquipARAnim.Succeeded()) pEquipARAnim = object_EquipARAnim.Object;
	static ConstructorHelpers::FObjectFinder<UAnimSequence> object_EquipCharacterAnim(TEXT("AnimSequence'/Game/Characters/Animations/AssaultRifle/Equip/A_AR_Equip.A_AR_Equip'"));
	if (object_EquipCharacterAnim.Succeeded()) pEquipCharacterAnim = object_EquipCharacterAnim.Object;

	// 收起武器动画资源加载
	static ConstructorHelpers::FObjectFinder<UAnimationAsset> object_UnEquipHolsterAnim(TEXT("AnimSequence'/Game/Weapons/ARHolster/Animations/A_W_AR_Holster_UnEquip.A_W_AR_Holster_UnEquip'"));
	if (object_UnEquipHolsterAnim.Succeeded()) pUnEquipHolsterAnim = object_UnEquipHolsterAnim.Object;
	static ConstructorHelpers::FObjectFinder<UAnimationAsset> object_UnEquipARAnim(TEXT("AnimSequence'/Game/Weapons/AssaultRifle/Animations/A_W_AR_UnEquip.A_W_AR_UnEquip'"));
	if (object_UnEquipARAnim.Succeeded()) pUnEquipARAnim = object_UnEquipARAnim.Object;
	static ConstructorHelpers::FObjectFinder<UAnimSequence> object_UnEquipCharacterAnim(TEXT("AnimSequence'/Game/Characters/Animations/AssaultRifle/Equip/A_AR_UnEquip.A_AR_UnEquip'"));
	if (object_UnEquipCharacterAnim.Succeeded()) pUnEquipCharacterAnim = object_UnEquipCharacterAnim.Object;

	// 换弹
	static ConstructorHelpers::FObjectFinder<UAnimationAsset> object_ReloadARAnim(TEXT("AnimSequence'/Game/Weapons/AssaultRifle/Animations/A_W_AR_Reload.A_W_AR_Reload'"));
	if (object_ReloadARAnim.Succeeded()) pReloadARAnim = object_ReloadARAnim.Object;
	static ConstructorHelpers::FObjectFinder<UAnimSequence> object_ReloadAnim(TEXT("AnimSequence'/Game/Characters/Animations/AssaultRifle/A_AR_Reload.A_AR_Reload'"));
	if (object_ReloadAnim.Object) pReloadAnim = object_ReloadAnim.Object;
	
	// 开火时相机抖动类加载
	static ConstructorHelpers::FClassFinder<UCameraShakeBase> class_CameraShake(TEXT("Blueprint'/Game/Blueprints/CameraShake.CameraShake_C'"));
	if (class_CameraShake.Succeeded()) pCameraShake = class_CameraShake.Class;

	// 弹壳粒子
	static ConstructorHelpers::FObjectFinder<UParticleSystem> object_ParticleShell(TEXT("ParticleSystem'/Game/Weapons/Shells/Particles/P_AR_Shell.P_AR_Shell'"));
	if (object_ParticleShell.Succeeded()) pParticleShell = object_ParticleShell.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> object_MaterialDecal(TEXT("Material'/Game/Decals/Bullet_Hole/M_Decal_Bullet_Metall.M_Decal_Bullet_Metall'"));
	if (object_MaterialDecal.Succeeded()) pMaterialDecal = object_MaterialDecal.Object;

}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Crouch Camera
	pTimeline_CrouchCamera = new FTimeline();
	pTimeline_CrouchCamera->SetLooping(false);
	timelineCallback_CrouchCamera.BindUFunction(this, TEXT("timelineCallback_Function_CrouchCamera"));
	pTimeline_CrouchCamera->AddInterpFloat(pCurveCamera, timelineCallback_CrouchCamera);

	// Aim Camera
	pTimeline_AimCamera = new FTimeline();
	pTimeline_AimCamera->SetLooping(false);
	timelineCallback_AimCamera.BindUFunction(this, TEXT("timelineCallback_Function_AimCamera"));
	pTimeline_AimCamera->AddInterpFloat(pCurveCamera, timelineCallback_AimCamera);

}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	UE_LOG(LogTemp, Warning, TEXT("Pitch: %f"), GetControlRotation().Pitch);
	UE_LOG(LogTemp, Warning, TEXT("Yaw: %f"), GetControlRotation().Yaw);
	UE_LOG(LogTemp, Warning, TEXT("Roll: %f"), GetControlRotation().Roll);
	*/

	// 转换Pitch，与动画中的角度对应
	Pitch = GetControlRotation().Pitch;
	if (Pitch >= 270.f && Pitch <= 360.f) {
		Pitch -= 360.f;
	}

	// 确定当前前进方向(WASD)
	FVector directionVector(axisMoveForward, axisMoveRight, 0);
	Direction = directionVector.Rotation().Yaw;

	if (pTimeline_CrouchCamera->IsPlaying()) pTimeline_CrouchCamera->TickTimeline(DeltaTime);
	if (pTimeline_AimCamera->IsPlaying()) pTimeline_AimCamera->TickTimeline(DeltaTime);
}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Look"), this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ACharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSCharacter::StartJump);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ATPSCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction(TEXT("Equip"), IE_Pressed, this, &ATPSCharacter::ToggleEquip);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &ATPSCharacter::Aim_Start);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &ATPSCharacter::Aim_End);
	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Released, this, &ATPSCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ATPSCharacter::Reload);


}

void ATPSCharacter::MoveForward(float Value) {
	axisMoveForward = Value;

	if (GetController() && Value != 0) {
		if (bCrouch && bAim) return;
		AddMovementInput(FQuat(FRotator(0, GetControlRotation().Yaw, 0)).GetForwardVector(), Value);
	}
}

void ATPSCharacter::MoveRight(float Value) {
	axisMoveRight = Value;

	if (GetController() && Value != 0) {
		if (bCrouch && bAim) return;
		AddMovementInput(FQuat(FRotator(0, GetControlRotation().Yaw, 0)).GetRightVector(), Value);
	}
}

void ATPSCharacter::StartJump() {
	if (bCrouch) return;

	bJump = true;
	Jump(); // ACharactor就有的函数

	// 设置timer，经过一段时间后把bJump设为false
	GetWorld()->GetTimerManager().SetTimer(timerHandle_Jump, this, &ATPSCharacter::timerFunction_Jump, 1.f, false, 0.1f);
}

void ATPSCharacter::timerFunction_Jump() {
	bJump = false;
	// 销毁定时器
	GetWorld()->GetTimerManager().ClearTimer(timerHandle_Jump);
}

void ATPSCharacter::ToggleCrouch() {
	// 如果此时角色正在下落，则直接返回
	if (GetCharacterMovement()->IsFalling()) return;

	if (bCrouch) {
		bCrouch = false;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		currentCrouchCameraLocation = pSpring->GetRelativeLocation();
		desiredCrouchCameraLocation = FVector(FVector(0, 0, 150.f));
		pTimeline_CrouchCamera->PlayFromStart();
	}
	else {
		bCrouch = true;
		GetCharacterMovement()->MaxWalkSpeed = 100.f;
		currentCrouchCameraLocation = pSpring->GetRelativeLocation();
		desiredCrouchCameraLocation = FVector(FVector(0, 0, 100.f));
		pTimeline_CrouchCamera->PlayFromStart();
	}
}

void ATPSCharacter::ToggleEquip() {
	
	if (bEquip) {
		bEquip = false;
		
		Cast<AGameMode_Main>(GetWorld()->GetAuthGameMode())->pHUD->UnEquip();

		pHolster->PlayAnimation(pUnEquipHolsterAnim, false);
		pGun->PlayAnimation(pUnEquipARAnim, false);
		GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(pUnEquipCharacterAnim, TEXT("Slot_Equip"));
		GetWorld()->GetTimerManager().SetTimer(timerHandle_UnEquip, this, &ATPSCharacter::timerFunction_UnEquip, 1.f, false, 0.8f);
	}
	else {
		bEquip = true;

		Cast<AGameMode_Main>(GetWorld()->GetAuthGameMode())->pHUD->Equip();

		pHolster->PlayAnimation(pEquipHolsterAnim, false);
		pGun->PlayAnimation(pEquipARAnim, false);
		GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(pEquipCharacterAnim, TEXT("Slot_Equip"));
		GetWorld()->GetTimerManager().SetTimer(timerHandle_Equip, this, &ATPSCharacter::timerFunction_Equip, 1.f, false, 0.5f);
	}

}

void ATPSCharacter::timerFunction_Equip() {
	pGun->PlayAnimation(pPoseARAnim,false);
	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
	pGun->AttachToComponent(GetMesh(), rules, TEXT("Socket_Gun"));
	GetWorld()->GetTimerManager().ClearTimer(timerHandle_Equip);
}

void ATPSCharacter::timerFunction_UnEquip() {
	pGun->PlayAnimation(pPoseARAnim, false);
	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
	pGun->AttachToComponent(GetMesh(), rules, TEXT("Socket_AR"));
	GetWorld()->GetTimerManager().ClearTimer(timerHandle_UnEquip);
}

void ATPSCharacter::Aim_Start() {
	if (bEquip == false) return;

	bAim = true;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	currentAimCameraLocation = pSpring->SocketOffset;
	desiredAimCameraLocation = FVector(150.f, 60.f, 65.f);
	currentFov = pCamera->FieldOfView;
	desiredFov = 65.f;
	pTimeline_AimCamera->PlayFromStart();

	if (GetVelocity().Size() > 200.f) {
		bAimTransition = false;
	}
	else {
		bAimTransition = true;
		GetWorld()->GetTimerManager().SetTimer(timerHandle_AimTransition, this, &ATPSCharacter::timerFunction_AimTransition, 1.f, false, 0.1f);
	}
}

void ATPSCharacter::timerFunction_AimTransition() {
	bAimTransition = false;
	GetWorld()->GetTimerManager().ClearTimer(timerHandle_AimTransition);
}

void ATPSCharacter::Aim_End() {
	bAim = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	currentAimCameraLocation = pSpring->SocketOffset;
	desiredAimCameraLocation = FVector(0, 0, 40.f);
	currentFov = pCamera->FieldOfView;
	desiredFov = 85.f;
	pTimeline_AimCamera->PlayFromStart();

	if (GetVelocity().Size() > 200.f) {
		bAimTransition = false;
	}
	else {
		bAimTransition = true;
		GetWorld()->GetTimerManager().SetTimer(timerHandle_AimTransition, this, &ATPSCharacter::timerFunction_AimTransition, 1.f, false, 0.1f);
	}
}

void ATPSCharacter::timelineCallback_Function_AimCamera(float Value) {
	FVector location = FMath::Lerp(currentAimCameraLocation, desiredAimCameraLocation, Value);
	pSpring->SocketOffset = location;
	float fov = FMath::Lerp(currentFov, desiredFov, Value);
	pCamera->SetFieldOfView(fov);
}

void ATPSCharacter::timelineCallback_Function_CrouchCamera(float Value) {
	FVector location = FMath::Lerp(currentCrouchCameraLocation, desiredCrouchCameraLocation, Value);
	pSpring->SetRelativeLocation(location);
}

void ATPSCharacter::Shoot() {
	int32 nCurrentAmmo = FCString::Atoi(*Cast<AGameMode_Main>(GetWorld()->GetAuthGameMode())->pHUD->currentAmmo.ToString());

	if (bAim && nCurrentAmmo > 0) {
		bShoot = true;
		nCurrentAmmo -= 1;
		Cast<AGameMode_Main>(GetWorld()->GetAuthGameMode())->pHUD->Shoot();
		GetWorld()->GetTimerManager().SetTimer(timerHandle_Shoot, this, &ATPSCharacter::timerFunction_Shoot, 1.f, false, 0.6f);

		// 开火时镜头抖动
		// GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(pCameraShake);

		// 弹壳粒子
		UGameplayStatics::SpawnEmitterAttached(pParticleShell, pGun, TEXT("Socket_Shell"), FVector(0, 0, 0), FRotator(0, 0, 0), EAttachLocation::SnapToTarget);

		FRotator path(Pitch, GetControlRotation().Yaw, GetControlRotation().Roll);
		FHitResult hitResult;
		GetWorld()->LineTraceSingleByChannel(hitResult, pGun->GetSocketLocation("Socket_Muzzle"), pGun->GetSocketLocation("Socket_Muzzle") + path.Vector().GetSafeNormal()*1000.f, ECollisionChannel::ECC_Visibility);

		UDecalComponent* pDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), pMaterialDecal, FVector(5.f, 5.f, 5.f), hitResult.Location);
		pDecal->SetFadeScreenSize(0.001);

		// 后坐力
		AddControllerPitchInput(-1.f);
	}
}

void ATPSCharacter::timerFunction_Shoot() {
	bShoot = false;
	GetWorld()->GetTimerManager().ClearTimer(timerHandle_Shoot);
}

void ATPSCharacter::Reload() {
	int32 nCurrentAmmo = FCString::Atoi(*Cast<AGameMode_Main>(GetWorld()->GetAuthGameMode())->pHUD->currentAmmo.ToString());
	if (nCurrentAmmo != 30) {
		GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(pReloadAnim, TEXT("Slot_Reload"));
		Cast<AGameMode_Main>(GetWorld()->GetAuthGameMode())->pHUD->Reload();
		pGun->PlayAnimation(pReloadARAnim, false);
	}
}

