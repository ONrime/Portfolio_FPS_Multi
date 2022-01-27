// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

UCLASS()
class DALOS_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	class UStaticMeshComponent* BodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	class USphereComponent* Sphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	class UProjectileMovementComponent* Movement;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY()
	UMaterialInterface* bulletHoleDecal = nullptr; //ÃÑ¾Ë º®¿¡ ³²´Â ÈçÀû
	float projectileDamage = 7.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ProjectileFire(FVector FireDir, AActor* Onwer);
	void SetProjectileVelocity(float Velocity);
	float GetProjectileDamage() { return projectileDamage; };

	UFUNCTION()
	virtual void OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
