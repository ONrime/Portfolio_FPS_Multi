// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Widget/Public/LobbyMenu_UserWidget.h"
#include "Net/UnrealNetwork.h"

void ULobbyMenu_UserWidget::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULobbyMenu_UserWidget, serverName);
	DOREPLIFETIME(ULobbyMenu_UserWidget, mapImage);
	DOREPLIFETIME(ULobbyMenu_UserWidget, mapTime);
	DOREPLIFETIME(ULobbyMenu_UserWidget, mapName);
	DOREPLIFETIME(ULobbyMenu_UserWidget, playerDisplay);
}
