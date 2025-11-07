// UIBootSubsystem.cpp
#include "UIBootSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ExperienceComponent.h"
#include "GEB_ProjectCharacter.h"
#include "Engine/Engine.h"

void UUIBootSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    UE_LOG(LogTemp, Warning, TEXT("[UIBootSubsystem] Initialize"));


    StatusHUDClass = LoadClass<UUserWidget>(
        nullptr,
        TEXT("WidgetBlueprint'/Game/UI/WBP_StatusHUD.WBP_StatusHUD_C'")
    );

    if (StatusHUDClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UIBootSubsystem] Loaded StatusHUD class OK."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[UIBootSubsystem] FAIL to load StatusHUD class. Check asset path!"));
    }

    FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UUIBootSubsystem::OnPostWorldInit);
}

void UUIBootSubsystem::Deinitialize()
{
    FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
    SpawnedHUD = nullptr;
    bTriedOnce = false;
}

void UUIBootSubsystem::OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS)
{
    if (!World)
        return;

    UE_LOG(LogTemp, Warning, TEXT("[UIBootSubsystem] OnPostWorldInit: WorldType=%d"), (int32)World->WorldType);

    // PIE/게임만
    if (World->WorldType != EWorldType::PIE && World->WorldType != EWorldType::Game)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UIBootSubsystem] Skip (not PIE/Game)"));
        return;
    }

    // 한 프레임 뒤에 시도(PC 생성 타이밍 보정)
    World->GetTimerManager().SetTimerForNextTick([this, World]()
        {
            TrySpawnHUD(World);
        });
}

void UUIBootSubsystem::TrySpawnHUD(UWorld* World)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC || !StatusHUDClass) return;

    UUserWidget* W = CreateWidget<UUserWidget>(PC, StatusHUDClass);
    if (!W) return;

    // 캐릭터에서 XP 가져오기
    APawn* Pawn = PC->GetPawn();
    if (Pawn)
    {
        if (UExperienceComponent* XP = Pawn->FindComponentByClass<UExperienceComponent>())
        {
            // 블루프린트 변수 'XP'에 직접 대입
            static const FName NAME_XP(TEXT("XP"));
            if (FProperty* P = W->GetClass()->FindPropertyByName(NAME_XP))
            {
                void* Dest = P->ContainerPtrToValuePtr<void>(W);
                UExperienceComponent** Ptr = reinterpret_cast<UExperienceComponent**>(Dest);
                *Ptr = XP;
            }
        }
    }

    W->AddToViewport(0);
    W->SetVisibility(ESlateVisibility::Visible);
}
