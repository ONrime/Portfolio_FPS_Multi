// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Projectile/Public/ProjectileBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Dalos/Widget/Public/MultiPlayer_HUD.h"
#include "Damage/Public/Bullet_DamageType.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//SetReplicates(true);
	//SetReplicateMovement(true);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESHBODY"));
	RootComponent = BodyMesh;
	BodyMesh->SetRelativeScale3D(FVector(2.0f, 0.025f, 0.025f));
	BodyMesh->SetCollisionProfileName("Projectile");
	BodyMesh->SetGenerateOverlapEvents(true);
	BodyMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnOverlapBegin_Body);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SPHERECOLLISION"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetSphereRadius(2.5f);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MOVEMENT"));
	Movement->SetUpdatedComponent(RootComponent);
	Movement->InitialSpeed = 1800.0f;
	Movement->MaxSpeed = 100000.0f;
	Movement->bRotationFollowsVelocity = false;
	Movement->bShouldBounce = false;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}
// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AProjectileBase::ProjectileFire(FVector FireDir, AActor* Onwer)
{
	UE_LOG(LogTemp, Warning, TEXT("ProjectileFire"));
	Movement->Velocity = FireDir * Movement->InitialSpeed;
}
void AProjectileBase::SetProjectileVelocity(float Velocity)
{
	Movement->InitialSpeed = Velocity;
}

void AProjectileBase::OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FVector decalLoc = SweepResult.Location;
	FRotator decalRot = FRotationMatrix::MakeFromX(SweepResult.Normal).Rotator();

	if (OtherActor != nullptr && OtherActor != GetOwner()) {
		if (OtherComp->GetCollisionProfileName() == "Wall") {
			auto bulletHole = UGameplayStatics::SpawnDecalAttached(bulletHoleDecal, FVector(25.0f, 25.0f, 25.0f), OtherComp, TEXT("BulletHole"), decalLoc, decalRot, EAttachLocation::KeepWorldPosition, 100.0f);
			bulletHole->SetLifeSpan(10.0f);
		}
		else if (OtherComp->GetCollisionProfileName() == "EBodyMesh") {
			//UGameplayStatics::ApplyDamage(OtherActor, projectileDamage, nullptr, GetOwner(), nullptr);
			if (OtherActor->HasAuthority()) {
				UGameplayStatics::ApplyPointDamage(OtherActor, projectileDamage, GetActorForwardVector(), SweepResult, nullptr, GetOwner(), UBullet_DamageType::StaticClass());
				UE_LOG(LogTemp, Warning, TEXT("Projectil: Player Hit"));
			}
			//UE_LOG(LogTemp, Warning, TEXT("bulletLoc2: %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
			
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Projectil: Hit"));
		}
		this->Destroy();
	}
}


