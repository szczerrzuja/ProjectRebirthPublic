// Fill out your copyright notice in the Description page of Project Settings.

#include "EntityAnimationBase.h"
#include "EntityInterface.h"
#include "Player\PlayerMovementComponent.h"
#include "CombatSwitchComponent.h"


UEntityAnimationBase::UEntityAnimationBase()
{
	ForwardVelocity = 0.0f;
	bIsJump = false;
	MovementAnimationState = 0;
	CombatAnimationID = 0;
	bEndOfCombatAnimation = false;
	
}



void UEntityAnimationBase::NativeInitializeAnimation() 
{
	Super::NativeInitializeAnimation();
	EntityReference = Cast<AEntityInterface>(TryGetPawnOwner());
	if (EntityReference)
	{
		MovementComponenetReference = EntityReference->GetPlayerMovementComponent();
	}
	
}

void UEntityAnimationBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	/*
	* In case of EntityRef dissapear, we want to take back that ref. Usefull in multiplayer
	*/
	if (EntityReference && MovementComponenetReference)
	{
		UpdateAnimProperties();
	}
	else
	{
		EntityReference = Cast<AEntityInterface>(TryGetPawnOwner());
		if(!EntityReference)
		{
			return;
		}
		MovementComponenetReference = EntityReference->GetPlayerMovementComponent();


		//TODO: Get all entity actors, cast them and search owner ?
	}
}

void UEntityAnimationBase::UpdateAnimProperties()
{
	ForwardVelocity = EntityReference->GetVelocity().Length();
	bIsJump = EntityReference->getJump();
	MovementAnimationState = MovementComponenetReference->GetCurrentMovementMode();
	CombatAnimationID = EntityReference->GetCombatSwitchComponenet()->GetCurrentAnimation();
	
}

void UEntityAnimationBase::ResetEndOFCombatAnim()
{

}


