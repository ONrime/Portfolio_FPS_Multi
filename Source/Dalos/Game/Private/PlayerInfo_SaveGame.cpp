// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Game/Public/PlayerInfo_SaveGame.h"

UPlayerInfo_SaveGame::UPlayerInfo_SaveGame()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D>BASE_PLAYERIMAGE(TEXT("Texture2D'/Engine/VREditor/Devices/Vive/UE4_Logo.UE4_Logo'"));
	if (BASE_PLAYERIMAGE.Succeeded()) S_playerInfo.playerImage = BASE_PLAYERIMAGE.Object;
	S_playerInfo.playerStatus = "NotReady";
	S_playerInfo.settingID = 0;
}
void UPlayerInfo_SaveGame::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UGameInfo_Instance, Owner);
}