// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileClass.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../EntityInterface.h"

AProjectileClass::AProjectileClass()
{
    PrimaryActorTick.bCanEverTick = true;
	ColiderSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Colider"));
    RootComponent = ColiderSphere;
	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Component"));
    ProjectileComponent->SetUpdatedComponent(ColiderSphere);
    ProjectileComponent->bRotationFollowsVelocity = true;
    ProjectileComponent->ProjectileGravityScale = 0.0f;


    ProjectileComponent->SetVelocityInLocalSpace(FVector(0, 0, 0));
    ProjectileComponent->MaxSpeed = 4000.0f;
    ProjectileComponent->Velocity = FVector(400.0f, 0, 0);
    ProjectileComponent->UpdateComponentVelocity();

}

void AProjectileClass::BeginPlay()
{
    Super::BeginPlay();
   
    
}

void AProjectileClass::InitializeProjectile(const FVector& InitVelocity, const FHitEntityDamage& Damage,
    const FHitEntityEffect& Effect, USceneComponent* HomingTarget)
{
     
    ProjectileComponent->MaxSpeed = InitVelocity.Length() * 1.1f;
    ProjectileComponent->InitialSpeed = InitVelocity.Length();
    ProjectileComponent->Velocity = InitVelocity;
;
    if (HomingTarget)
    {
        ProjectileComponent->bIsHomingProjectile = true;
        ProjectileComponent->HomingTargetComponent = HomingTarget;
        ProjectileComponent->HomingAccelerationMagnitude = InitVelocity.Length();

    }
    ProjectileComponent->UpdateComponentVelocity();
    HitDamage = Damage;
}

void AProjectileClass::InitializeProjectile(const FVector& InitVelocity, const FHitEntityDamage& Damage, USceneComponent* HomingTarget)
{
    
    ProjectileComponent->MaxSpeed = InitVelocity.Length() * 1.1f;
    ProjectileComponent->InitialSpeed = InitVelocity.Length();
    ProjectileComponent->Velocity = InitVelocity;

      
    if (HomingTarget)
    {
        ProjectileComponent->bIsHomingProjectile = true;
        ProjectileComponent->HomingTargetComponent = HomingTarget;
        ProjectileComponent->HomingAccelerationMagnitude = ProjectileComponent->MaxSpeed * targetingAccelerationMagnitude;

    }
    ProjectileComponent->UpdateComponentVelocity();
    HitDamage = Damage;
}

void AProjectileClass::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (ProjectileComponent->HomingTargetComponent != NULL)
    {
        ProjectileComponent->HomingAccelerationMagnitude = ProjectileComponent->MaxSpeed * targetingAccelerationMagnitude
            * (LinearAccelerationMultiplayer((ProjectileComponent->HomingTargetComponent->GetComponentLocation() - this->GetActorLocation()).Length())+1.0f);
    }
    
    ProjectileComponent->UpdateComponentVelocity();
}

void AProjectileClass::OnCollisionWithProjectile(UPrimitiveComponent* OverlappedComponent,
	AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bfromSweep, const FHitResult& Hit) 
{
    //TODO: cosider collisions with terrain
    AEntityInterface* hittedEntity = Cast<AEntityInterface>(otherActor);
    if (hittedEntity && GetOwner() != otherActor)
    {
        hittedEntity->OnCollisionHostalityDamage(HitDamage);
        hittedEntity->OnCollisionHostalityKnockback(40.0f, otherActor->GetActorLocation() - this->GetActorLocation() + FVector(0.0f, 0.0f, 0.1f));
        Destroy();
    }
    
}
float AProjectileClass::LinearAccelerationMultiplayer(float currentDistance) {
    return (-MaximumAccelerationMultiplayer * currentDistance / MaxDistance + MaximumAccelerationMultiplayer)
        * (MaxDistance >= currentDistance);
}