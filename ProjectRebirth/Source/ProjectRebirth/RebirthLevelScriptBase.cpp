// Fill out your copyright notice in the Description page of Project Settings.


#include "RebirthLevelScriptBase.h"

void ARebirthLevelScriptBase::RegisterRespawnPointToLevelObject(APlayerRespawnPoint* playerSpawnPoint)
{
	
	RegisteredRespawnPoints.AddUnique(playerSpawnPoint);
}


