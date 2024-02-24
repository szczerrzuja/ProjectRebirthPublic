// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EntityAnimationBase.generated.h"

/**
 * This class was created becouse i needed some getters and setters accesable in CombatSwitchComponent. Also this class should be faster than animationBP
 * Wish me luck with this. 
 */
UCLASS(Transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class PROJECTREBIRTH_API UEntityAnimationBase : public UAnimInstance
{
	GENERATED_BODY()
public: 
	UEntityAnimationBase();
private: 
	class UPlayerMovementComponent* MovementComponenetReference;
protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
		class AEntityInterface* EntityReference;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Properties")
		float remTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
		float ForwardVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
		bool bIsJump;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
		uint8 MovementAnimationState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
		uint8 CombatAnimationID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Properties")
		bool bEndOfCombatAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Properties")
		bool bReplayAnimation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
		class UCombatSwitchComponent* CombatSwitchComponent;

	UFUNCTION(BlueprintCallable, Category = "Animations")
		virtual void UpdateAnimProperties();
	UFUNCTION(BlueprintCallable, Category = "Animations")
		virtual void ResetEndOFCombatAnim();

	bool GetEndOfCombatAAnimation() const { return this->bEndOfCombatAnimation; }
	float GetRemainingAnimationTime() const { return this->remTime; }

	
};
