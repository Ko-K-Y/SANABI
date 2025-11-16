// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HealthInterface.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Blueprint/UserWidget.h"
#include "HealthInterface.h"
#include "GEB_ProjectCharacter.generated.h"

// ---------- Forward Declarations ----------
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UWeaponComponent;        // ¹«±â
class UExperienceComponent;    // °æÇèÄ¡/·¹º§
class UHealthComponent;
class UWBP_StatusHUD;

class UWeaponComponent;        // ï¿½ï¿½ï¿½ï¿½
class UExperienceComponent;    // ï¿½ï¿½ï¿½ï¿½Ä¡/ï¿½ï¿½ï¿½ï¿½

// UI
class UUserWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class GEB_PROJECT_API AGEB_ProjectCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGEB_ProjectCharacter();

	// AActor / APawn overrides
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// -------- Input handlers (public: Å° ï¿½ï¿½ï¿½Îµï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿? --------
	void Cheat_AddExp50();   // I key

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION()
	void DebugHurt();

protected:
	// -------- Movement / Look --------
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// -------- Combat --------
	void Shoot(const FInputActionValue& Value);
	void Reload(const FInputActionValue& Value);
	
	// ---------- Components ----------
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom = nullptr;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera = nullptr;

	/** MappingContext & Input Actions (Enhanced Input) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction = nullptr;

	// ---------- Gameplay ----------
	/** ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®(ï¿½ï¿½Å¸ï¿½Ó¿ï¿½ FindComponentByClassï¿½ï¿½ Ä³ï¿½ï¿½) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponComponent* WeaponComp = nullptr;

	/** EXP/Level component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UExperienceComponent* Experience = nullptr;

	// ---------- UI ----------
	/** ï¿½×»ï¿½ ï¿½ï¿½ï¿½Ì´ï¿½ ï¿½ï¿½ï¿½ï¿½ HUD (ï¿½ï¿½ï¿½ï¿½/ï¿½ï¿½ï¿½ï¿½Ä¡ ï¿½ï¿½) */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> StatusWidgetClass;

	UPROPERTY(Transient)
	UUserWidget* StatusWidget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWBP_StatusHUD> StatusHUDClass;

	UPROPERTY()
	TObjectPtr<UWBP_StatusHUD> StatusHUD;
};
