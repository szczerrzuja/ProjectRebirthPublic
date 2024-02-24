// Fill out your copyright notice in the Description page of Project Settings.


#include "Rukki.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include <EnhancedInputSubsystems.h>
#include "PlayerMovementComponent.h"
#include "../EntityAnimationBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "../HostileEntities/BaseHostileClass.h"
#include "RebirthPlayerController.h"



// Sets default values
ARukki::ARukki(const FObjectInitializer& ObjectInitalizer) 
	: Super(ObjectInitalizer.SetDefaultSubobjectClass<UPlayerMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	//compilation error below actualy compiles. Maybe some Visual Studio 2022 bug, or maybe something is wrong 
	EntityMovementComponent = Cast<UPlayerMovementComponent>(GetCharacterMovement());
	//in the fufter player could have different stat and combat components
	CombatComponent = CreateDefaultSubobject<UCombatSwitchComponent>(TEXT("CombatComponent"));
	StatsComponent = CreateDefaultSubobject<UBaseStatsComponent>(TEXT("Stast component"));
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//in order to use movement component coliding componentn MUST be root component :)
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Handler"));
	SpringArmComp->bInheritPitch = SpringArmComp->bInheritRoll = SpringArmComp->bInheritYaw = false;
	SpringArmComp->SetupAttachment(GetCapsuleComponent());
	SpringArmComp->bUsePawnControlRotation = true;

	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	//Attach the Spring Arm to the Character's Skeletal Mesh Component
	//Attach the Camera to the SpringArmComponent
	cameraComponent->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	bUseControllerRotationPitch = bUseControllerRotationYaw = bUseControllerRotationRoll = false;
	bWasJumpPressed = false;
	

}

// Called when the game starts 
void ARukki::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->JumpZVelocity = FMath::Sqrt(-2.0f * GetCharacterMovement()->GetGravityZ() * jumpHeight);

	//CanMove = true;
	AnimationBase = Cast<UEntityAnimationBase>(GetMesh()->GetAnimInstance());
	StatsComponent->HealFull();
}
	

// Called every frame
void ARukki::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//CAMERA UPDATE
	CameraLockOn();


		
}


void ARukki::SpawnRukkiProjectile()
{

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	AProjectileClass* ProjectileRef = GetWorld()->SpawnActor<AProjectileClass>(RukkiProjectileTemplate ,GetActorForwardVector() * 2 + GetActorLocation(),
		GetActorRotation(), SpawnParams);
	if (!ProjectileRef)
		return;	

	if (bIsLockedOnTarget && LockedOnTarget)
	{
		
		ProjectileRef->InitializeProjectile(GetActorForwardVector() * 2000.0f, FHitEntityDamage(30.0f, DAMAGE_TYPE_Lightining), LockedOnTarget->GetCapsuleComponent());
	}
	else
	{
		ProjectileRef->InitializeProjectile(GetActorForwardVector() * 2000.0f, FHitEntityDamage(30.0f, DAMAGE_TYPE_Lightining), NULL);
		
	}

	
}

void ARukki::ClampCameraHeight()
{
}

void ARukki::CameraLockOn()
{
	//TODO: Controller yaw and pitch rotation depending on the input and velocity (problem with running away from enemy)
	//THIS IS NO URGENT
	if (LockedOnTarget && bIsLockedOnTarget)
	{
		FVector direction = LockedOnTarget->GetActorLocation() - this->GetActorLocation();
		FRotator deltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), direction.ToOrientationRotator());
		
		
		if (deltaRotation.Yaw * deltaRotation.Yaw + deltaRotation.Pitch * deltaRotation.Pitch < 25.0f || bGlueCameraToTarget) {
			bGlueCameraToTarget = true;

			deltaRotation.Roll = 0.0f;
			Controller->SetControlRotation(GetControlRotation() - deltaRotation);
		}
		else
		{
			AddControllerYawInput(-deltaRotation.Yaw * CameraSmoothRotationRate * GetWorld()->DeltaTimeSeconds);
			AddControllerPitchInput(deltaRotation.Pitch * CameraSmoothRotationRate * GetWorld()->DeltaTimeSeconds);
		}

	}
}

