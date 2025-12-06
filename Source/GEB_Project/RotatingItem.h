#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingItem.generated.h"

UCLASS()
class GEB_PROJECT_API ARotatingItem : public AActor
{
	GENERATED_BODY()

public:
	ARotatingItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	// 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* TriggerComp;

	// 설정 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float RotationSpeed = 90.0f; // 초당 회전 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ShrinkSpeed = 5.0f;    // 줄어드는 속도

	// 내부 상태 변수
	bool bIsTriggered = false;

	// 블루프린트에서 로직을 구현할 수 있는 이벤트 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnHookActivated();

	// 충돌 감지 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};