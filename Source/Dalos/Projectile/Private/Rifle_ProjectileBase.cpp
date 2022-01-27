// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Projectile/Public/Rifle_ProjectileBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

ARifle_ProjectileBase::ARifle_ProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh->SetRelativeScale3D(FVector(0.3f, 0.02f, 0.02f));
	//MeshBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>BULLET_MATERIAL(TEXT("Material'/Game/Player/Projectile/Pistol_Tracer_Mat.Pistol_Tracer_Mat'"));
	if (BULLET_MATERIAL.Succeeded()) {
		BodyMesh->SetMaterial(0.0f, BULLET_MATERIAL.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh>STATICMESH_BODY(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (STATICMESH_BODY.Succeeded()) {
		BodyMesh->SetStaticMesh(STATICMESH_BODY.Object);
	}
	Movement->InitialSpeed = 3000.0f;
	Movement->MaxSpeed = 100000.0f;
	Movement->bRotationFollowsVelocity = false;
	Movement->bShouldBounce = false;
	Movement->ProjectileGravityScale = 0.0f;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>BULLETHOLE_DECAL(TEXT("Material'/Game/Player/Projectile/Bullet_Hole_M.Bullet_Hole_M'"));
	if (BULLETHOLE_DECAL.Succeeded()) {
		bulletHoleDecal = BULLETHOLE_DECAL.Object;
	}

	projectileDamage = 12.0f;
}

void ARifle_ProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(1000.0f);
}

void ARifle_ProjectileBase::OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin_Body(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//UE_LOG(LogTemp, Warning, TEXT("ProjectileHitt"));
}