void ARukki::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("RUKKI DIED"));
	bCanLockOnMe = false;
	UnlockLockedOnMe();
	LockedOnMe.Empty();
	LockedOnMe.Shrink();
	GetWorld()->GetTimerManager().SetTimer(AfterDeathTimerHandle, this, &ARukki::AfterDeath, TimeToSweepBody, false);
}


void ARukki::AfterDeath()
{
	ARebirthPlayerController* controller = Cast<ARebirthPlayerController>(GetController());
	if (controller)
	{
		//TODO: after death penalty
		Destroy();
		controller->Respawn();

	}

}

void ARukki::OnRespawn()
{
	StatsComponent->OnRespawn();
}

void ARukki::OnMove(const FInputActionValue& Value)
{
	
	FVector vec3 = FVector(Value[1], Value[0], 0.0f);
	const float leng = vec3.Length();
	vec3.Normalize();
	
	if (EntityMovementComponent->CustomMovementMode == CMOVE_Slide) {
		AddMovementInput(vec3, FMath::Min(1.0f, leng));
	}
	else {
		const FRotator Rotation = FRotator(0.0f,Controller->GetControlRotation().Yaw,0.0f);
		AddMovementInput(Rotation.RotateVector(vec3), FMath::Min(1.0f, leng));
	}


}

void ARukki::OnJump(const FInputActionValue& Value)
{
	bPressedJump = true;
	bWasJumpPressed = true;
	Super::Jump();
	
}
void ARukki::OnReleaseJump(const FInputActionValue& Value)
{
	
	Super::StopJumping();
	bPressedJump = false;
	bWasJumpPressed = false;
	
}
void ARukki::OnCameraMove(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxis.X * GetWorld()->GetDeltaSeconds() * LookSensivityY * !bIsLockedOnTarget);
	AddControllerPitchInput(-LookAxis.Y * GetWorld()->GetDeltaSeconds() * LookSensivityX * !bIsLockedOnTarget);
}
void ARukki::OnSprint(const FInputActionValue& Value)
{
	EntityMovementComponent->SprintON();
}
void ARukki::OnReleaseSprint(const FInputActionValue& Value)
{
	EntityMovementComponent->SprintOFF();
}
void ARukki::OnCruch(const FInputActionValue& Value)
{
	EntityMovementComponent->CrouchPressed();
}
void ARukki::OnDash(const FInputActionValue& Value)
{
	EntityMovementComponent->DashPressed();
}
void ARukki::OnBlock(const FInputActionValue& Value) 
{
	CombatComponent->SetCombatFlags((uint32)COMBAT_Block, (uint32)COMBAT_Block);
	CombatComponent->SetIsInCombat(true);
	//RukkiMovementComponent->SetWantCombat(true);

}

void ARukki::OnReleaseBlock(const FInputActionValue& Value) {
	CombatComponent->SetCombatFlags(0, (uint32)COMBAT_Block);
	CombatComponent->SetIsInCombat(false);
	//RukkiMovementComponent->SetWantCombat(false);
}
void ARukki::OnAttack_1(const FInputActionValue& Value) {
	CombatComponent->SetCombatFlags((uint32)COMBAT_Attack1, (uint32)COMBAT_Attack1);

}
void ARukki::OnReleaseAttack_1(const FInputActionValue& Value) {
	CombatComponent->SetCombatFlags(0, (uint32)COMBAT_Attack1);

}
void ARukki::OnAttack_2(const FInputActionValue& Value) {
	CombatComponent->SetCombatFlags((uint32)COMBAT_Attack2, (uint32)COMBAT_Attack2);

}
void ARukki::OnReleaseAttack_2(const FInputActionValue& Value) {
	CombatComponent->SetCombatFlags(0 , (uint32)COMBAT_Attack2);

}
void ARukki::OnLockOnTarget(const FInputActionValue& Value)
{
	if (bIsLockedOnTarget)
	{
		ReleaseLockOnTargetAnyway();
		bGlueCameraToTarget = false;
	}
	else
	{
		SearchForTarget();
	}
}
void ARukki::OnShootProjectileSimple(const FInputActionValue& Value)
{
	SpawnRukkiProjectile();
}

