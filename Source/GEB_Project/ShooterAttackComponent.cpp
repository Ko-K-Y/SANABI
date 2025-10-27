// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAttackComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h" // 회전 계산을 위해 포함

void UShooterAttackComponent::PerformAttack_Implementation()
{
	if (isCooldown) { return; }

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner || !ProjectileClass) return;

	// 1. 공격 애니메이션 재생 (BaseComponent 로직 유지)
	UAttackComponent::PerformAttack_Implementation(); 
	
	// 2. 발사 위치 및 방향 계산
	
	// Mesh에서 발사 소켓 위치 찾기
	FVector StartLocation = Owner->GetMesh()->GetSocketLocation(MuzzleSocketName);

	// 플레이어가 있는 방향으로 회전시키기 위한 목표 (이 로직은 AI Controller에서 하는 것이 좋지만, 컴포넌트에서 임시 처리)
	// 하지만 깔끔한 분리를 위해 여기서는 **Enemy의 현재 전방**으로 발사하거나, 
	// AI Controller가 Enemy Pawn의 **회전을 미리 Target 방향으로 완료**했다고 가정합니다.
	
	// 임시: 현재 Enemy의 Rotation을 발사 방향으로 사용
	FRotator SpawnRotation = Owner->GetControlRotation(); 
    
	// 만약 Enemy가 플레이어를 바라보도록 회전시키는 로직이 AI 컨트롤러에 있다면, 이 회전은 플레이어를 향하게 됩니다.

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Owner;

	// 3. Projectile 생성
	UWorld* World = GetWorld();
	if (World)
	{
		AEnemyProjectile* Projectile = World->SpawnActor<AEnemyProjectile>(
			ProjectileClass,
			StartLocation,
			SpawnRotation,
			SpawnParams
		);
		
		// 생성된 Projectile에 속도 설정 등 추가 초기화 로직
		// if (Projectile) { Projectile->InitializeVelocity(...); }
	}
}
