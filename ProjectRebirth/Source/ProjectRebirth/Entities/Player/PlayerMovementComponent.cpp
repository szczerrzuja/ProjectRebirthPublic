// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "../CombatSwitchComponent.h"

#pragma region Network
bool UPlayerMovementComponent::FSavedMove_Player::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Player* NewPMove = static_cast<FSavedMove_Player*>(NewMove.Get());
	if (Saved_bWantsToSprint != NewPMove->Saved_bWantsToSprint)
	{
		return false;
	}
	if (Saved_bWantsToDash != NewPMove->Saved_bWantsToDash)
	{
		return false;
	}

	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UPlayerMovementComponent::FSavedMove_Player::Clear()
{
	FSavedMove_Character::Clear();
	Saved_bWantsToSprint = 0;
	Saved_bWantsToDash = 0;
	Saved_bPrevWantsToCrouch = 0;
}

uint8 UPlayerMovementComponent::FSavedMove_Player::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if(Saved_bWantsToSprint)	Result |= FLAG_Sprint;
	if (Saved_bWantsToDash) Result |= FLAG_Dash;
	return Result;

}

void UPlayerMovementComponent::FSavedMove_Player::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	UPlayerMovementComponent* playerMovement = Cast<UPlayerMovementComponent>(C->GetCharacterMovement());
	
	Saved_bWantsToSprint = playerMovement->SAFE_WantToSprint;
	Saved_bPrevWantsToCrouch = playerMovement->SAFE_PrevWantToCrouch;
	Saved_bWantsToDash = playerMovement->SAFE_WantsToDash;

}

void UPlayerMovementComponent::FSavedMove_Player::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);
	UPlayerMovementComponent* playerMovement = Cast<UPlayerMovementComponent>(C->GetCharacterMovement());
	playerMovement->SAFE_WantToSprint = Saved_bWantsToSprint;
	playerMovement->SAFE_PrevWantToCrouch = Saved_bPrevWantsToCrouch;
	playerMovement->SAFE_WantsToDash = Saved_bWantsToDash;
}

UPlayerMovementComponent::FNetworkPredictionData_Client_Player::FNetworkPredictionData_Client_Player(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{

}

FSavedMovePtr UPlayerMovementComponent::FNetworkPredictionData_Client_Player::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Player);
}
#pragma endregion

UPlayerMovementComponent::UPlayerMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
}

FNetworkPredictionData_Client* UPlayerMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);

	if (ClientPredictionData == nullptr)
	{
		UPlayerMovementComponent* MutableThis = const_cast<UPlayerMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Player(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.0f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.0f;
	}
	return ClientPredictionData;
}

bool UPlayerMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;;
}

void UPlayerMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	case CMOVE_Combat:
		PhysCombat(deltaTime, Iterations);
		break;
	case CMOVE_Stunned:
		PhysCombat(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid movement mode %i"), CustomMovementMode)
	}
}

void UPlayerMovementComponent::UpdateFromCompressedFlags(uint8 flags)
{
	Super::UpdateFromCompressedFlags(flags);

	SAFE_WantToSprint = (flags & FSavedMove_Player::FLAG_Sprint); //!=0;
	SAFE_WantsToDash = (flags & FSavedMove_Player::FLAG_Dash); //!=0;
	

}

void UPlayerMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (SAFE_WantToSprint)
			MaxWalkSpeed = Sprint_MaxWalkSpeed;
		else
			MaxWalkSpeed = Walk_MaxWalkSpeed;
	}
	SAFE_PrevWantToCrouch = bWantsToCrouch;
	if(RukkiOwner->GetCombatSwitchComponenet())
		SAFE_WantCombat = RukkiOwner->GetCombatSwitchComponenet()->GetIsInCombat();

}

void UPlayerMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	RukkiOwner = Cast<AEntityInterface>(GetOwner());
}

