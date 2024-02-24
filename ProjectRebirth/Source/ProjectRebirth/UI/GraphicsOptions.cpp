// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphicsOptions.h"
#include "GameFramework/GameUserSettings.h"

int UGraphicsOptions::PreviousIndex(int index, int max)
{
	return (index-1) * (index-1 >= 0) + (max - 1) * (index-1 < 0);
	
}

int UGraphicsOptions::NextIndex(int index, int max)
{
	return (index+1)%max;
}

void UGraphicsOptions::InitValues()
{
	UGameUserSettings* userSettings = UGameUserSettings::GetGameUserSettings();
	userSettings->GetTextureQuality();

	WindowMode = userSettings->GetDefaultWindowMode();
	TextureQualityIndex = userSettings->GetTextureQuality();
	AntiAliasing = userSettings->GetAntiAliasingQuality()>0;
	ShadowQualityIndex = userSettings->GetShadowQuality();
	IlluminationQuality = userSettings->GetGlobalIlluminationQuality();
	ShaderQualityIndex = userSettings->GetShadingQuality();
	EffectQualityIndex = userSettings->GetVisualEffectQuality();



}

