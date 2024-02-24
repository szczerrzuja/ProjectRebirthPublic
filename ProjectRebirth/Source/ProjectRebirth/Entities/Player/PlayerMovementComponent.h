// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Rukki.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMovementComponent.generated.h"

//delegates are dynamic functions that can be bind and called

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDashStartDelegate);

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None			UMETA(Hidden),
	CMOVE_Slide			UMETA(DisplayName = "Slide"),
	CMOVE_Combat		UMETA(DisplayName = "Combat"),
	CMOVE_Climb			UMETA(DisplayName = "Climb"),
	CMOVE_WallRun		UMETA(DisplayName = "WallRun"),
	CMOVE_Stunned		UMETA(DisplayName = "Stun/Knockback"),
	CMOVE_Max			UMETA(Hidden),
};

/**
 * 
 */
UCLASS()
class PROJECTREBIRTH_API UPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	//those classes are more smaller than UObject and they will be deleted and created alot. They are more c++ class
	class FSavedMove_Player : public FSavedMove_Character
	{
	public:
		typedef FSavedMove_Character Super;
		enum CompressedFlags
		{
			FLAG_Sprint		= 0x10,
			FLAG_Dash		= 0x20,
			FLAG_Custom_2	= 0x40,
			FLAG_Custom_3	= 0x80,
		};

		uint8 Saved_bWantsToSprint : 1;
		uint8 Saved_bPrevWantsToCrouch : 1;
		uint8 Saved_bWantsToDash : 1;
		//Check if two moves can be combined, animation related ?
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		//clears saved flags
		virtual void Clear() override;
		/*
		* reduces data send to server by compressing flags to one variable
		* USE ONLY FOR THINGS LIKE SPRINT, WALLRUN, CLIMB corelated with inputm, dash can be send in RPC or something like that
		*/
		virtual uint8 GetCompressedFlags() const override;
		/*
		* Set saved fields of SavedMove depending on character movement compoenent fields
		*/
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		/*
		* Set saved fields of  character movement depending on SavedMove  fields
		*/
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_Player : public FNetworkPredictionData_Client_Character
	{
	public:

		FNetworkPredictionData_Client_Player(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;
		
		virtual FSavedMovePtr AllocateNewMove() override;
	};
#pragma region Slide/Crouch
public:
	bool SAFE_PrevWantToCrouch;
	bool SAFE_WantsToDash;
	//determinates minimum speed to perform slide
	UPROPERTY(EditDefaultsOnly) float Slide_MinSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly) float Slide_MaxSpeed = 400.f;
	//determinates force value added at start of slide
	UPROPERTY(EditDefaultsOnly) float Slide_EnterImpulse = 400.f;
	//determinates how fast slope will change velocity
	UPROPERTY(EditDefaultsOnly) float Slide_GravityForce = 4000.f;
	UPROPERTY(EditDefaultsOnly) float Slide_BrakingDeceleration = 1000.f;
	//determinates friction
	UPROPERTY(EditDefaultsOnly) float Slide_Friction = 0.06f;
	UPROPERTY(EditDefaultsOnly) float Slide_ControllMultiplayer_X = 4.f;
	UPROPERTY(EditDefaultsOnly) float Slide_ControllMultiplayer_Y = 4.f;

private:

	//SLIDE
	void EnterSlide(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode);
	void ExitSlide();
	void PhysSlide(float deltaTime, int32 Iterations);
	bool CanSlide() const;

#pragma endregion
#pragma region Climbing
public:
	UPROPERTY(EditDefaultsOnly) float Climb_SpeedUp = 300.0f;
	UPROPERTY(EditDefaultsOnly) float Climb_SpeedDown = 400.0f;
	UPROPERTY(EditDefaultsOnly) float Climb_BrakingDeceleration = 1000.0f;
private:

	//for MovementModes methods
	//SLIDE
	void EnterClimb(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode);
	void ExitClimb();
	void PhysClimb(float deltaTime, int32 Iterations);
	bool CanClimb() const;
#pragma endregion
#pragma region Dash
	/*
	* Dash is a movement move (not state) that adds velocity in direction. 
	*/
public:
	//delegate
	UPROPERTY(BlueprintAssignable) FDashStartDelegate DashStartDelegete;

	UPROPERTY(EditDefaultsOnly) float Dash_Impulse = 300.0f;
	UPROPERTY(EditDefaultsOnly) float Dash_CooldownDur = 1.0f;
private:
	FTimerHandle Dash_CooldownTimerHandle;

	void OnDashCooldownFinished();
	bool CanDash() const;
	void PerformDash();
#pragma endregion
#pragma region WallRun
	
#pragma endregion
#pragma region Combat
	public:
		bool SAFE_WantCombat;
	private:
		//combat functions
		void EnterCombat(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode);
		void ExitCombat();
		void PhysCombat(float deltaTime, int32 Iterations);
		bool CanCombat() const;

#pragma endregion

public:


	UPROPERTY(Transient) class AEntityInterface* RukkiOwner;
	//safe variable for sprinting
	bool SAFE_WantToSprint;
	float DashStartTime;
	UPROPERTY(EditDefaultsOnly) float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly) float Walk_MaxWalkSpeed;

public:

	UPlayerMovementComponent();
	//if no predictionData_Client create one :)
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	UFUNCTION(BlueprintPure) 
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	
	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;

protected:
	//calling custom Phys method, for example PhysSlide
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	//handles change of flags depending on flags from server??
	virtual void UpdateFromCompressedFlags(uint8 flags) override;
	//method for movement logic, used to change SAFE flags
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void InitializeComponent() override;
	//method to make transitions between movement modes
	virtual void UpdateCharacterStateBeforeMovement(float deltaSeconds) override;
	// on changed movement mode
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

private:

/*
* Getters/setters/change flags
*/
public:
	UFUNCTION(BlueprintCallable) void SprintON();
	UFUNCTION(BlueprintCallable) void SprintOFF();
	UFUNCTION(BlueprintCallable) void CrouchPressed();
	UFUNCTION(BlueprintCallable) void DashPressed();
	UFUNCTION(BlueprintCallable) void DashReleased();

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	UFUNCTION(BlueprintPure) bool IsMovementMode(EMovementMode InMovementMode) const;
	UFUNCTION(BlueprintPure) int GetCurrentMovementMode() const;
	UFUNCTION(BlueprintCallable) void SetWantCombat(bool state);
	UFUNCTION(BlueprintPure) bool GetWantCombat() { return SAFE_WantCombat; }

};