void UPlayerMovementComponent::UpdateCharacterStateBeforeMovement(float deltaSeconds)
{


	if (SAFE_WantCombat)
	{
		SetMovementMode(MOVE_Custom, CMOVE_Combat);
	}

	if (MovementMode == MOVE_Walking && SAFE_PrevWantToCrouch)//&& !bWantsToCrouch )
	{
		if (CanSlide())
		{
			SetMovementMode(MOVE_Custom, CMOVE_Slide);
		}
	}
	if (IsCustomMovementMode(CMOVE_Slide) && !bWantsToCrouch)
	{
		SetMovementMode(MOVE_Walking);
	}

	if (SAFE_WantsToDash && CanDash())
	{
		PerformDash();
		SAFE_WantsToDash = false;
	}

	Super::UpdateCharacterStateBeforeMovement(deltaSeconds);

	


}

void UPlayerMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Slide) ExitSlide();

	if (IsCustomMovementMode(CMOVE_Slide)) EnterSlide(PreviousMovementMode, (ECustomMovementMode)PreviousCustomMode);

	if (IsFalling())
	{
		bOrientRotationToMovement = true;
	}
	if (IsCustomMovementMode(CMOVE_Combat)) {
		EnterCombat(PreviousMovementMode, (ECustomMovementMode)PreviousCustomMode);
	}
}

bool UPlayerMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide);
}

bool UPlayerMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}
#pragma region Slide

void UPlayerMovementComponent::EnterSlide(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	//basicly i want to make capsule shrink at sliding and i am using implemented crounching
	bWantsToCrouch = true;
	bOrientRotationToMovement = false;
	Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;

	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true, NULL);
}

void UPlayerMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;
	bOrientRotationToMovement = true;
}

void UPlayerMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}


	if (!CanSlide())
	{
		SetMovementMode(MOVE_Walking);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;

	// Perform the simulation of the move
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Save current values
		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;
		const float accelerationLeng = Acceleration.Length();
		const FVector acc = Acceleration.GetSafeNormal2D();

		FVector SlopeForce = CurrentFloor.HitResult.Normal;
		SlopeForce.Z = 0.f;
		Velocity += SlopeForce * Slide_GravityForce * deltaTime;

		//this line is responsible for converting input vector of WSAD (AddInputVector converts input vector to acceleration in movementComponent) to move when sliding
		
		Acceleration = (UpdatedComponent->GetRightVector() * acc.Y* Slide_ControllMultiplayer_Y +
			UpdatedComponent->GetForwardVector() * FMath::Max(acc.X, -0.6f) * Slide_ControllMultiplayer_X)* accelerationLeng;


		// Apply acceleration
		CalcVelocity(timeTick, GroundFriction * Slide_Friction, false, GetMaxBrakingDeceleration());

		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;
		bool bFloorWalkable = CurrentFloor.IsWalkableFloor();

		//check if can still slide
		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			// try to move forward
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);

			if (IsFalling())
			{
				// pawn decided to jump up
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
				}
				StartNewPhysics(remainingTime, Iterations);
				return;
			}
			else if (IsSwimming()) //just entered water
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}


		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			// calculate possible alternate movement
			const FVector GravDir = FVector(0.f, 0.f, -1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);
			if (!NewDelta.IsZero())
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			// check if just entered water
			if (IsSwimming())
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;
			}
		}

		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround() && bFloorWalkable)
		{
			// Make velocity reflect actual move
			if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
	}


	FHitResult Hit;
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(Velocity.GetSafeNormal2D(), FVector::UpVector).ToQuat();
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, false, Hit);
}

bool UPlayerMovementComponent::CanSlide() const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	bool bValidSurface = GetWorld()->LineTraceTestByProfile(Start, End, ProfileName, RukkiOwner->GetIgnoreCharacterParams());
	bool bEnoughSpeed = Velocity.SizeSquared() > pow(Slide_MinSpeed, 2);
	
	return bValidSurface && bEnoughSpeed;
}
#pragma endregion
#pragma region Climb

void UPlayerMovementComponent::EnterClimb(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
}

void UPlayerMovementComponent::ExitClimb()
{
}

void UPlayerMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
}

