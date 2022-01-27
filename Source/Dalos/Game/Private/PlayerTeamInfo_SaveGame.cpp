// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Game/Public/PlayerTeamInfo_SaveGame.h"

UPlayerTeamInfo_SaveGame::UPlayerTeamInfo_SaveGame()
{
	S_playerTeamInfo.TeamName = "Red";
}

void UPlayerTeamInfo_SaveGame::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UGameInfo_Instance, Owner);
}