// Copyright Epic Games, Inc. All Rights Reserved.

#include "GEB_ProjectCharacter.h"

// Engine / Components
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameInstance.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h" // EKeys

// UI (UMG)
#include "Blueprint/UserWidget.h"

// Gameplay/Project
#include "WeaponComponent.h"
#include "ExperienceComponent.h"
#include "PlayerProgressGameInstance.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGEB_ProjectCharacter

AGEB_ProjectCharacter::AGEB_ProjectCharacter()
{
	// �ݸ��� ĸ��
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// ��Ʈ�ѷ� ȸ���� ���� �������� �ʵ���
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// �̵� ������Ʈ �⺻��(���ø� �� ����)
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// ī�޶� ��
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// �ȷο� ī�޶�
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// ����ġ ������Ʈ
	Experience = CreateDefaultSubobject<UExperienceComponent>(TEXT("Experience"));
}

void AGEB_ProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ���� ������Ʈ ĳ��
	WeaponComp = FindComponentByClass<UWeaponComponent>();
	if (!WeaponComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponComp is null!"));
	}

	// ��Ʈ�ѷ�/�����÷��̾�
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	
	if (ULocalPlayer* LP = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, /*Priority*/0);
			}
		}
	}

	// �׻� ǥ�õǴ� HUD
	if (!StatusWidget && StatusWidgetClass)
	{
		StatusWidget = CreateWidget<UUserWidget>(PC, StatusWidgetClass);
		if (StatusWidget)
		{
			StatusWidget->AddToViewport(/*ZOrder=*/0);
			StatusWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}



	// ���� �Է� ���: ���� ����(Ŀ�� ����)
	{
		FInputModeGameOnly Mode;
		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = false;
	}

	// GameInstance�κ��� ����ġ ���� ����
	if (Experience)
	{
		if (UGameInstance* BaseGI = GetGameInstance())
		{
			if (UPlayerProgressGameInstance* GI = Cast<UPlayerProgressGameInstance>(BaseGI))
			{
				GI->ApplyTo(Experience);
			}
		}
	}

#if !UE_BUILD_SHIPPING
	// ���۽� ����� ǥ��
	if (Experience && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 2.5f, FColor::Yellow,
			FString::Printf(TEXT("Start Lv.%d  %d / %d"),
				Experience->GetLevel(), Experience->GetCurExp(), Experience->GetExpToLv()));
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGEB_ProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// MappingContext�� BeginPlay���� �߰�

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Input �׼� ���ε�(�� ���� ����)
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jump
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}

		// Move
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGEB_ProjectCharacter::Move);
		}

		// Look
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGEB_ProjectCharacter::Look);
		}

		// Shoot / Reload (����)
		if (ShootAction)
		{
			EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AGEB_ProjectCharacter::Shoot);
		}
		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AGEB_ProjectCharacter::Reload);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
			TEXT("'%s' Failed to find an Enhanced Input component!"),
			*GetNameSafe(this));
	}

	// �׽�Ʈ Ű ���ε�(I/Z) 
	PlayerInputComponent->BindKey(EKeys::I, IE_Pressed, this, &AGEB_ProjectCharacter::Cheat_AddExp50);
}

void AGEB_ProjectCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (!Controller) return;

	const FRotator Rot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, Rot.Yaw, 0.f);

	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Axis.Y);
	AddMovementInput(Right, Axis.X);
}

void AGEB_ProjectCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(Axis.X);
		AddControllerPitchInput(Axis.Y);
	}
}


void AGEB_ProjectCharacter::Cheat_AddExp50()
{
	if (Experience)
	{
		Experience->AddEXP(50);
		UE_LOG(LogTemplateCharacter, Log, TEXT("Cheat_AddExp50"));
	}
}



void AGEB_ProjectCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ���� �� ���൵ ����
	if (Experience)
	{
		if (UGameInstance* BaseGI = GetGameInstance())
		{
			if (UPlayerProgressGameInstance* GI = Cast<UPlayerProgressGameInstance>(BaseGI))
			{
				GI->CaptureFrom(Experience);
			}
		}
	}
	Super::EndPlay(EndPlayReason);
}



void AGEB_ProjectCharacter::Shoot(const FInputActionValue& /*Value*/)
{
	UE_LOG(LogTemp, Warning, TEXT("Shoot!"));
	if (WeaponComp) { WeaponComp->Fire(); }
}

void AGEB_ProjectCharacter::Reload(const FInputActionValue& /*Value*/)
{
	UE_LOG(LogTemp, Warning, TEXT("Reload!"));
	if (WeaponComp) { WeaponComp->Reload(); }
}