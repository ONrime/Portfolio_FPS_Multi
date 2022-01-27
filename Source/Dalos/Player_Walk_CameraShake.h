// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShake.h"
#include "Player_Walk_CameraShake.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API UPlayer_Walk_CameraShake : public UMatineeCameraShake
{
	GENERATED_BODY()
	
public:
	UPlayer_Walk_CameraShake();
};
