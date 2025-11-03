#pragma once

#include "Engine/GameInstance.h"
#include "SpawnPointGameInstance.generated.h"

UCLASS()
class GEB_PROJECT_API USpawnPointGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    USpawnPointGameInstance();

    // 레벨 리로드 사이에서도 그대로 유지됨(앱 재시작은 고려 X)
    virtual void Init() override;
    virtual void Shutdown() override;

    // 체크포인트(스폰 위치) 기록
    UFUNCTION(BlueprintCallable, Category = "SpawnPoint")
    void SetSpawnPoint(const FTransform& InTransform);

    // 체크포인트(스폰 위치) 조회
    UFUNCTION(BlueprintPure, Category = "SpawnPoint")
    const FTransform& GetSpawnPoint() const { return LastSpawnTransform; }

    // 초기화(원점 Identity로 리셋)
    UFUNCTION(BlueprintCallable, Category = "SpawnPoint")
    void ClearSpawnPoint();

    // 어디서든 GI에 접근하기 위한 헬퍼(블루프린트에서도 사용 가능)
    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "SpawnPoint")
    static USpawnPointGameInstance* Get(const UObject* WorldContextObject);

    // 세팅 여부(필요 시 사용)
    UFUNCTION(BlueprintPure, Category = "SpawnPoint")
    bool HasSpawnPoint() const { return bHasSpawnPoint; }

private:
    // 마지막 저장된 스폰 Transform
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnPoint", meta = (AllowPrivateAccess = "true"))
    FTransform LastSpawnTransform;

    // SetSpawnPoint가 호출된 적 있는지
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnPoint", meta = (AllowPrivateAccess = "true"))
    bool bHasSpawnPoint;
};
