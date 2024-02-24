// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatSwitchComponent.h"
#include "BaseStatsComponent.h"
#include "EntityInterface.generated.h"


UCLASS(Abstract)
class PROJECTREBIRTH_API AEntityInterface : public ACharacter
{
	GENERATED_BODY()
#pragma region COMPONENTS
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		class UPlayerMovementComponent* EntityMovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		UCombatSwitchComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Entity Statisctics", meta = (AllowPrivateAccess = "true"))
		class UBaseStatsComponent* StatsComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		class UEntityAnimationBase* AnimationBase;
public:
	UFUNCTION(BlueprintCallable)
		UCombatSwitchComponent* GetCombatSwitchComponenet() const { return CombatComponent; }
	UFUNCTION(BlueprintCallable)
		class UEntityAnimationBase* GetEntityAnimationBase() const { return AnimationBase; }
	UFUNCTION(BlueprintCallable)
		class UPlayerMovementComponent* GetPlayerMovementComponent() const { return EntityMovementComponent; }
	UFUNCTION(BlueprintCallable)
		class UBaseStatsComponent* GetBaseStatsComponent() const { return StatsComponent; }
#pragma endregion
#pragma region Combat
protected:
	TSet<AEntityInterface*> LockedOnMe;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float LockLenght = 350.0f;
	//this variable is to choose witch object will colide with cameralockondetection
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		TArray<TEnumAsByte<EObjectTypeQuery>> LockOnObjectTypes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		bool bCanLockOnMe;
	bool bIsLockedOnTarget;
	AEntityInterface* LockedOnTarget;

	FTimerHandle AfterDeathTimerHandle;
	float TimeToSweepBody;

	void UnlockLockedOnMe();
	void SearchForTarget();
	bool TryLockOnTarget(AEntityInterface* targetedEntity);
	void AddEntityToLocked(AEntityInterface* targetedEntity);
public:

	//this method is to realease from  camera lock on IF LockedOnTarget matches NewTarget
	bool ReleaseLockOnTarget(AEntityInterface* NewTarget);
	//this method is to realease from ANY camera lock on
	void ReleaseLockOnTargetAnyway();
	void RemoveEntityFromLockedOnList(AEntityInterface* ToRemoveEnt);
	void LockOnThisEnetity(AEntityInterface* NewTarget);
	/*
	* Hit direction is relative to world
	*/
	virtual void OnCollisionHostality(const FHitEntityDamage& HitDamage, const FHitEntityEffect& HitEffects,
	float KnockbackStrenght, FVector HitDirection);
	virtual void OnCollisionHostalityDamage(const FHitEntityDamage& HitDamage);
	virtual void OnCollisionHostalityKnockback( float KnockbackStrenght, FVector HitDirection);
	virtual void OnDeath();
	//method to override to corpse disposal. 
protected:
	virtual void AfterDeath();
#pragma endregion


protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Properties")
		float jumpHeight;
	// Sets default values for this character's properties
	AEntityInterface(const FObjectInitializer& ObjectInitalizer);
	AEntityInterface();
	virtual void RestartLevel();


	UFUNCTION()
		void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex,
			bool bfromSweep, const FHitResult& SweepResoult);

#pragma region Getters&Setters&InputHandlers
protected:
	bool bWasJumpPressed;
public:
	//for collisions, creates Querry for ignoring this character and childrens
	virtual FCollisionQueryParams GetIgnoreCharacterParams() const;
	UFUNCTION(BlueprintCallable)
		bool getJump();


#pragma endregion
};
