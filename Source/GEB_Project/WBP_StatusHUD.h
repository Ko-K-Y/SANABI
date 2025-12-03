#pragma once
#include "Blueprint/UserWidget.h"
#include "WBP_StatusHUD.generated.h"

class UHealthComponent;
class UExperienceComponent;

UCLASS()
class GEB_PROJECT_API UWBP_StatusHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    // 캐릭터에서 넘겨줄 때 호출
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetHealth(UHealthComponent* InHealth);

    // BP에 이미 만든 그 함수: C++에서 호출 -> BP 구현 실행
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateHearts(int32 Current, int32 Max);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetExperience(UExperienceComponent* InExp);    

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateExp(int32 CurExp, int32 ExpToLevel);     
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateLevel(int32 NewLevel);                    

protected:
    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    TObjectPtr<UHealthComponent> Health = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    TObjectPtr<UExperienceComponent> Exp = nullptr;

    UFUNCTION()
    void HandleHealthChanged(int32 Current, int32 Max);

    UFUNCTION()
    void HandleDeath();

    UFUNCTION()               
        void HandleExpChanged(int32 Cur, int32 Max);

    UFUNCTION()         
        void HandleLevelUp(int32 Lv);
};
