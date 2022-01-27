// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Weapone/Public/WeaponeBase.h"
#include "AK_Weapone.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API AAK_Weapone : public AWeaponeBase
{
	GENERATED_BODY()
	
public:
	AAK_Weapone();

protected:
	virtual void BeginPlay() override;

private:

public:
	virtual UClass* GetStaticClass() override;
	virtual AWeaponeBase* SpawnToHand(APawn* owner, FVector loc, FRotator rot) override;
	virtual void ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot) override;
	virtual float GetFireRecoilPitch() override;
	virtual float GetFireRecoilYaw() override;

	virtual void PlayFireMontage() override;
	virtual void StopFireMontage() override;
	virtual void PlayReloadMontage() override;
	virtual void StopReloadMontage() override;

};
