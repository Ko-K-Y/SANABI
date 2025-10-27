// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterEnemy.h"


AShooterEnemy::AShooterEnemy()
{
	
}

void AShooterEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShooterEnemy::Attack()
{
	// ABaseEnemy에 AttackComponent를 포인터형 변수 AttackComp로 정의해놓았음.
	// AttackComp->FunctionName() 형식으로 사용할 것.
}
