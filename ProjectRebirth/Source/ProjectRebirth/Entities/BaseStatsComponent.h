// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseStatsComponent.generated.h"

#pragma region ENUMS
namespace StatisticsEnums
{
	enum RegenerativesKeys : uint8
	{
		Max_value,
		Regeneration,
		CurrentValue
	};
}
/*
* StatRegen are all stats with regen abilities, like mana, Health, stamina and also thigs thats will regen on condition
*/
UENUM(BlueprintType)
enum UStatisticType : uint8
{
	STAT_NONE = 0				UMETA(Hidden),
	STAT_Regeneratives			UMETA(DisplayName = "Regeneratives"),
	STAT_Resistances			UMETA(DisplayName = "Resistance"),
	STAT_EffectRes				UMETA(DisplayName = "Efect Resistance"),
	STAT_utility				UMETA(DisplayName = "Utility"),
	STAT_Max					UMETA(Hidden)
};

UENUM(BlueprintType)
enum URegenType : uint8
{
	REGENTYPE_NONE				UMETA(Hidden),
	REGENTYPE_BySecond			UMETA(DisplayName = "Regen every Second"),
	REGENTYPE_OnDamageRecive	UMETA(DisplayName = "Regen on hits"),
	REGENTYPE_Max				UMETA(Hidden)
};


UENUM(BlueprintType)
enum URegenStatsNames : uint8
{
	URegNames_NONE					UMETA(Hidden),
	URegNames_HealthPoints			UMETA(DisplayName = "HealthPoints"),	
	URegNames_Custom				UMETA(Hidden)
};

UENUM(BlueprintType)
enum UResitanceTypeStat : uint8
{
	DAMAGE_TYPE_NONE				UMETA(Hidden),
	DAMAGE_TYPE_True				UMETA(DisplayName = "True"),
	DAMAGE_TYPE_Slash				UMETA(DisplayName = "Slash"),
	DAMAGE_TYPE_stab 				UMETA(DisplayName = "Stab "),
	DAMAGE_TYPE_Blunt				UMETA(DisplayName = "Blunt"),
	DAMAGE_TYPE_Fire				UMETA(DisplayName = "Fire"),
	DAMAGE_TYPE_Ice					UMETA(DisplayName = "Ice"),
	DAMAGE_TYPE_Wind				UMETA(DisplayName = "Wind"),
	DAMAGE_TYPE_Earth				UMETA(DisplayName = "Earth"),
	DAMAGE_TYPE_Lightining			UMETA(DisplayName = "lightining"),
	DAMAGE_TYPE_Dark				UMETA(DisplayName = "Dark"),
	DAMAGE_TYPE_Light				UMETA(DisplayName = "Light"),
	DAMAGE_TYPE_Spirit				UMETA(DisplayName = "Spirit"),
	DAMAGE_TYPE_MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum UEffectResistancesStat : uint8
{
	EFFECT_NONE					UMETA(Hidden),
	EFFECT_Knockback			UMETA(DisplayName = "Knockback"),
	EFFECT_Stun					UMETA(DisplayName = "Stun"),
	EFFECT_Poison				UMETA(DisplayName = "Poison"),
	EFFECT_Max					UMETA(Hidden)
};

UENUM(BlueprintType)
enum UUtilityStats : uint8
{
	UTILITY_NONE				UMETA(Hidden),
	UTILITY_MovementSpeed		UMETA(DisplayName = "Movement Speed modifier"),
	UTILITY_JumpCount			UMETA(DisplayName = "Jump Count"),
	UTILITY_Max					UMETA(Hidden)
};
#pragma endregion
/*
* Helper Classes for hit event handling
*/
USTRUCT(BlueprintType)
struct FHitEntityDamage {
	GENERATED_BODY()

	FHitEntityDamage();
	FHitEntityDamage(float damageVal, TEnumAsByte<UResitanceTypeStat> dmgType);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<UResitanceTypeStat> DamageType;
};

USTRUCT(BlueprintType)
struct FHitEntityEffect {
	GENERATED_BODY()

