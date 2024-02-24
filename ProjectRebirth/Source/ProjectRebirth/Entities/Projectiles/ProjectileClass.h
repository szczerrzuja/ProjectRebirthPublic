// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABasicProjectileInterface.h"
#include "ProjectileClass.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTREBIRTH_API AProjectileClass : public ABasicProjectileInterface
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AProjectileClass();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* ColiderSphere; 
	FHitEntityDamage HitDamage;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileProperties" )
		float targetingAccelerationMagnitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileProperties")
		float MaximumAccelerationMultiplayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileProperties")
		float MaxDistance;
	// Called every frame
	virtual void InitializeProjectile(const FVector& InitVelocity, const FHitEntityDamage& Damage,
		const FHitEntityEffect& Effect, USceneComponent* HomingTarget = NULL) override;
	virtual void InitializeProjectile(const FVector& InitVelocity, const FHitEntityDamage& Damage, USceneComponent* HomingTarget = NULL) override;

	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	virtual void OnCollisionWithProjectile(UPrimitiveComponent* OverlappedComponent,
		AActor* otherActor, UPrimitiveComponent* otherComponent,
		int32 otherBodyIndex, bool bfromSweep, const FHitResult& Hit);

private:
	float LinearAccelerationMultiplayer(float currentDistance);
};
