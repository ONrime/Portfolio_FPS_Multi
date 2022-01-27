// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Widget/Lobby/Public/Lobby_Menu_UserWidget.h"
#include "Dalos/Stage/Lobby/Public/Lobby_PlayerController.h"
#include "Dalos/Stage/Lobby/Public/Lobby_GameState.h"
#include "Dalos/Widget/Public/LobbyCount_UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ULobby_Menu_UserWidget::InitWidget_Implementation()
{

}

void ULobby_Menu_UserWidget::PlayerClickReadyButton()
{
	UE_LOG(LogTemp, Warning, TEXT("Menu_PlayerClickReadyButton"));
	ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (ReadyState == "NotReady") {
		Ctrl->Server_ChangeAllPlayerInfo(Ctrl->GetSettingID() - 1, 0, "Ready");
		ReadyState = "Ready";
	}
	else {
		Ctrl->Server_ChangeAllPlayerInfo(Ctrl->GetSettingID() - 1, 0, "NotReady");
		ReadyState = "NotReady";
	}
	
}

void ULobby_Menu_UserWidget::PlayerClickTeamButton(bool RedCheck)
{
	ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	FString TeamName = "";
	if (RedCheck) { // Blue로 바꾸기
		TeamName = "Blue";
	}
	else { // Red로 바꾸기
		TeamName = "Red";
	}
	Ctrl->SetTeamState(TeamName);
	Ctrl->Server_ChangeAllPlayerInfo(Ctrl->GetSettingID() - 1, 1, TeamName);
	UE_LOG(LogTemp, Warning, TEXT("PlayerClickTeamButton: %s"), *TeamName);
}

void ULobby_Menu_UserWidget::FirstInPlayerList_Implementation(const TArray<FPlayerInfo>& Settings)
{
}
void ULobby_Menu_UserWidget::AddPlayerList_Implementation(FPlayerInfo Setting)
{
}
void ULobby_Menu_UserWidget::DelPlayerList_Implementation(FPlayerInfo Setting)
{
}
void ULobby_Menu_UserWidget::ChangeReady_Implementation(int FirstID, const FString& Change)
{
}
void ULobby_Menu_UserWidget::ChangeTeam_Implementation(FPlayerInfo Setting, bool RedCheck)
{
}
void ULobby_Menu_UserWidget::ChangeRoom_Implementation()
{
}
void ULobby_Menu_UserWidget::ChangeGameSetting_Implementation(FGameSetting Setting)
{
}
void ULobby_Menu_UserWidget::UpdateChat_Implementation(const FString& SenderText, const FString& SenderName)
{
}

void ULobby_Menu_UserWidget::StartCountDown()
{
	UE_LOG(LogTemp, Warning, TEXT("StartCountDown: Widget"));
	if (StateAndCountDown_Widget) {
		StateAndCountDown_Widget->PlayCountDown();
		UE_LOG(LogTemp, Warning, TEXT("StartCountDown: Widget2"));
	}
}
void ULobby_Menu_UserWidget::StopCountDown()
{
	if (StateAndCountDown_Widget) {
		StateAndCountDown_Widget->StopCountDown();
	}
}
