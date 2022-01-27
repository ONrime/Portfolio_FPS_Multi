// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Projectile/Public/ProjectileBase.h"
#include "Rifle_ProjectileBase.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API ARifle_ProjectileBase : public AProjectileBase
{
	GENERATED_BODY()
	
	
public:
	ARifle_ProjectileBase();


protected:
	virtual void BeginPlay() override;

private:

public:
	virtual void OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


};
