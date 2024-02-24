// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../EntityInterface.h"
#include "BaseHostileClass.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREBIRTH_API ABaseHostileClass : public AEntityInterface
{
	GENERATED_BODY()
	
	ABaseHostileClass(const FObjectInitializer& ObjectInitalizer);
	ABaseHostileClass();

	virtual void BeginPlay() override;
};