		FHitEntityEffect();
	FHitEntityEffect(float duration, float strenght, TEnumAsByte<UEffectResistancesStat> effectType);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Strenght;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<UEffectResistancesStat> EffectType;
};



/*
* Simple class for parameters like resistances, power buffs ?
*/

USTRUCT(BlueprintType)
struct FUStat
{
	GENERATED_BODY()

public:
	FUStat();
	FUStat(float val);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float value;


};
/*
* Simple class for parameters like health with health regen ect.
*/

USTRUCT(BlueprintType)
struct FUStatRegen : public FUStat
{
	GENERATED_BODY()

private:
	
public:
	FUStatRegen();
	FUStatRegen(float reg_val, float currval, float maxVal, TEnumAsByte<URegenStatsNames> regName, TEnumAsByte<URegenType> regType);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Regen_Value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Max_value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<URegenType> RegenType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<URegenStatsNames> RegenName;

	float const& operator[] (std::size_t i);

	

};
USTRUCT(BlueprintType)
struct FUResStat : public FUStat
{
	GENERATED_BODY()

public:
	FUResStat();
	FUResStat(float val, TEnumAsByte<UResitanceTypeStat> resNam);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<UResitanceTypeStat> ResistanceName;
};
USTRUCT(BlueprintType)
struct FUEffectResistance : public FUStat
{
	GENERATED_BODY()

public:
	FUEffectResistance();
	FUEffectResistance(float val, TEnumAsByte<UEffectResistancesStat> resNam);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<UEffectResistancesStat> FUEffectResist;
};

USTRUCT(BlueprintType)
struct FUUtility : public FUStat
{
	GENERATED_BODY()

public:
	FUUtility();
	FUUtility(float val, TEnumAsByte<UUtilityStats> resNam);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<UUtilityStats> FUUtilityName;
};
/*
* TODO: Create methods for applying damage, aply temporary effects like poison, slowness
* Integration with movement component. Method for damage must pass array for multiple damages
* After that improve method for recieve damage for inluding damage resistance
* TODO2: Create Basic Stats - containing basic MaxHp, Regeneration, Resistances ect.CurrentStast and Effects - reduce/increase certain stats for time 
* that are affecting stats. For Player starts from equipment
* This class MUST contain seperate classes for that becouse it MUST HAVE HIGH SCALABILITY
* Calculating current state of character must be done here EXCEPT health and other regeneratives calculation 
* This MUST BE FAST 
*/
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTREBIRTH_API UBaseStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseStatsComponent();
private:
	void RechargeOnHit();
	void RechargeOnSecond(float DeltaTime);
	class AEntityInterface* EntityOwner;
	void InitStats();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", TitleProperty = "RegenName"))
	TArray<FUStatRegen> Regeneratives;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", TitleProperty = "ResistanceName"))
	TArray<FUResStat> Resistances;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", TitleProperty = "FUEffectResist"))
	TArray<FUEffectResistance> EffectRes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", TitleProperty = "FUUtilityName"))
	TArray<FUUtility> Utilities;
	bool IsAlive;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void InitializeComponent() override;
	void HealFull();
	void OnRespawn();
#pragma region OnHitArea
public:
	//returns how much in % current hp changed
	float DoDamage(const FHitEntityDamage& damage);
	void ApplyEffect(const FHitEntityEffect& Effect);
#pragma endregion
#pragma region getters/setters
public:

	UFUNCTION(BlueprintCallable)	
		FUStatRegen GetRegenarative(uint8 type) { return Regeneratives[(int)type - 1]; }
	UFUNCTION(BlueprintCallable)	
		FUStat GetResistances(uint8 type) { return Resistances[type - 1]; }
	UFUNCTION(BlueprintCallable)	
		FUStat GetEffectRes(uint8 type) { return EffectRes[type - 1]; }
	UFUNCTION(BlueprintCallable)	
		FUStat GetUtilitys(uint8 type) { return Utilities[type - 1]; }
	UFUNCTION(BlueprintCallable)	
		bool GetIsAlive() { return IsAlive; }

#pragma endregion 


};


