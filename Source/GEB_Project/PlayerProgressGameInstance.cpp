#include "PlayerProgressGameInstance.h"
#include "ExperienceComponent.h"

void UPlayerProgressGameInstance::CaptureFrom(UExperienceComponent* XP)
{
    if (!XP) return;
    SavedLevel = XP->GetLevel();
    SavedCurExp = XP->GetCurExp();
}

void UPlayerProgressGameInstance::ApplyTo(UExperienceComponent* XP) const
{
    if (!XP) return;

    // 함수 이름 반드시 LoadState로!
    XP->LoadState(SavedLevel, SavedCurExp, /*InExpToLevel=*/0);
}
