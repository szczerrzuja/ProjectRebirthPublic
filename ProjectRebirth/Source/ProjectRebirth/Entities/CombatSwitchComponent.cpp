// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSwitchComponent.h"
#include "Player\Rukki.h"
#include "Player\PlayerMovementComponent.h"
#include "EntityAnimationBase.h"

/*
* TODO: make charged attack mechanics
* 
*/

UCombatSwitchComponent::UCombatSwitchComponent()
{



}

UCombatSwitchComponent::UCombatSwitchComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AllowReregistration();
	PrimaryComponentTick.TickGroup = TG_StartPhysics;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bAllowReregistration = true;

	bHasInputChanged = false;
	currentAnimation = 1;
	bIsAnimationReady = false;
	bIsMovementComponentReady = false;


	currentComboState = 0;


}

void UCombatSwitchComponent::InitializeComponent()
{
	Super::InitializeComponent();

}
void UCombatSwitchComponent::BeginPlay()
{
	Super::BeginPlay();
	this->CombatFlags = (uint32)0;
	EntityOnwer = Cast<AEntityInterface>(GetOwner());
	
}

void UCombatSwitchComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* T)
{
	Super::TickComponent(DeltaTime, TickType, T);
	
	//Animbase is needed to use this component
	if (EntityOnwer->GetEntityAnimationBase())
	{

		const float remainingTimeofanimation = EntityOnwer->GetEntityAnimationBase()->GetRemainingAnimationTime();
		//bIsInCombat = remainingTimeofanimation >= 0.02f || IsInCombatMode();
		bTrasistedInCurrentFrame = false;
		bIsAnimationReady = EntityOnwer->GetEntityAnimationBase()->GetEndOfCombatAAnimation();
		//change state of combo
		if ((remainingTimeofanimation <= 0.02f || bIsAnimationReady) && !bTrasistedInCurrentFrame && bHasInputChanged)
		{
			switchAnimation();

		}

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("%i"), EntityOnwer->GetEntityAnimationBase() == nullptr));
	}

	
}


/*
* This method is to change desired flags to certain values
*/
void  UCombatSwitchComponent::SetCombatFlags(uint32 combFlags, uint32 switchingMask)
{
	CombatFlags = (CombatFlags & (~switchingMask)) | combFlags;
	bHasInputChanged = true;
}

bool UCombatSwitchComponent::IsInCombatMode() const
{
	return EntityOnwer->GetPlayerMovementComponent()->IsCustomMovementMode(CMOVE_Combat);
}
//this method affects movementComponent and might owner of this component
void UCombatSwitchComponent::SetIsInCombat(bool state)
{
	bIsInCombat = state;

}

bool UCombatSwitchComponent::GetIsInCombat() const
{

	return bIsInCombat;
}



bool UCombatSwitchComponent::switchAnimation()
{

	for (int i = 0; i < NonComboMoves.Num(); i++)
	{
		if (((uint32)NonComboMoves[i].inputCode & CombatFlags) == (uint32)NonComboMoves[i].inputCode)
		{

			currentAnimation = NonComboMoves[i].aimationCode;

			bIsInCombo = false;
			bTrasistedInCurrentFrame = true;
			bIsAnimationReady = false;
			bIsInCombat = true;
			bHasInputChanged = false;
			currentComboState = 0;
			return true;
		}

	}

	for (int i = 0; i < Combos[currentComboState].ComboArray.Num(); i++)
	{
		if (((uint32)Combos[currentComboState].ComboArray[i].inputCode & CombatFlags) == (uint32)Combos[currentComboState].ComboArray[i].inputCode)
		{
			
			currentAnimation = Combos[currentComboState].ComboArray[i].aimationCode;
			currentComboState = Combos[currentComboState].ComboArray[i].nextAttackArrayCode;

			bIsInCombo = true;
			bTrasistedInCurrentFrame = true;
			bIsAnimationReady = false;
			bIsInCombat = true;
			bHasInputChanged = false;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("%i :: %i"), (int)currentAnimation, (int)currentComboState));
			return true;
		}

	}
	return false;
	
	
}

