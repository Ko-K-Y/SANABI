// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HealthInterface.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Blueprint/UserWidget.h"
#include "HealthComponent.h"
#include "GEB_ProjectCharacter.generated.h"

// ---------- Forward Declarations ----------
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UWeaponComponent;        // ����
class UExperienceComponent;    // ����ġ/����
class UHealthComponent;
class UWBP_StatusHUD;

class UWeaponComponent;        // ����
class UExperienceComponent;    // ����ġ/����

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
	virtual void Tick(float DeltaTime) override;

	// -------- Input handlers (public: Ű ���ε����� ���� ���? --------
	void Cheat_AddExp50();   // I key

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION()
	void DebugHurt();

	// 에디터에서 사망 몽타주를 넣을 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* DeathMontage;

	// 중복 사망 방지용 변수
	bool bIsDead = false;

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
	/** ���� ������Ʈ(��Ÿ�ӿ� FindComponentByClass�� ĳ��) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponComponent* WeaponComp = nullptr;

	/** EXP/Level component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UExperienceComponent* Experience = nullptr;

	// ---------- UI ----------
	/** �׻� ���̴� ���� HUD (����/����ġ ��) */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> StatusWidgetClass;

	UPROPERTY(Transient)
	UUserWidget* StatusWidget = nullptr;

	// 11.24 권신혁 추가
	// 블루프린트의 변수를 가져오는 함수
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool GetValueFromBP();

	// 1. 에디터에서 피격 몽타주를 넣을 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* HitReactMontage;

	// 2. 체력 컴포넌트가 신호를 보내면 실행될 함수
	UFUNCTION(BlueprintCallable, Category = "OnHit")
	void OnHit();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWBP_StatusHUD> StatusHUDClass;

	UPROPERTY()
	TObjectPtr<UWBP_StatusHUD> StatusHUD;

	// 사망 신호를 받으면 실행될 함수
	UFUNCTION()
	void OnDeath();

	// 12.05 권신혁 추가. 에임 보정 기능 추가
	// 에임 보정 반경 (화면 중앙 기준)
	UPROPERTY(EditAnywhere, Category = "AimAssist")
	float AssistRadius = 100.0f;

	// 에임 보정 거리
	UPROPERTY(EditAnywhere, Category = "AimAssist")
	float AssistRange = 2000.0f;

	// 가장 적합한 타겟을 찾는 함수
	AActor* FindBestTarget(float Radius, float Range);

	// 에임 감속 계수 (1.0 = 정배율, 0.5 = 절반 속도, 0.1 = 매우 느림)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimAssist")
	float AimDeceleration = 0.5f;

};