// Fill out your copyright notice in the Description page of Project Settings.


#include "RukkiHUD.h"
#include "Components/ProgressBar.h"

void URukkiHUD::SetHPLevel(float currHP, float maxHP)
{
	if (HealthBar)
	{
		const float precentage = currHP / maxHP;
		HealthBar->SetPercent(precentage*(precentage>=0.0f));
	}
}

void URukkiHUD::SetMPLevel(float currMP, float maxMP)
{

	if (ManaBar)
	{
		const float precentage = currMP / maxMP;
		ManaBar->SetPercent(precentage * (precentage >= 0.0f));
	}
}
