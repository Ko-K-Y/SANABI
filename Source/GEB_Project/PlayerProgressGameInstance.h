#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerProgressGameInstance.generated.h"

UCLASS()
class GEB_PROJECT_API UPlayerProgressGameInstance : public UGameInstance
{
    GENERATED_BODY()
public:
    // 저장해둘 값
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progress")
    int32 SavedLevel = 1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Progress")
    int32 SavedCurExp = 0;

    // 컴포넌트 -> GI로 복사
    UFUNCTION(BlueprintCallable, Category = "Progress")
    void CaptureFrom(class UExperienceComponent* XP);

    // GI -> 컴포넌트로 적용
    UFUNCTION(BlueprintCallable, Category = "Progress")
    void ApplyTo(class UExperienceComponent* XP) const;
};
