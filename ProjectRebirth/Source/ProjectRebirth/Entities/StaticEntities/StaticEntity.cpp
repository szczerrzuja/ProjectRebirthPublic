// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticEntity.h"
#include "Components/ShapeComponent.h"
#include "../Player/Rukki.h"


// Sets default values
AStaticEntity::AStaticEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStaticEntity::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Kolizje powinny dzialac"));
}

// Called every frame
void AStaticEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*
* Those methods must be called in blueprint in order to make this shit dynamic. 
*/
void AStaticEntity::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent,
	int32 otherBodyIndex, bool bfromSweep, const FHitResult& SweepResoult)
{
	
	ARukki* HittedEntity = Cast<ARukki>(otherActor);
	if (HittedEntity)
	{
		HittedEntity->OnCollisionHostality(damageOutput, EffectOutput, knockbackStrenght, FVector(0.0f, 0.0f, 0.0f));
	}
}

void AStaticEntity::OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, 
	int32 otherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("JU¿ nie koliduje"));
}

void AStaticEntity::OnActorHit(UPrimitiveComponent* HitComp, AActor* otherActor, UPrimitiveComponent* OtherComp, 
	FVector NormalImpulse, const FHitResult& Hit)
{
	//TODO: Apply knockback on hit BUT effects and other shit on Overlap ???
	AEntityInterface* HittedEntity = Cast<AEntityInterface>(otherActor);
	if (HittedEntity)
	{
		HittedEntity->OnCollisionHostality(damageOutput, EffectOutput, knockbackStrenght, -Hit.ImpactNormal);
		
	}
}


