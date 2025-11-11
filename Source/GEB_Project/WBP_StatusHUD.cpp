#include "WBP_StatusHUD.h"
#include "HealthComponent.h"
#include "Engine/Engine.h"


void UWBP_StatusHUD::SetHealth(UHealthComponent* InHealth)
{
    if (Health == InHealth) return;

    // 기존 바인딩 풀기
    if (Health)
    {
        Health->OnHealthChanged.RemoveAll(this);
        Health->OnDeath.RemoveAll(this);
    }

    Health = InHealth;

    if (Health)
    {
        // 델리게이트 바인딩
        Health->OnHealthChanged.AddDynamic(this, &UWBP_StatusHUD::HandleHealthChanged);
        Health->OnDeath.AddDynamic(this, &UWBP_StatusHUD::HandleDeath);

        // 초기 1회 갱신
        HandleHealthChanged(Health->GetCurrentHealth_Implementation(),
            Health->GetMaxHealth_Implementation());
    }
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("SetHealth called"));
    if (Health)
    {
        HandleHealthChanged(Health->GetCurrentHealth_Implementation(),
            Health->GetMaxHealth_Implementation());
    }
}

void UWBP_StatusHUD::HandleHealthChanged(int32 Current, int32 Max)
{
    UpdateHearts(Current, Max);   // BP의 UpdateHearts 호출
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
        FString::Printf(TEXT("HandleHealthChanged C=%d M=%d"), Current, Max));
    UpdateHearts(Current, Max);
}

void UWBP_StatusHUD::HandleDeath()
{
    // 필요하면 사망 UI 처리
}
