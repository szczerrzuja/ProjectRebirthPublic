// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityInterface.h"
#include "Player/PlayerMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EntityAnimationBase.h"
#include "Kismet/KismetSystemLibrary.h"


void AEntityInterface::SearchForTarget()
{
	//actors to ignore (this)
	TArray<AActor*> ignoreActors;
	ignoreActors.Init(this, 1);
	//output Array
	TArray<AActor*> outActors;
	//class type to cetect collisions
	UClass* seekClass = AEntityInterface::StaticClass();
	//TODO: METHOD THAT COULD BE OVERRITEN for different types of ovelaps
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), this->GetActorLocation(), LockLenght,
		LockOnObjectTypes, seekClass, ignoreActors, outActors);

	//TODO: Check if target in in sight (isnt behind obstacle)
	if (outActors.Num() > 0)
	{
		int minimumDistanceIndex = -1;
		float distanceToThis = 0.0f;
		float helperDistance = 0.0f;
		bool helperBoolean = false;
		//find closest actor to this
		for (int i = 0; i < outActors.Num(); i++)
		{
			if (!(Cast<AEntityInterface>(outActors[i])->bCanLockOnMe))
				continue;
			helperDistance = FVector::DistSquared(this->GetActorLocation(), outActors[i]->GetActorLocation());
			helperBoolean = (helperDistance < distanceToThis || minimumDistanceIndex == -1);
			distanceToThis = helperDistance * helperBoolean +
				distanceToThis * !helperBoolean;

			minimumDistanceIndex = i * helperBoolean +
				minimumDistanceIndex * !helperBoolean;
		}
		if(minimumDistanceIndex>-1)
			TryLockOnTarget(Cast<AEntityInterface>(outActors[minimumDistanceIndex]));
	}


}
//TODO: AEntityInterface is targetable ect. 
bool AEntityInterface::TryLockOnTarget(AEntityInterface* targetedEntity)
{
	if (targetedEntity)
	{
		bIsLockedOnTarget = true;
		LockedOnTarget = targetedEntity;
		targetedEntity->AddEntityToLocked(this);
		return true;
	}
	return false;
}

void AEntityInterface::AddEntityToLocked(AEntityInterface* targetedEntity)
{
	LockedOnMe.Emplace(targetedEntity);
}

void AEntityInterface::LockOnThisEnetity(AEntityInterface* NewTarget)
{
	if (bIsLockedOnTarget && LockedOnTarget)
		return;
	ReleaseLockOnTargetAnyway();
	bIsLockedOnTarget = true;
	LockedOnTarget = NewTarget;
	LockedOnTarget->AddEntityToLocked(this);

}
void AEntityInterface::OnCollisionHostality(const FHitEntityDamage& HitDamage, const FHitEntityEffect& HitEffects,
	float KnockbackStrenght, FVector HitDirection)
{

	StatsComponent->DoDamage(HitDamage);
	const float knockbackApplied = KnockbackStrenght - StatsComponent->GetEffectRes(EFFECT_Knockback).value;
	LaunchCharacter(HitDirection * knockbackApplied * (knockbackApplied > 0.0f), true, true);


}
void AEntityInterface::OnCollisionHostalityDamage(const FHitEntityDamage& HitDamage)
{
	StatsComponent->DoDamage(HitDamage);
}

void AEntityInterface::OnCollisionHostalityKnockback(float KnockbackStrenght, FVector HitDirection)
{
	const float knockbackApplied = KnockbackStrenght - StatsComponent->GetEffectRes(EFFECT_Knockback).value;
	LaunchCharacter(HitDirection * knockbackApplied * (knockbackApplied > 0.0f), true, true);
}

void AEntityInterface::OnDeath()
{
	
	bCanLockOnMe = false; 
	UnlockLockedOnMe();
	LockedOnMe.Empty();
	LockedOnMe.Shrink();
	GetWorld()->GetTimerManager().SetTimer(AfterDeathTimerHandle, this, &AEntityInterface::AfterDeath, TimeToSweepBody, false);
}

void AEntityInterface::AfterDeath()
{
	Destroy();
}

bool AEntityInterface::ReleaseLockOnTarget(AEntityInterface* NewTarget)
{

	if (LockedOnTarget == NewTarget)
	{
		LockedOnTarget->RemoveEntityFromLockedOnList(this);
		bIsLockedOnTarget = false;
		LockedOnTarget = NULL;
		return true;
	}
	return false;
}

void AEntityInterface::ReleaseLockOnTargetAnyway()
{
	LockedOnTarget->RemoveEntityFromLockedOnList(this);
	bIsLockedOnTarget = false;
	LockedOnTarget = NULL;
}

void AEntityInterface::RemoveEntityFromLockedOnList(AEntityInterface* ToRemoveEnt)
{
	LockedOnMe.Remove(ToRemoveEnt);
}

void AEntityInterface::UnlockLockedOnMe()
{
	
	for (auto& Element : LockedOnMe)
	{
		Element->ReleaseLockOnTarget(this);
	}
	LockedOnMe.Empty();
}

// Sets default values
AEntityInterface::AEntityInterface()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LockedOnTarget = NULL;
	bCanLockOnMe = true;
	TimeToSweepBody = 3.0f;
}
AEntityInterface::AEntityInterface(const FObjectInitializer& ObjectInitalizer)
{
	PrimaryActorTick.bCanEverTick = true;
	LockedOnTarget = NULL;
	bCanLockOnMe = true;
	TimeToSweepBody = 3.0f;
}


void AEntityInterface::RestartLevel()
{
}


void AEntityInterface::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bfromSweep, const FHitResult& SweepResoult)
{

}
FCollisionQueryParams AEntityInterface::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}
bool AEntityInterface::getJump() { return bWasJumpPressed; }