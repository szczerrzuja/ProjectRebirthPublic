// Fill out your copyright notice in the Description page of Project Settings.


#include "RebirthPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "../../ProjectRebirthGameModeBase.h"
#include "../../UI/RukkiHUD.h"
#include "Rukki.h"
ARebirthPlayerController::ARebirthPlayerController()
{
}
void ARebirthPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
	PlayerHUD = CreateWidget<URukkiHUD>(Cast<APlayerController>(this), PlayerHudClass);
	PlayerHUD->AddToPlayerScreen();
	RukkiPtr = Cast<ARukki>(GetCharacter());
}

void ARebirthPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ARebirthPlayerController::Respawn_Implementation()
{
	AProjectRebirthGameModeBase* gameMode = Cast<AProjectRebirthGameModeBase>(GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		//TODO: Acknowledged pawn to spawnpoint aactor
		gameMode->RestartPlayerAtTransform(AcknowledgedPawn->GetController(), SpawnPoint->GetRespawnTransform());
		
	}
	
}
