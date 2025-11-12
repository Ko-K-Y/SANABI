#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpawnPointBlueprintLibrary.generated.h"

class USpawnPointSubsystem;
class AActor;

UCLASS()
class GEB_PROJECT_API USpawnPointBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Subsystem 핸들을 얻습니다. 필요하면 이걸로 직접 Get/Set 호출 가능 */
    UFUNCTION(BlueprintPure, Category = "SpawnPoint", meta = (WorldContext = "WorldContextObject"))
    static USpawnPointSubsystem* GetSubsystem(const UObject* WorldContextObject);

    /** 저장된 스폰 포인트를 Transform으로 받습니다. 반환값: 존재 여부 */
    UFUNCTION(BlueprintCallable, Category = "SpawnPoint", meta = (WorldContext = "WorldContextObject"))
    static bool GetSpawnPointTransform(const UObject* WorldContextObject, FTransform& OutTransform);

    /** 스폰 포인트를 Transform으로 설정 */
    UFUNCTION(BlueprintCallable, Category = "SpawnPoint", meta = (WorldContext = "WorldContextObject"))
    static void SetSpawnPointTransform(const UObject* WorldContextObject, const FTransform& InTransform);

    /** 특정 액터(예: 체크포인트 화살표/플레이어)의 현재 Transform을 스폰 포인트로 설정 */
    UFUNCTION(BlueprintCallable, Category = "SpawnPoint", meta = (WorldContext = "WorldContextObject"))
    static void SetSpawnPointFromActor(const UObject* WorldContextObject, const AActor* Actor);

    /** 저장된 스폰 포인트로 지정한 플레이어를 즉시 이동(텔레포트) */
    UFUNCTION(BlueprintCallable, Category = "SpawnPoint", meta = (WorldContext = "WorldContextObject"))
    static bool TeleportPlayerToSpawnPoint(const UObject* WorldContextObject, int32 PlayerIndex = 0);

    /** 스폰 포인트 초기화(없음 상태로) */
    UFUNCTION(BlueprintCallable, Category = "SpawnPoint", meta = (WorldContext = "WorldContextObject"))
    static void ClearSpawnPoint(const UObject* WorldContextObject);
};
