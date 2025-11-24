// ExperienceComponent.cpp
#include "ExperienceComponent.h"
#include "Engine/Engine.h"

UExperienceComponent::UExperienceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ExpToLevel = CalcExpToNext(Level);
}

void UExperienceComponent::BeginPlay()
{
	Super::BeginPlay();
	ExpToLevel = CalcExpToNext(Level);
	// 시작 시 한번 브로드캐스트해서 HUD가 즉시 갱신되게 할 수도 있음
	OnExpChanged.Broadcast(CurExp, ExpToLevel);
}

void UExperienceComponent::AddEXP(int32 Amount)
{
	if (Amount <= 0) return;

	CurExp += Amount;
	bool bLeveled = false;

	// 여러 레벨 연속 상승도 처리
	while (CurExp >= ExpToLevel)
	{
		CurExp -= ExpToLevel;
		Level++;
		ExpToLevel = CalcExpToNext(Level);
		bLeveled = true;

		OnLevelUp.Broadcast(Level);
	}

	OnExpChanged.Broadcast(CurExp, ExpToLevel);

#if !UE_BUILD_SHIPPING
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 2.5f, FColor::Yellow,
			FString::Printf(TEXT("[EXP] Lv.%d  %d / %d%s"),
				Level, CurExp, ExpToLevel, bLeveled ? TEXT("  (LEVEL UP!)") : TEXT("")));
	}
#endif
}

void UExperienceComponent::LoadState(int32 InLevel, int32 InCurExp, int32 InExpToLevel)
{
	Level = InLevel;
	CurExp = InCurExp;
	ExpToLevel = (InExpToLevel > 0) ? InExpToLevel : CalcExpToNext(Level);

	// UI 즉시 갱신 이벤트
	OnExpChanged.Broadcast(CurExp, ExpToLevel);
}
