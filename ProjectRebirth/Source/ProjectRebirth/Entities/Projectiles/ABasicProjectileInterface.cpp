// Fill out your copyright notice in the Description page of Project Settings.


#include "ABasicProjectileInterface.h"

void ABasicProjectileInterface::InitializeProjectile(const FVector& InitVelocity, const FHitEntityDamage& Damage,
	const FHitEntityEffect& Effect, USceneComponent* HomingTarget)
{
}

void ABasicProjectileInterface::InitializeProjectile(const FVector& InitVelocity, const FHitEntityDamage& Damage,
	USceneComponent* HomingTarget)
{
}

void ABasicProjectileInterface::OnCollisionWithProjectile(UPrimitiveComponent* OverlappedComponent,
	AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bfromSweep, const FHitResult& Hit)
{
}
