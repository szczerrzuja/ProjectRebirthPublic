// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "WorldInteract/PlayerRespawnPoint.h"
#include "RebirthLevelScriptBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREBIRTH_API ARebirthLevelScriptBase : public ALevelScriptActor
{
	GENERATED_BODY()


private:
	//TODO: Sort spawnpoint by ID
	TArray<APlayerRespawnPoint*> RegisteredRespawnPoints;

public:
	void RegisterRespawnPointToLevelObject(APlayerRespawnPoint* playerSpawnPoint);

	APlayerRespawnPoint* GetSpawnPoint(int index) {
		if (index < RegisteredRespawnPoints.Num())
		{
			return RegisteredRespawnPoints[index];
		}
		else
			return nullptr;
	}
	APlayerRespawnPoint* GetDefaultPlayerRespawnPoint() {
		return RegisteredRespawnPoints[0];
	}
};
