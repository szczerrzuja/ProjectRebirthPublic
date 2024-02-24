// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerRespawnPoint.generated.h"
/*
* TODO: System to save point ID and map ID
*/
UCLASS()
class PROJECTREBIRTH_API APlayerRespawnPoint : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

public:	
	// Sets default values for this actor's properties
	APlayerRespawnPoint();
	APlayerRespawnPoint(const FObjectInitializer& ObjectInitalizer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FTransform SpawnTransform;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//if need restart level, otherwise dont
	void BeforeRespawn();
	//TOFO: Create animation on resapwn of character
	void AfterRespawn();
	UFUNCTION(BlueprintCallable)
	const FTransform& GetRespawnTransform();
	UFUNCTION(BlueprintCallable)
		void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex,
			bool bfromSweep, const FHitResult& SweepResoult);
};
