// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatSwitchComponent.generated.h"


UENUM(BlueprintType)
enum EWeaponType : uint8
{
    WEAPON_None			UMETA(Hidden),
    WEAPON_Melee		UMETA(DisplayName = "Melee"),
    WEAPON_Sword		UMETA(DisplayName = "Sword"),
    WEAPON_Max			UMETA(Hidden),
};
UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum EInputCombo : int32
{
    COMBAT_None = 0x00              UMETA(Hidden),
    COMBAT_Froward = 0x01           UMETA(DisplayName = "MoveForward"),
    COMBAT_Backward = 0x02          UMETA(DisplayName = "MoveBackward"),
    COMBAT_Right = 0x04             UMETA(DisplayName = "MoveRight"),
    COMBAT_Left = 0x08              UMETA(DisplayName = "MoveLeft"),
    COMBAT_Attack1 = 0x10           UMETA(DisplayName = "Attack1"),
    COMBAT_Attack2 = 0x20           UMETA(DisplayName = "Attack2"),
    COMBAT_DistantSpell = 0x40      UMETA(DisplayName = "DistantSpell"),
    COMBAT_Block = 0x80             UMETA(DisplayName = "Block"),
    COMBAT_Max = 0xfffffff          UMETA(Hidden)
};
USTRUCT(BlueprintType)
struct FNonComboMove
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FVector3f KnockbackLocalDirection;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float knockabackStrenght;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        uint8 aimationCode;
    UPROPERTY(EditAnywhere, BlueprintReadWRite, meta = (BitMask, BitmaskEnum = "/Script/ProjectRebirth.EInputCombo"))
        int inputCode;
    UPROPERTY(EditAnywhere, BlueprintReadWRite)
        bool bChargeAble; 
    UPROPERTY(EditAnywhere, BlueprintReadWRite)
        float chargeTime;

};
USTRUCT(BlueprintType)
struct FAnimationInputRelation : public FNonComboMove
{
    GENERATED_BODY()
    /*
    * This property have an id of the attack after input is correct.  
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int nextAttackArrayCode;


};
/*
* Helper struct for contain switching behaviour, current move effects/damage
*/
USTRUCT(BlueprintType)
struct FMoveHandler
{
    GENERATED_BODY()       
    UPROPERTY(EditAnywhere, BlueprintReadWRite, Category = "Combos")
    TEnumAsByte<EWeaponType> WeaponType;
    //first is input next is id of animation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combos")
    TArray<FAnimationInputRelation> ComboArray; 

};

/**
 * This class is used to switch between combat animations. With animations are:
 * 1. Launched other objects and animations
 * 2. Turning on/off ceratin colliders
 * 3. Other things i dont know
 */
UCLASS(Blueprintable)
class PROJECTREBIRTH_API UCombatSwitchComponent : public UActorComponent
{
    GENERATED_BODY()
#pragma region CombatSwtiches
protected:

    bool bIsAnimationReady;
    bool bIsMovementComponentReady;
    bool bIsInCombat;
    bool bTrasistedInCurrentFrame;
    bool bHasInputChanged;
    bool bIsInCombo;

    BYTE currentAnimation;
    uint32 currentComboState;
    uint32 CombatFlags;

    UPROPERTY(Transient) class AEntityInterface* EntityOnwer;
    UPROPERTY(EditAnywhere, BLueprintReadWrite, Category = "Combos List", meta = (AllowPrivateAccess = "true"))
        TArray<FNonComboMove> NonComboMoves;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combos List", meta = (AllowPrivateAccess = "true"))
        TArray<FMoveHandler> Combos;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combos Param", meta = (AllowPrivateAccess = "true"))
        float timeToResetCombo;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combos Param", meta = (AllowPrivateAccess = "true"))
        int moveAfterReset;
public:
    
    UCombatSwitchComponent();
    UCombatSwitchComponent(const FObjectInitializer& ObjectInitializer);
   


     void InitializeComponent() override;
     void BeginPlay() override;
     void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* T) override;


     void SetCombatFlags(uint32 combFlags, uint32 switchingMask);
     UFUNCTION(BlueprintCallable)
         bool IsInCombatMode() const;
     UFUNCTION(BlueprintCallable)
         uint8 GetCurrentAnimation() {         return this->currentAnimation;     }
    
     UFUNCTION(BlueprintCallable)
         int32 GetCurrentComboState() const { return (int32)this->currentComboState; }
     UFUNCTION(BlueprintCallable)
         void SetIsInCombat(bool state);
     UFUNCTION(BlueprintCallable)
         bool GetIsInCombat() const;

private:
    bool switchAnimation();
     
#pragma endregion
};