bool UPlayerMovementComponent::CanClimb() const
{
	return false;
}
#pragma endregion
#pragma region Dash
void UPlayerMovementComponent::OnDashCooldownFinished()
{
	SAFE_WantsToDash = true;
}
bool UPlayerMovementComponent::CanDash() const
{
	return (IsWalking() && !IsCrouching()) || IsFalling();
}

void UPlayerMovementComponent::PerformDash()
{
	DashStartTime = GetWorld()->GetTimeSeconds();

	FVector DashDirection = (Acceleration.IsNearlyZero() ? UpdatedComponent->GetForwardVector() : Acceleration).GetSafeNormal2D();
	Velocity = Dash_Impulse * (DashDirection + FVector::UpVector * 0.1f);

	FQuat NewRotation = FRotationMatrix::MakeFromXZ(DashDirection, FVector::UpVector).ToQuat();
	FHitResult hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, false, hit);

	SetMovementMode(MOVE_Falling);

	DashStartDelegete.Broadcast();
}
#pragma endregion
#pragma region Combat
void UPlayerMovementComponent::EnterCombat(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	
}
void UPlayerMovementComponent::ExitCombat()
{
	
}
void UPlayerMovementComponent::PhysCombat(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CanCombat())
	{
		SetMovementMode(MOVE_Walking);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}



}

bool UPlayerMovementComponent::CanCombat() const
{
	return (bool)SAFE_WantCombat;
}

#pragma endregion
void UPlayerMovementComponent::SprintON()
{
	SAFE_WantToSprint = true;
}

void UPlayerMovementComponent::SprintOFF()
{
	SAFE_WantToSprint = false;
}

void UPlayerMovementComponent::CrouchPressed()
{
	bWantsToCrouch = ~bWantsToCrouch;
}

void UPlayerMovementComponent::DashPressed()
{
	float Current = GetWorld()->GetTimeSeconds();
	if (Current - DashStartTime > Dash_CooldownDur)
	{
		SAFE_WantsToDash = true;
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(Dash_CooldownTimerHandle, this, &UPlayerMovementComponent::OnDashCooldownFinished, Dash_CooldownDur-(Current-DashStartTime));
	}
}

void UPlayerMovementComponent::DashReleased()
{
	GetWorld()->GetTimerManager().ClearTimer(Dash_CooldownTimerHandle);
	SAFE_WantsToDash = false;
}

float UPlayerMovementComponent::GetMaxSpeed() const
{
	if (IsMovementMode(MOVE_Walking) && SAFE_WantToSprint && !IsCrouching()) return Sprint_MaxWalkSpeed;

	if (MovementMode != MOVE_Custom) return Super::GetMaxSpeed();

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return Slide_MaxSpeed;
	case CMOVE_Climb:
		if (Velocity.Z < 0.0f) return Climb_SpeedDown;
		else return Climb_SpeedUp;
	case CMOVE_Combat:
		return 0.0f;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
			return -1.f;
	}
}

float UPlayerMovementComponent::GetMaxBrakingDeceleration() const
{

	if (MovementMode != MOVE_Custom) return Super::GetMaxBrakingDeceleration();

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return Slide_BrakingDeceleration;
	case CMOVE_Climb:
		return Climb_BrakingDeceleration;
	case CMOVE_Combat:
		return 0.0f;
	default:
		return Super::GetMaxBrakingDeceleration();
		;
	}
}
/*
* Function return id of combined movement states. Each state is unique
* 1 - Walking
* 2 - NavWalking
* 3 - Falling
* 4 - Swimming
* 5 - Flying
* 6 - custom / not used
* 7 - Slide
* 8 - Combat
* 9 - climb (not implemented)
* 10 -  WallRun (not Implemented)
* 11 - stunned
*/
int UPlayerMovementComponent::GetCurrentMovementMode() const {
		return (int)MovementMode+(int)CustomMovementMode;
}

void UPlayerMovementComponent::SetWantCombat(bool state)
{

	SAFE_WantCombat = state;
}


bool UPlayerMovementComponent::IsMovementMode(EMovementMode InMovementMode) const
{
	return InMovementMode == MovementMode;
}