void ARukki::PawnClientRestart()
{
	Super::PawnClientRestart();
	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();

			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(basicMapingContext, Priority);
		}
	}
}

// Called to bind functionality to input
void ARukki::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		if (A_Move)
		{
			PlayerEnhancedInputComponent->BindAction(A_Move, ETriggerEvent::Triggered, this, &ARukki::OnMove);
		}
		if (A_Jump)
		{
			PlayerEnhancedInputComponent->BindAction(A_Jump, ETriggerEvent::Started, this, &ARukki::OnJump);
			PlayerEnhancedInputComponent->BindAction(A_Jump, ETriggerEvent::Completed, this, &ARukki::OnReleaseJump);
		}
		if (A_CameraRotation)
		{
			PlayerEnhancedInputComponent->BindAction(A_CameraRotation, ETriggerEvent::Triggered, this, &ARukki::OnCameraMove);
		}
		if (A_Sprint)
		{
			PlayerEnhancedInputComponent->BindAction(A_Sprint, ETriggerEvent::Started, this, &ARukki::OnSprint);
			PlayerEnhancedInputComponent->BindAction(A_Sprint, ETriggerEvent::Completed, this, &ARukki::OnReleaseSprint);
		}
		if (A_Crouch)
		{
			PlayerEnhancedInputComponent->BindAction(A_Crouch, ETriggerEvent::Started, this, &ARukki::OnCruch);
		}
		if (A_Dash)
		{
			PlayerEnhancedInputComponent->BindAction(A_Dash, ETriggerEvent::Started, this, &ARukki::OnDash);
		}
		if (A_Attack_1){
			PlayerEnhancedInputComponent->BindAction(A_Attack_1, ETriggerEvent::Started, this, &ARukki::OnAttack_1);
			PlayerEnhancedInputComponent->BindAction(A_Attack_1, ETriggerEvent::Completed, this, &ARukki::OnReleaseAttack_1);
		}
		if (A_Attack_2) {
			PlayerEnhancedInputComponent->BindAction(A_Attack_2, ETriggerEvent::Started, this, &ARukki::OnAttack_2);
			PlayerEnhancedInputComponent->BindAction(A_Attack_2, ETriggerEvent::Completed, this, &ARukki::OnReleaseAttack_2);
		}
		if (A_Attack_Block) {
			PlayerEnhancedInputComponent->BindAction(A_Attack_Block, ETriggerEvent::Started, this, &ARukki::OnBlock);
			PlayerEnhancedInputComponent->BindAction(A_Attack_Block, ETriggerEvent::Completed, this, &ARukki::OnReleaseBlock);
		}
		if (A_CameraLock)
		{
			PlayerEnhancedInputComponent->BindAction(A_CameraLock, ETriggerEvent::Started, this, &ARukki::OnLockOnTarget);
		}
		if (A_Shoot_Projectile)
		{
			PlayerEnhancedInputComponent->BindAction(A_Shoot_Projectile, ETriggerEvent::Started, this, &ARukki::OnShootProjectileSimple);
		}
	}
	
}

void ARukki::OnHitWithAttack(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, 
	int32 otherBodyIndex, bool bfromSweep, const FHitResult& Hit)
{
	AEntityInterface* AttackedEntity = Cast<AEntityInterface>(otherActor);
	if (AttackedEntity && this != otherActor)
	{
		//TODO: Knockbac depending on attack knockback direction
		AttackedEntity->OnCollisionHostality(FHitEntityDamage(100.0f, DAMAGE_TYPE_Fire), FHitEntityEffect(), 1000.0f, this->GetActorForwardVector());
		
	}
}