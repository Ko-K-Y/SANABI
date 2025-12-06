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
#include "Kismet/KismetMathLibrary.h" // 내적 계산용
#include "DrawDebugHelpers.h"

// Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h" // EKeys

// UI (UMG)
#include "Blueprint/UserWidget.h"
#include "HealthComponent.h"     
#include "HealthInterface.h"
#include "WBP_StatusHUD.h"

// Gameplay/Project
#include "WeaponComponent.h"
#include "ExperienceComponent.h"
#include "PlayerProgressGameInstance.h"
#include "HealthComponent.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGEB_ProjectCharacter

AGEB_ProjectCharacter::AGEB_ProjectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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

	// 11.24 권신혁 추가. 이벤트 연결
	UHealthComponent* HealthComp = FindComponentByClass<UHealthComponent>();
	if (HealthComp)
	{
		// HealthComp의 OnDamaged가 울리면 -> 내 OnHit 함수를 실행해라
		HealthComp->OnDamaged.AddDynamic(this, &AGEB_ProjectCharacter::OnHit);
		// OnDeath 이벤트에 내 OnDeath 함수 바인딩
		HealthComp->OnDeath.AddDynamic(this, &AGEB_ProjectCharacter::OnDeath);
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

	if (StatusHUDClass)
	{
		StatusHUD = CreateWidget<UWBP_StatusHUD>(PC, StatusHUDClass);
		if (StatusHUD)
		{
			StatusHUD->AddToViewport(0);
			StatusHUD->SetVisibility(ESlateVisibility::Visible);

			// Health 컴포넌트 찾아서 위젯에 주입
			UHealthComponent* HC = nullptr;
			if (HealthComponent)        HC = HealthComponent;
			else                        HC = FindComponentByClass<UHealthComponent>();

			if (HC)
			{
				StatusHUD->SetHealth(HC);   // ← 여기서 바인딩 + 초기 하트 갱신 끝
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("HealthComponent not found on Character"));
			}
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

// [추가] Tick 함수 구현
void AGEB_ProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); // 부모 클래스의 Tick도 실행 (필수)

	// 1. 무기가 사격 중인지 확인
	bool bWeaponIsFiring = false;
	if (WeaponComp)
	{
		bWeaponIsFiring = WeaponComp->IsShooting();
	}

	// 2. 조준 중(줌)이거나 OR 사격 중이라면? -> 무조건 정면(카메라) 보기
	// (bIsAiming 변수는 블루프린트에서 제어하거나 C++에 있다면 사용)
	// 만약 bIsAiming이 블루프린트 전용 변수라면 GetValueFromBP() 처럼 가져와야 할 수도 있습니다.
	// 여기서는 C++ 변수라고 가정합니다. (없으면 선언 필요)
	bool bIsAimingState = false; // 일단 임시 변수 (C++에 bIsAiming 선언 필요)

	// *중요: bIsAiming이 C++에 없다면 헤더에 bool bIsAiming 추가하고 줌 로직에서 켜고 꺼야 함.

	if (bIsAimingState || bWeaponIsFiring)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	// 3. 둘 다 아니라면 -> 이동 방향 보기 (평소 상태)
	else
	{
		float CurrentSpeed = GetVelocity().Size2D();
		if (CurrentSpeed > 0.1f)
		{
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		else
		{
			// 멈춰있을 때 정면 볼지 말지는 선택 사항
			bUseControllerRotationYaw = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}

	// 크로스헤어 타겟 검사
	CheckCrosshairTarget();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGEB_ProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// MappingContext�� BeginPlay���� �߰�

	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Debug_Hurt", IE_Pressed, this, &AGEB_ProjectCharacter::DebugHurt);


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

// 12.05 권신혁 수정. 에임 감속 기능 추가
void AGEB_ProjectCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// 1. 에임 감속 계수 (기본 1.0)
	float Friction = 1.0f;

	// 2. 화면 중앙에 적이 있는지 검사 
	AActor* Target = FindBestTarget(AssistRadius, AssistRange);

	if (Target)
	{
		// 적이 있으면 에디터에서 설정한 감속 계수로 Friction 설정
		Friction = AimDeceleration;

		// (선택) 움직이는 적을 살짝 따라가게 하려면 여기서 AddControllerYawInput을 미세하게 넣어줄 수도 있음 (Camera Magnetism)
	}

	if (Controller != nullptr)
	{
		// 3. 입력값에 Friction 곱하기
		AddControllerYawInput(LookAxisVector.X * Friction);
		AddControllerPitchInput(LookAxisVector.Y * Friction);
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
	// 11.24 권신혁 추가. 그래플링 중이면 공격 불가
	bool InGrappling = GetValueFromBP();
	if (InGrappling) return;


	UE_LOG(LogTemp, Warning, TEXT("Shoot!"));
	if (WeaponComp) { WeaponComp->Fire(); }
}

void AGEB_ProjectCharacter::Reload(const FInputActionValue& /*Value*/)
{
	UE_LOG(LogTemp, Warning, TEXT("Reload!"));
	if (WeaponComp) { WeaponComp->Reload(); }
}

// 11.24 권신혁 추가. 피격 당하면 호출되는 함수
void AGEB_ProjectCharacter::OnHit()
{
	// 로그 확인
	UE_LOG(LogTemp, Warning, TEXT("Ouch! I'm hit!"));

	// 몽타주 재생
	if (HitReactMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 피격 애니메이션 재생
			AnimInstance->Montage_Play(HitReactMontage);
		}

	}
}
void AGEB_ProjectCharacter::DebugHurt()
{
	UHealthComponent* HC = HealthComponent ? HealthComponent.Get() : FindComponentByClass<UHealthComponent>();
	if (HC)
	{
		IHealthInterface::Execute_ApplyDamage(HC, 1.f);
	}
}

// 12.03 권신혁 추가. 사망 처리 함수
void AGEB_ProjectCharacter::OnDeath()
{
	// 이미 죽은 상태면 또 죽지 않음
	if (bIsDead) return;

	bIsDead = true;
	UE_LOG(LogTemp, Warning, TEXT("Player is Dead!"));

	// 1. 키보드/마우스 입력 차단
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	// 2. 사망 애니메이션 재생
	if (DeathMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 몽타주 재생
			AnimInstance->Montage_Play(DeathMontage);
		}
	}

	// 3. 충돌 끄기 (적이 시체를 밟고 지나가도록 / 선택사항)
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}

// 12.05 권신혁 추가. 가장 적합한 타겟 찾기 함수
AActor* AGEB_ProjectCharacter::FindBestTarget(float Radius, float Range)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return nullptr;

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot); // 카메라 위치/회전 가져오기

	FVector Start = CamLoc;
	FVector End = Start + (CamRot.Vector() * Range);

	// 1. 구체 트레이스 (Sphere Trace)로 범위 내 모든 적 감지
	TArray<FHitResult> OutHits;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 나 자신 무시

	// Enemy 채널(또는 Pawn)로 검사
	bool bHit = GetWorld()->SweepMultiByChannel(
		OutHits, Start, End, FQuat::Identity, ECollisionChannel::ECC_Pawn, Sphere, Params
	);

	AActor* BestTarget = nullptr;
	float BestDotProduct = -1.0f; // -1 ~ 1 (1에 가까울수록 정면)

	// 2. 감지된 적들 중 "화면 정중앙"에 가장 가까운 놈 찾기
	for (const FHitResult& Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		// 적이 맞는지 확인 (Tag나 Interface, Class 등으로 체크)
		if (HitActor && HitActor->ActorHasTag("Enemy"))
		{
			// 방향 벡터 계산 (카메라 -> 적)
			FVector DirToTarget = (HitActor->GetActorLocation() - Start).GetSafeNormal();

			// 내적(Dot Product): 카메라 방향과 적 방향이 얼마나 일치하는지 (-1:뒤, 0:옆, 1:정면)
			float Dot = FVector::DotProduct(CamRot.Vector(), DirToTarget);

			// 가장 정면에 가까운(내적값이 큰) 적을 선택
			if (Dot > BestDotProduct)
			{
				BestDotProduct = Dot;
				BestTarget = HitActor;
			}
		}

		// DrawDebugSphere(GetWorld(), End, Radius, 12, FColor::Green, false, 0.1f);
	}

	return BestTarget;
}

void AGEB_ProjectCharacter::CheckCrosshairTarget()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	FVector Start = CamLoc;
	FVector End = Start + (CamRot.Vector() * CrosshairCheckRange);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// 화면 중앙으로 레이저 발사
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, Start, End, ECollisionChannel::ECC_Visibility, Params
	);


	bool bCurrentHitEnemy = false;

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		// 적 태그가 있거나, HealthComponent를 가진 대상인지 확인
		if (HitActor && (HitActor->ActorHasTag("Enemy") || HitActor->FindComponentByClass<UHealthComponent>()))
		{
			bCurrentHitEnemy = true;
			// 적을 보고 있다면 화면에 메시지 출력
			// GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Red, TEXT("ENEMY TARGETED!"));
		}
	}

	// [중요] 상태가 변했을 때만 이벤트 호출 (성능 최적화)
	// (예: 허공을 보다가 적을 봤을 때 OR 적을 보다가 허공을 봤을 때)
	if (bCurrentHitEnemy != bIsTargetingEnemy)
	{
		bIsTargetingEnemy = bCurrentHitEnemy;

		// 블루프린트로 신호 보냄! (True면 적, False면 아님)
		OnCrosshairTargetChanged(bIsTargetingEnemy);
	}
}