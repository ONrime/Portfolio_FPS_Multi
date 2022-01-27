// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "PlayerInfo_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API UPlayerInfo_SaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPlayerInfo_SaveGame();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
	FPlayerInfo S_playerInfo;

protected:

private:
};
