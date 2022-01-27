// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "PlayerTeamInfo_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API UPlayerTeamInfo_SaveGame : public USaveGame
{
	GENERATED_BODY()
	
	public:
	UPlayerTeamInfo_SaveGame();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
	FPlayerTeamInfo S_playerTeamInfo;

protected:

private:
};
