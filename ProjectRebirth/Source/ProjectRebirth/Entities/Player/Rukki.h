// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputActionValue.h"
#include "CoreMinimal.h"
#include "../EntityInterface.h"
#include "../Projectiles/ProjectileClass.h"
#include "Rukki.generated.h"

UCLASS()
class PROJECTREBIRTH_API ARukki : public AEntityInterface 
{
	GENERATED_BODY()

#pragma region COMPONENTS
private:

	//componenets

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true") )
		class UCameraComponent* cameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArmComp;



#pragma endregion


public:
	// Sets default values for this character's properties
	ARukki(const FObjectInitializer& ObjectInitalizer );

	/*
	//OnTrigger
	UFUNCTION()
		void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex,
			bool bfromSweep, const FHitResult& SweepResoult);
	*/
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



#pragma region Input
public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Player controller for more advanced input controll
	// Pointers to input actions
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_Move;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_Jump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_CameraRotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_CameraLock;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_Sprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_Crouch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_Dash;
	/*
	* those input methods are for combat related stuff, and won't be calling movementcontroller methods directly
	* Input methods will be setting boolean variables or maybe combined boolean to one input vector
	* 
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_Attack_1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_Attack_2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_Attack_Block;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputAction* A_Shoot_Projectile;

protected:
	//input action map context
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Input Mapping")
		class UInputMappingContext* basicMapingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Priority")
		int32 Priority = 0;




#pragma endregion
#pragma region Camera&Combat
private:
	bool bGlueCameraToTarget = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Values", meta = (AllowPrivateAccess = "true"))
		float LookSensivityX = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Controls|Values", meta = (AllowPrivateAccess = "true"))
		float LookSensivityY = 50.0f;
	//for camera lockon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float CameraSmoothRotationRate = 50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float CameraMaxZRotation = 50.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float CameraMinZRotation = -50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Projectile", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AProjectileClass> RukkiProjectileTemplate;

	void SpawnRukkiProjectile();
	void ClampCameraHeight();
	void CameraLockOn();

protected:
	//method to override to corpse disposal. 
	void AfterDeath() override;
	void OnDeath() override;
	void OnRespawn();

	
#pragma endregion
#pragma region Getters&Setters&InputHandlers

public:
	void OnMove(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnReleaseJump(const FInputActionValue& Value);
	void OnCameraMove(const FInputActionValue& Value);
	void OnSprint(const FInputActionValue& Value);
	void OnReleaseSprint(const FInputActionValue& Value);
	void OnCruch(const FInputActionValue& Value);
	void OnDash(const FInputActionValue& Value);

	void OnBlock(const FInputActionValue& Value);
	void OnReleaseBlock(const FInputActionValue& Value);
	void OnAttack_1(const FInputActionValue& Value);
	void OnReleaseAttack_1(const FInputActionValue& Value);
	void OnAttack_2(const FInputActionValue& Value);
	void OnReleaseAttack_2(const FInputActionValue& Value);
	void OnLockOnTarget(const FInputActionValue& Value);
	void OnShootProjectileSimple(const FInputActionValue& Value);

	/// getters and setters for special fields

	class UCameraComponent* GetCameraComponent() const
	{
		return cameraComponent;
	}
	class USpringArmComponent* GetSpringArmComponent() const
	{
		return SpringArmComp;
	}
	//for collisions, creates Querry for ignoring this character and childrens
	UFUNCTION(BLueprintCallable)
	void OnHitWithAttack(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, 
		UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bfromSweep, const FHitResult& Hit);

	UBaseStatsComponent* GetStatsComponent() const
	{
		return this->StatsComponent;
	}
#pragma endregion
};
