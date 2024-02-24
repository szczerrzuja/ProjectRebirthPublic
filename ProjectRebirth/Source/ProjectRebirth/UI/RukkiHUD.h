// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RukkiHUD.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PROJECTREBIRTH_API URukkiHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void SetHPLevel(float currHP, float maxHP);
	UFUNCTION(BlueprintCallable)
		void SetMPLevel(float currMP, float maxMP);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* HealthBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* ManaBar;
};
