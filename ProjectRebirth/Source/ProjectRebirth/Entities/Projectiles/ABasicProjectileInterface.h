// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../BaseStatsComponent.h"
#include "ABasicProjectileInterface.generated.h"

/*
* This class is not for projectiles per se. Its for All projectiles/rays type things
*/
UCLASS(Abstract, Blueprintable)
class PROJECTREBIRTH_API ABasicProjectileInterface : public AActor
{
	GENERATED_BODY()

public:	
	// Called every frame
	virtual void InitializeProjectile(const FVector& InitVelocity, const FHitEntityDamage& Damage,
		const FHitEntityEffect& Effect, USceneComponent* HomingTarget);
	virtual void InitializeProjectile(const FVector& InitVelocity, const FHitEntityDamage& Damage,
		USceneComponent* HomingTarget);

	virtual void OnCollisionWithProjectile(UPrimitiveComponent* OverlappedComponent, 
		AActor* otherActor, UPrimitiveComponent* otherComponent,
		int32 otherBodyIndex, bool bfromSweep, const FHitResult& Hit);
};
