// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerRespawnPoint.h"
#include "Components/SphereComponent.h"
#include "../Entities/Player/RebirthPlayerController.h"
#include "../Entities/Player/Rukki.h"
#include "../RebirthLevelScriptBase.h"

APlayerRespawnPoint::APlayerRespawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Colision Component"));
	SphereComponent->InitSphereRadius(40.0f);
	RootComponent = SphereComponent;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if (Mesh)
	{
		Mesh->AlwaysLoadOnClient = true;
		Mesh->AlwaysLoadOnServer = true;
		Mesh->bOwnerNoSee = false;
		Mesh->bCastDynamicShadow = true;
		Mesh->bAffectDynamicIndirectLighting = true;
		Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;

		static FName MeshCollisionProfileName(TEXT("CharacterMesh"));
		Mesh->SetCollisionProfileName(MeshCollisionProfileName);
		Mesh->SetGenerateOverlapEvents(false);
		Mesh->SetCanEverAffectNavigation(false);
		Mesh->SetupAttachment(SphereComponent);
	}

}

// Sets default values
APlayerRespawnPoint::APlayerRespawnPoint(const FObjectInitializer& ObjectInitalizer)
{
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Colision Component"));
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayerRespawnPoint::OnActorBeginOverlap);
	RootComponent = SphereComponent;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	if (Mesh)
	{
		Mesh->AlwaysLoadOnClient = true;
		Mesh->AlwaysLoadOnServer = true;
		Mesh->bOwnerNoSee = false;
		Mesh->bCastDynamicShadow = true;
		Mesh->bAffectDynamicIndirectLighting = true;
		Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		
		static FName MeshCollisionProfileName(TEXT("CharacterMesh"));
		Mesh->SetCollisionProfileName(MeshCollisionProfileName);
		Mesh->SetGenerateOverlapEvents(false);
		Mesh->SetCanEverAffectNavigation(false);
		Mesh->SetupAttachment(SphereComponent);
	}
	
}

// Called when the game starts or when spawned
void APlayerRespawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (ARebirthLevelScriptBase* levelScript = Cast<ARebirthLevelScriptBase>(GetLevel()->GetLevelScriptActor()))
	{
		levelScript->RegisterRespawnPointToLevelObject(this);
	}
}

// Called every frame
void APlayerRespawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
//TODO: AFTER AND BEFORE GRAPHICAL EFFECTS
void APlayerRespawnPoint::BeforeRespawn()
{

}

void APlayerRespawnPoint::AfterRespawn()
{

}

const FTransform& APlayerRespawnPoint::GetRespawnTransform() 
{
	SpawnTransform = GetActorTransform();
	SpawnTransform.SetLocation(GetActorLocation() + this->GetActorUpVector() * 100.0f);
	SpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
	return SpawnTransform;
}

void APlayerRespawnPoint::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bfromSweep, const FHitResult& SweepResoult)
{
	if (ARukki* playerObject = Cast<ARukki>(otherActor))
	{
		ARebirthPlayerController* playerController = Cast< ARebirthPlayerController>(playerObject->GetController());
		if (playerController)
		{
			playerController->SetSpawnPoint(this);
			UE_LOG(LogTemp, Warning, TEXT("SPawnpoint Changed:: %f : %f : %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
		}
	}
}

