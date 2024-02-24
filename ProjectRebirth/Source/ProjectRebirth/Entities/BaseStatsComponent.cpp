// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseStatsComponent.h"
#include "EntityInterface.h"
#include <array>

FHitEntityDamage::FHitEntityDamage()
{
	DamageValue = 0.0f;
	DamageType = DAMAGE_TYPE_NONE;
}

FHitEntityDamage::FHitEntityDamage(float damageVal, TEnumAsByte<UResitanceTypeStat> dmgType)
{
	DamageValue = damageVal;
	DamageType = dmgType;
}



FHitEntityEffect::FHitEntityEffect()
{
	Duration = 0.0f;
	Strenght = 0.0f;
	EffectType = EFFECT_NONE;
}

FHitEntityEffect::FHitEntityEffect(float duration, float strenght, TEnumAsByte<UEffectResistancesStat> effectType)
{
	Duration = duration;
	Strenght = strenght;
	EffectType = effectType;
}


FUStat::FUStat()
{
	value = 0.0f;
}

FUStat::FUStat(float val)
{
	this->value = val;
}

FUStatRegen::FUStatRegen() : FUStat()
{
	Max_value = 0.0f;
	Regen_Value = 0.0f;
	
	RegenType = REGENTYPE_NONE;
	RegenName = URegNames_NONE;
}

FUStatRegen::FUStatRegen(float reg_val, float currval, float maxVal, TEnumAsByte<URegenStatsNames> regName, TEnumAsByte<URegenType> regType) : FUStat(maxVal)
{

	Regen_Value = reg_val;
	Max_value = currval;
	RegenType = regType;
	RegenName = regName;
}

float const& FUStatRegen::operator[](std::size_t i)
{
	
	const std::array<float*, 3> fields = { &value, &Regen_Value, &Max_value };
	return *fields[i];
	
}


FUResStat::FUResStat() : FUStat()
{
	ResistanceName = DAMAGE_TYPE_NONE;
}

FUResStat::FUResStat(float val, TEnumAsByte<UResitanceTypeStat> resNam) : FUStat(val)
{
	ResistanceName = resNam;
}

FUEffectResistance::FUEffectResistance() : FUStat()
{
	
}

FUEffectResistance::FUEffectResistance(float val, TEnumAsByte<UEffectResistancesStat> effRes) : FUStat(val)
{
	FUEffectResist = effRes;
}

FUUtility::FUUtility() : FUStat()
{
	
}

FUUtility::FUUtility(float val, TEnumAsByte<UUtilityStats> Util) : FUStat(val)
{
	FUUtilityName = Util;
}


// Sets default values for this component's properties
UBaseStatsComponent::UBaseStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	AllowReregistration();
	PrimaryComponentTick.TickGroup = TG_StartPhysics;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bAllowReregistration = true;

	EntityOwner = Cast<AEntityInterface>(this->GetOwner());
	IsAlive = true;
	InitStats();


}
void UBaseStatsComponent::InitStats()
{

	//must init those to work properly
	for (int i = 0 + 1; i < URegNames_Custom; i++)
	{
		Regeneratives.Add(FUStatRegen(0.0f, 0.0f, 0.0f, (TEnumAsByte<URegenStatsNames>)i, REGENTYPE_BySecond));
	}

	for (int i = DAMAGE_TYPE_NONE + 1; i < DAMAGE_TYPE_MAX; i++)
	{
		Resistances.Add(FUResStat(0.0f, (TEnumAsByte<UResitanceTypeStat>)i));
	}
	for (int i = EFFECT_NONE + 1; i < EFFECT_Max; i++)
	{
		EffectRes.Add(FUEffectResistance(0.0f, (TEnumAsByte<UEffectResistancesStat>)i));
	}
	for (int i = UTILITY_NONE + 1; i < UTILITY_Max; i++)
	{
		Utilities.Add(FUUtility(0.0f, (TEnumAsByte<UUtilityStats>)i));
	}

}
void UBaseStatsComponent::RechargeOnHit()
{
	//TODO: damage ammount depending on damage dealt OR ammount of hits occured
}
//This function updates regeneratives. Other function is for updating effect on charater
void UBaseStatsComponent::RechargeOnSecond(float DeltaTime)
{

	//owner is dead, or died 
	if (Regeneratives[URegNames_HealthPoints - 1].value< -0.01f || !IsAlive)
	{
		if (IsAlive)
		{
			IsAlive = false;
			EntityOwner->OnDeath();
		}
		return;
	}
	//
	for (int i = 0; i < Regeneratives.Num(); i++)
	{
		//regen type somehow is 0
		if (Regeneratives[i].RegenType == REGENTYPE_BySecond)
		{
			Regeneratives[i].value = Regeneratives[i].value + Regeneratives[i].Regen_Value * DeltaTime;
			//clamp function is faster than function created by me 
			Regeneratives[i].value= std::clamp(Regeneratives[i].value, -1000.0f, Regeneratives[i].Max_value);
		}
	}
}



// Called when the game starts
void UBaseStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	

}

void UBaseStatsComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
}


// Called every frame
void UBaseStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RechargeOnSecond(DeltaTime);
}

void UBaseStatsComponent::HealFull()
{
	for (int i = 0; i < Regeneratives.Num() - 1; i++)
	{
		Regeneratives[i].value = Regeneratives[i].Max_value;
	}
}

void UBaseStatsComponent::OnRespawn()
{
	for (int i = 0; i < Regeneratives.Num() - 1; i++)
	{
		Regeneratives[i].value = Regeneratives[i].Max_value;
	}
	IsAlive = true;
}
//TODO
void UBaseStatsComponent::ApplyEffect(const FHitEntityEffect& Effect)
{

}
/*
* called when owner get damaged.Calculates damage recieved
* TODO: Regeneration, when damage is lower than resistance on special monsters. Maybe redo stats system OR if/elseif
*/
float UBaseStatsComponent::DoDamage(const FHitEntityDamage& damage)
{

	
	const float previousHP = Regeneratives[URegNames_HealthPoints - 1].Max_value;
	if (damage.DamageType != DAMAGE_TYPE_NONE)
	{
		const float dmgVal = damage.DamageValue - Resistances[damage.DamageType - 1].value;
		Regeneratives[URegNames_HealthPoints - 1].value -= dmgVal * (float)(dmgVal >= 0);
	}
	else
	{
		Regeneratives[URegNames_HealthPoints - 1].value -= damage.DamageValue;
		
	}
	//if HP is equal 0 or less
	if (Regeneratives[URegNames_HealthPoints - 1].value <= 0.0f && IsAlive)
	{
		IsAlive = false;
		EntityOwner->OnDeath();
	}
	return previousHP / Regeneratives[URegNames_HealthPoints - 1].value;
	
}
		


