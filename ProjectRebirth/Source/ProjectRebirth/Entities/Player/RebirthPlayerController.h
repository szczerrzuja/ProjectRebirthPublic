// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../../WorldInteract/PlayerRespawnPoint.h"
#include "RebirthPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTREBIRTH_API ARebirthPlayerController : public APlayerController
{
	GENERATED_BODY()

	ARebirthPlayerController();
protected: 
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#pragma region HUD

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class URukkiHUD> PlayerHudClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "HUD", AllowPrivateAccess = "true"))
		class URukkiHUD* PlayerHUD;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class ARukki* RukkiPtr;

#pragma endregion
#pragma region Respawn
	//TODO: respawn in different map TIP: In game Instance 
private:
	//it must be aactor or actorlike object to check if still exists and if can spawn on it 
	//TODO: replace with ID of spawnpoint or something otherwise spawnpoint can be "Unloaded" as part of world can be unloaded
	//or replace with Transform its hard to tell
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APlayerRespawnPoint* SpawnPoint;

public:
	void SetSpawnPoint(APlayerRespawnPoint* NewSpawnPoint) { SpawnPoint = NewSpawnPoint; }
	//this method dont work becouse someone 
	UFUNCTION(BlueprintNativeEvent, Category = RespawnMechanics)
	void Respawn();
	void Respawn_Implementation();
#pragma endregion

};
