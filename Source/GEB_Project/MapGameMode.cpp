#include "MapGameMode.h"
#include "SavePointGISubsystem.h"

#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"

AMapGameMode::AMapGameMode()
{
	// 기본값은 UPROPERTY 초기값 사용
}

void AMapGameMode::RestartPlayer(AController* NewPlayer)
{
	if (!IsValid(NewPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("[RestartPlayer] INVALID Controller -> Super"));
		Super::RestartPlayer(NewPlayer);
		return;
	}

	FTransform SaveXform;
	if (TryGetSaveSpawnTransform(SaveXform))
	{
		UE_LOG(LogTemp, Warning, TEXT("[RestartPlayer] Using SavePoint: Loc=%s, Rot=%s"),
			*SaveXform.GetLocation().ToString(),
			*SaveXform.GetRotation().Rotator().ToString());

		EnsureSaveStartAt(SaveXform);              // 안전하게 PlayerStart도 동기화
		RestartPlayerAtTransform(NewPlayer, SaveXform);
		return;
	}

	// 세이브 포인트가 없을 때는 부모 로직
	AActor* StartSpot = ChoosePlayerStart(NewPlayer);
	const FTransform StartXform = StartSpot ? StartSpot->GetActorTransform() : FTransform::Identity;

	UE_LOG(LogTemp, Warning, TEXT("[RestartPlayer] Using Default Start: Loc=%s, Rot=%s%s"),
		*StartXform.GetLocation().ToString(),
		*StartXform.GetRotation().Rotator().ToString(),
		StartSpot ? *FString::Printf(TEXT(", StartActor=%s"), *StartSpot->GetName()) : TEXT(", StartActor=None"));

	Super::RestartPlayer(NewPlayer);
}

AActor* AMapGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	FTransform SaveXform;
	if (TryGetSaveSpawnTransform(SaveXform))
	{
		EnsureSaveStartAt(SaveXform);
		UE_LOG(LogTemp, Warning, TEXT("[ChoosePlayerStart] Using SavePoint Start at %s"),
			*SaveXform.GetLocation().ToString());
		return CachedSaveStart;
	}

	AActor* Fallback = Super::ChoosePlayerStart_Implementation(Player);
	if (Fallback)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ChoosePlayerStart] Fallback Start: %s @ %s"),
			*Fallback->GetName(), *Fallback->GetActorLocation().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ChoosePlayerStart] Fallback Start: None"));
	}
	return Fallback;
}

APawn* AMapGameMode::SpawnDefaultPawnAtTransform(AController* NewPlayer, const FTransform& SpawnTransform)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (!PawnClass) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* NewPawn = World->SpawnActor<APawn>(PawnClass, SpawnTransform, Params);
	if (!NewPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SpawnDefaultPawnAtTransform] FAILED at %s — fallback to Super"),
			*SpawnTransform.GetLocation().ToString());
		return Super::SpawnDefaultPawnAtTransform(NewPlayer, SpawnTransform);
	}
	return NewPawn;
}

bool AMapGameMode::TryGetSaveSpawnTransform(FTransform& OutTransform) const
{
	UWorld* World = GetWorld();
	if (!World) { UE_LOG(LogTemp, Warning, TEXT("[SavePoint] FAIL: World==nullptr")); return false; }

	UGameInstance* GI = World->GetGameInstance();
	if (!GI) { UE_LOG(LogTemp, Warning, TEXT("[SavePoint] FAIL: GameInstance==nullptr")); return false; }

	const USavePointGISubsystem* SP = GI->GetSubsystem<USavePointGISubsystem>();
	if (!SP) { UE_LOG(LogTemp, Warning, TEXT("[SavePoint] FAIL: SavePointGISubsystem==nullptr (GI=%s)"), *GetNameSafe(GI)); return false; }

	if (!SP->HasSavePoint()) { UE_LOG(LogTemp, Warning, TEXT("[SavePoint] FAIL: HasSavePoint==false (GI=%s)"), *GetNameSafe(GI)); return false; }

	FTransform T = SP->GetSavePoint();
	UE_LOG(LogTemp, Warning, TEXT("[SavePoint] Found(GI): Loc=%s, Rot=%s"),
		*T.GetLocation().ToString(), *T.GetRotation().Rotator().ToString());

	if (bSnapToFloor)
	{
		T = ApplyFloorSnap(T);
		UE_LOG(LogTemp, Warning, TEXT("[SavePoint] After FloorSnap: Loc=%s, Rot=%s"),
			*T.GetLocation().ToString(), *T.GetRotation().Rotator().ToString());
	}

	OutTransform = T;
	return true;
}

FTransform AMapGameMode::ApplyFloorSnap(const FTransform& InTransform) const
{
	UWorld* World = GetWorld();
	if (!World) return InTransform;

	const FVector BaseLoc = InTransform.GetLocation();
	const FVector Start = BaseLoc + FVector(0.f, 0.f, FloorTraceDistance * 0.5f);
	const FVector End = BaseLoc - FVector(0.f, 0.f, FloorTraceDistance);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SavePointFloorSnap), false);
	const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	if (bHit)
	{
		FTransform Out = InTransform;
		const float HalfHeight = GetDefaultPawnCapsuleHalfHeight();
		FVector Loc = InTransform.GetLocation();
		Loc.Z = Hit.ImpactPoint.Z + FloorOffset + HalfHeight;
		Out.SetLocation(Loc);
		return Out;
	}
	return InTransform;
}

AActor* AMapGameMode::GetOrCreateSavePlayerStart()
{
	if (CachedSaveStart && !CachedSaveStart->IsPendingKill())
	{
		return CachedSaveStart;
	}

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	// 레벨의 PlayerStart를 건드리지 않고, 런타임 전용 Start를 새로 스폰
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.ObjectFlags |= RF_Transient; // 저장/리로드 대상 아님

	CachedSaveStart = World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), FTransform::Identity, Params);
	if (CachedSaveStart)
	{
		CachedSaveStart->SetActorHiddenInGame(true);
		CachedSaveStart->SetActorEnableCollision(false); // 충돌 비활성
	}
	return CachedSaveStart;
}

void AMapGameMode::EnsureSaveStartAt(const FTransform& SaveXform)
{
	AActor* PS = GetOrCreateSavePlayerStart();
	if (PS)
	{
		PS->SetActorTransform(SaveXform); // 모빌리티 경고 없음(런타임 스폰)
	}
}

float AMapGameMode::GetDefaultPawnCapsuleHalfHeight() const
{
	UClass* PawnClass = GetDefaultPawnClassForController(nullptr);
	if (!PawnClass) PawnClass = DefaultPawnClass;

	if (!PawnClass) return 0.f;

	const ACharacter* CharCDO = Cast<ACharacter>(PawnClass->GetDefaultObject());
	if (CharCDO && CharCDO->GetCapsuleComponent())
	{
		return CharCDO->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	return 0.f;
}
