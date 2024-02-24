// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseHostileClass.h"
#include "Components/CapsuleComponent.h"
#include "../Player/PlayerMovementComponent.h"
#include "../EntityAnimationBase.h"

ABaseHostileClass::ABaseHostileClass(const FObjectInitializer& ObjectInitalizer)
	: Super(ObjectInitalizer.SetDefaultSubobjectClass<UPlayerMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	EntityMovementComponent = Cast<UPlayerMovementComponent>(GetCharacterMovement());
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
	bUseControllerRotationPitch = bUseControllerRotationYaw = bUseControllerRotationRoll = false;

	bWasJumpPressed = false;
}
ABaseHostileClass::ABaseHostileClass()
{

}

void ABaseHostileClass::BeginPlay()
{
	Super::BeginPlay();
	AnimationBase = Cast<UEntityAnimationBase>(GetMesh()->GetAnimInstance());
	StatsComponent->HealFull();
}
