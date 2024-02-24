// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GenericPlatform/GenericWindow.h"
#include "GraphicsOptions.generated.h"


UENUM(BlueprintType)
enum EUIQualityLevel : uint8
{
	Low = 0 UMETA(DisplayName = "Low"),
	Medium  UMETA(DisplayName = "Medium"),
	High  UMETA(DisplayName = "Medium"),
	Epic  UMETA(DisplayName = "Medium"),
	Cinematic  UMETA(DisplayName = "Medium")
};
/**
 * 
 */
UCLASS()
class PROJECTREBIRTH_API UGraphicsOptions : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TEnumAsByte<EWindowMode::Type> WindowMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int TextureQualityIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int TextureFilterIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool AntiAliasing;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int ShadowQualityIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int IlluminationQuality;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int LevelOfDetailsIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int ShaderQualityIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int EffectQualityIndex;

		UFUNCTION(BlueprintCallable, BlueprintPure)
		int NextIndex(int index, int max);
		UFUNCTION(BlueprintCallable, BlueprintPure)
		int PreviousIndex(int index, int max);

		//initialize values after opening game
		UFUNCTION(BlueprintCallable)
		void InitValues();


};
