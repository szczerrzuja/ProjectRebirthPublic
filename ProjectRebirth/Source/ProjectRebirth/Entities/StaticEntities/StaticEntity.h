// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../BaseStatsComponent.h"
#include "StaticEntity.generated.h"

UCLASS()
class PROJECTREBIRTH_API AStaticEntity : public AActor
{
	GENERATED_BODY()
		//shape componenet is used as handle for other types of colliders. It should be invisible
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OnHitEvent", meta = (AllowPrivateAccess = "true"))
		FHitEntityDamage damageOutput = FHitEntityDamage();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OnHitEvent", meta = (AllowPrivateAccess = "true"))
		float knockbackStrenght;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OnHitEvent", meta = (AllowPrivateAccess = "true"))
		FHitEntityEffect EffectOutput = FHitEntityEffect();
public:	
	// Sets default values for this actor's properties
	AStaticEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/*
	* Those methods are to learn how overlap and hit works
	* 
	*/
	UFUNCTION(BlueprintCallable)
		void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex,
			bool bfromSweep, const FHitResult& SweepResoult);
	UFUNCTION(BlueprintCallable)
		void OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex);
	UFUNCTION(BlueprintCallable)
		void OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
