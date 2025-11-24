// ExperienceComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExperienceComponent.generated.h"

// 블루프린트에서도 바인딩 가능한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExpChanged, int32, CurExp, int32, ExpToLevel);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GEB_PROJECT_API UExperienceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UExperienceComponent();

	// 경험치 추가
	UFUNCTION(BlueprintCallable)
	void AddEXP(int32 Amount);

	// 현재 값 조회 (UI용)
	UFUNCTION(BlueprintPure) int32 GetLevel()   const { return Level; }
	UFUNCTION(BlueprintPure) int32 GetCurExp()  const { return CurExp; }
	UFUNCTION(BlueprintPure) int32 GetExpToLv() const { return ExpToLevel; }

	// 이벤트
	UPROPERTY(BlueprintAssignable) FOnLevelUp    OnLevelUp;
	UPROPERTY(BlueprintAssignable) FOnExpChanged OnExpChanged;

	UFUNCTION(BlueprintCallable)
	void LoadState(int32 InLevel, int32 InCurExp, int32 InExpToLevel);


protected:
	virtual void BeginPlay() override;

private:
	// 내부 계산
	void TryLevelUp();
	int32 CalcExpToNext(int32 Lv) const { return 100 + Lv * 50; } // 필요시 조절

private:
	// 저장/에디터 노출
	UPROPERTY(EditAnywhere, Category = "EXP")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, Category = "EXP")
	int32 CurExp = 0;

	UPROPERTY(EditAnywhere, Category = "EXP")
	int32 ExpToLevel = 100;
};
