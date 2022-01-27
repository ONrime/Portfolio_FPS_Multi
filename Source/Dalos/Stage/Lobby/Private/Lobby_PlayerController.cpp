// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Stage/Lobby/Public/Lobby_PlayerController.h"
#include "Dalos/Stage/Lobby/Public/Lobby_GameMode.h"
#include "Dalos/Stage/Lobby/Public/Lobby_GameState.h"
#include "Dalos/Widget/Lobby/Public/Lobby_Menu_UserWidget.h"
#include "Dalos/Game/Public/PlayerInfo_SaveGame.h"
#include "Dalos/Game/Public/GameInfo_Instance.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ALobby_PlayerController::ALobby_PlayerController()
{
	static ConstructorHelpers::FClassFinder<ULobby_Menu_UserWidget>LOBBYMENE_WIDGET_CLASS(TEXT("WidgetBlueprint'/Game/UI/Lobby/LobbyMenu.LobbyMenu_C'"));
	if (LOBBYMENE_WIDGET_CLASS.Succeeded()) {
		Lobby_Menu_Class = LOBBYMENE_WIDGET_CLASS.Class;
		UE_LOG(LogTemp, Warning, TEXT("Find"));
	}
	bReplicates = true;
}

void ALobby_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{ // ����(����)�϶� �۵� Ŭ���̾�Ʈ�� ���� �ǰ� ���� �۵�
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay: %s, %s"), *TeamState, *RoomState); // �׽�Ʈ ��

		ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		// �׽�Ʈ�� 
		ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(Ctrl));
		UE_LOG(LogTemp, Warning, TEXT("OnRep_UpdateClinetFirstPlayerList: %d"), State->AllPlayerInfo.Num());

		//if (!LobbyMenu_Widget) LobbyMenu_Widget = CreateWidget<ULobby_Menu_UserWidget>(Ctrl, Lobby_Menu_Class);
		SetInputMode(FInputModeUIOnly()); // UI�� ��Ʈ�ѷ��� �����ϰ� �մϴ�.
	}
}

void ALobby_PlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

/* �ʱ�ȭ �Լ��Դϴ�.
* BeginPlay�� ��� ���ϴ� ������ ������ ������ Ŭ���̾�Ʈ�� ���� �ʿ��� ������ �ް� ���� 
* �ʱ�ȭ�� �ϱ⸦ ���߱⿡ �ʱ�ȭ �Լ��� ���� ���� �����߽��ϴ�.
* �α����� �ǰ� ������ Ŭ���̾�Ʈ�� ���� �̸�, �������� �÷��̾� ������ ������ AllPlayerInfo�� �߰��մϴ�.
* �׶� AllPlayerInfo�߰��� �Ǹ鼭 �ٸ� Ŭ���̾�Ʈ���� �����Ͽ� ������ �����Ͱ� �� ���� �Ǿ��ٴ� ���� �˰Ե� Ŭ���̾�Ʈ��
* ��Ʈ�ѷ��� ���� UI�� �ʱ�ȭ �մϴ�.
*/
void ALobby_PlayerController::SetFirstPlayerList()
{
	UE_LOG(LogTemp, Warning, TEXT("SetFirstPlayerList"));
	if (!LobbyMenu_Widget && IsLocalController()) LobbyMenu_Widget = CreateWidget<ULobby_Menu_UserWidget>(this, Lobby_Menu_Class);
	if (LobbyMenu_Widget) 
	{
		ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
		LobbyMenu_Widget->FirstInPlayerList(State->AllPlayerInfo); // �ڱ� ȭ�鿡 ����Ʈ ����
		LobbyMenu_Widget->AddToViewport();
		LobbyMenu_Widget->InitWidget();
		SetInputMode(FInputModeUIOnly()); // UI�� ��Ʈ�ѷ��� �����ϰ� �մϴ�.
		SetShowMouseCursor(true);
		if (HasAuthority()) State->OnRep_GameSettingChange();
	}
}

// �÷��̾ �߰� �� �� ������ �ִ� �÷��̾���� ����Ʈ UI�� ������Ʈ �մϴ�.
void ALobby_PlayerController::PlayerListAdd(FPlayerInfo Setting)
{
	if (IsLocalController())
	{
		if (!LobbyMenu_Widget) LobbyMenu_Widget = CreateWidget<ULobby_Menu_UserWidget>(this, Lobby_Menu_Class);
		LobbyMenu_Widget->AddPlayerList(Setting);
	}
}

// �÷��̾ ���� �� ������ �ִ� �÷��̾���� ����Ʈ UI�� ������Ʈ �մϴ�.
void ALobby_PlayerController::DelPlayerInfo(FPlayerInfo Setting)
{
	if (IsLocalController())
	{
		//UE_LOG(LogTemp, Warning, TEXT("DelPlayerInfo"));
		LobbyMenu_Widget->DelPlayerList(Setting);
	}
}

// GameMode�� ���� ��, �� ����, ID�� �޾� Ŭ���̾�Ʈ�� �����մϴ�.
void ALobby_PlayerController::SetInitSetting(FString Team, FString Room, int FirstID) // �������� ����
{
	TeamState = Team;
	RoomState = Room;
	SettingID = FirstID;
	Client_LoginPlayer(TeamState, RoomState, SettingID); // Ŭ���̾�Ʈ�� ����
	//SaveGameCheck(TeamState, RoomState, FirstID); // �÷��̾ ������ �ִ� ���̺� �ҷ�����
}

// ��, �� ����, ID�� ���� Ŭ���̾�Ʈ�� �ڱ⿡�� ����� ������(�÷��̾� �̸�, ������)�� �ҷ��ɴϴ�. 
bool ALobby_PlayerController::Client_LoginPlayer_Validate(const FString& TeamName, const FString& State, int FirstID)
{
	return true;
}
void ALobby_PlayerController::Client_LoginPlayer_Implementation(const FString& TeamName, const FString& State, int FirstID)
{
	UE_LOG(LogTemp, Warning, TEXT("Client_LoginPlayer: %d"), FirstID);
	SaveGameCheck(TeamName, State, FirstID); // �÷��̾ ������ �ִ� ���̺� �ҷ�����
}

/* ������ ���� ���� ��, �� ����, ID�� �ڱ⿡�� ����� ������(�÷��̾� �̸�, ������)�� ��ģ
* FPlayerInfo ����ü�� �־ ������ �����մϴ�.
*/
void ALobby_PlayerController::SaveGameCheck(const FString& TeamName, const FString& State, int FirstID)
{
	UPlayerInfo_SaveGame* LoadGameInstance = Cast<UPlayerInfo_SaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerInfo_SaveGame::StaticClass()));
	LoadGameInstance = Cast<UPlayerInfo_SaveGame>(UGameplayStatics::LoadGameFromSlot("PlayerSettingsSave", 0));
	ALobby_GameState* States = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
	if (LoadGameInstance)
	{
		PlayerSetting = LoadGameInstance->S_playerInfo;
		PlayerSetting.playerTeamStatus = TeamName;
		PlayerSetting.playerStatus = State;
		PlayerSetting.settingID = FirstID;
		UE_LOG(LogTemp, Warning, TEXT("SaveGameCheck: %s"), *(PlayerSetting.playerName));
		Server_SendPlayerSetting(PlayerSetting); // ������ �÷��̾� ���� ����, ���� ������Ʈ�� ����(��ü)
	}
	// ���� ������ ���̺� �ϴ� ���?
}

/* ������ ��Ʈ�ѷ��� ���� ���� �÷��̾� ������ GameState�� ����(����, Ŭ���̾�Ʈ)
* ���� ���� GameState�� ����(���� ����)�� Ŭ���̾�Ʈ���� ������ �ʱ�ȭ �϶�� �˸��ϴ�.
*/
bool ALobby_PlayerController::Server_SendPlayerSetting_Validate(FPlayerInfo Setting)
{
	return true;
}
void ALobby_PlayerController::Server_SendPlayerSetting_Implementation(FPlayerInfo Setting)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_SendPlayerSetting"));
	PlayerSetting = Setting; // ������ ���� ����
	ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
	//State->AddPlayerInfoCount = State->AddPlayerInfoCount + 1;
	State->AllPlayerInfo.Add(Setting);
	State->OnRep_AllPlayerInfoChange(); // ����(���� ����)������ �����ϰ� �ϱ� ����

	//State->NetMultiCast_AddAllPlayerInfo(Setting); // ���� ������Ʈ�� ���̺� �߰�
}

/* �÷��̾� ������ �ٲ� �� �۵��ϴ� �Լ��Դϴ�.
* �غ� ����, �� ����, �� ���°� �ٲ� �� �۵��˴ϴ�.
*/
bool ALobby_PlayerController::Server_ChangeAllPlayerInfo_Validate(int Index, int Category, const FString& Change)
{
	return true;
}
void ALobby_PlayerController::Server_ChangeAllPlayerInfo_Implementation(int Index, int Category, const FString& Change)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_ChangeAllPlayerInfo: %s"), *Change);
	ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
	int Point = -1;
	if (Category == 0)
	{ // 0 �̸� Ready�� 1 �̸� ���� �ٲ۴�
		for (int i = 0; i < State->AllPlayerInfo.Num(); i++)
		{
			if (State->AllPlayerInfo[i].settingID == Index + 1)
			{
				Point = i;
				State->AllPlayerInfo[i].playerReadyStatus = Change;
				PlayerSetting.playerReadyStatus = Change;
				int ReadyCount = 0;
				ALobby_GameMode* GameMode = Cast<ALobby_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
				for (int j = 0; j < State->AllPlayerInfo.Num(); j++)
				{
					if (State->AllPlayerInfo[j].playerReadyStatus == "Ready") {
						ReadyCount++; // ��ü �غ� �ο��� ī��Ʈ �մϴ�.
					}
				}

				// ��ü �غ� �ο��� �˻��Ͽ� ��ġ ���� ī��Ʈ �ٿ��� �� �� ������ �����մϴ�.
				if (ReadyCount >= State->AllPlayerInfo.Num())
				{ // ī��Ʈ �ٿ�
					GameMode->StartCountDown();
				}
				else
				{ // ī��Ʈ �ٿ� ���߿� ���� �غ� Ǯ�� ī��Ʈ �ٿ��� �ʱ�ȭ �մϴ�.
					GameMode->StopCountDown();
				}
			}
		}
	}
	else if (Category == 1)
	{ // 1 �̸� ���� �ٲ۴�
		// ���� �ٲ�� �ٸ� �÷��̾��� ����Ʈ�� ����� ���� �����մϴ�.
		for (int i = 0; i < State->AllPlayerInfo.Num(); i++)
		{
			// ID�� ���� �÷��̾ �����Ͽ� �ٲ� �÷��̾ ã���ϴ�.
			if (State->AllPlayerInfo[i].settingID == Index + 1)
			{
				Point = i;
				State->AllPlayerInfo[i].playerTeamStatus = Change;
				PlayerSetting.playerTeamStatus = Change;
				TeamState = Change;
			}
		}
	}
	else
	{ // 2 �̸� �� ���¸� �ٲ۴�
		for (int i = 0; i < State->AllPlayerInfo.Num(); i++)
		{
			// ID�� ���� �÷��̾ �����Ͽ� �ٲ� �÷��̾ ã���ϴ�.
			if (State->AllPlayerInfo[i].settingID == Index + 1)
			{
				Point = i;
				State->AllPlayerInfo[i].playerStatus = Change;
				PlayerSetting.playerStatus = Change;
				RoomState = Change;
			}
		}
	}
	for (int i = 0; i < State->AllPlayerController.Num(); i++)
	{
		// �ٲ� ������ UI�� ������Ʈ �մϴ�.
		ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(State->AllPlayerController[i]);
		Ctrl->Client_ChangePlayerInfo(State->AllPlayerInfo[Point], Category, Change); // ui�� �ٲٱ� ����
	}
}

// �ٲ� ����(��, ��, �غ�)�� UI�� ������Ʈ �մϴ�.
bool ALobby_PlayerController::Client_ChangePlayerInfo_Validate(FPlayerInfo Setting, int Category, const FString& Change)
{
	return true;
}
void ALobby_PlayerController::Client_ChangePlayerInfo_Implementation(FPlayerInfo Setting, int Category, const FString& Change)
{
	if (LobbyMenu_Widget) {
		ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
		if (Category == 0)
		{ // �غ� ����
			LobbyMenu_Widget->ChangeReady(Setting.settingID, Change);
			UE_LOG(LogTemp, Warning, TEXT("Client_ChangePlayerInfo: %d"), Setting.settingID);
		}
		else if (Category == 1) 
		{ // �� ����
			if (Change == "Red") {
				LobbyMenu_Widget->ChangeTeam(Setting, false);
			}
			else {
				LobbyMenu_Widget->ChangeTeam(Setting, true);
			}
			UE_LOG(LogTemp, Warning, TEXT("Client_ChangePlayerInfo: %s"), *Setting.playerTeamStatus);
			UE_LOG(LogTemp, Warning, TEXT("Client_ChangePlayerInfo: %d"), Setting.settingID);
		}
		else 
		{ // �� ����

		}
	}
}

// ����(��ġ) ������ ����Ǹ� GameState�� �˸��ϴ�.
bool ALobby_PlayerController::Server_ChangeGameSetting_Validate(FGameSetting Setting)
{
	return true;
}
void ALobby_PlayerController::Server_ChangeGameSetting_Implementation(FGameSetting Setting)
{
	ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
	State->GameSetting = Setting;
}

// GameState�� ���� ����Ǹ� �ٲ� ����(��ġ) ������ UI�� ������Ʈ �մϴ�.
void ALobby_PlayerController::ChangeGameSetting()
{
	if (IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("ChangeGameSetting: Ctrl"));
		ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
		LobbyMenu_Widget->ChangeGameSetting(State->GameSetting);
	}
}

// �� ���¸� UI�� ������Ʈ �մϴ�.
void ALobby_PlayerController::OnRep_UpdateClinetFirstPlayerList()
{
	// Ŭ���̾�Ʈ
	if (IsLocalController())
	{
		if (RoomState == "Host" && LobbyMenu_Widget)
		{
			LobbyMenu_Widget->ChangeRoom();
		}
	}
}

// ������ ä�� ���� ����
bool ALobby_PlayerController::GetChatMessage_Validate(const FString& TextToSend)
{
	return true;
}
void ALobby_PlayerController::GetChatMessage_Implementation(const FString& TextToSend)
{
	UE_LOG(LogTemp, Warning, TEXT("GetChatMessage"));
	auto State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
	for (int i = 0; i < State->AllPlayerController.Num(); i++)
	{
		ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(State->AllPlayerController[i]);
		Ctrl->UpdateText(TextToSend, PlayerSetting.playerName);
	}
}

// ��� �÷��̾� ��Ʈ�ѷ����� ü�� ������Ʈ
bool ALobby_PlayerController::UpdateText_Validate(const FString& SenderText, const FString& SenderName)
{
	return true;
}
void ALobby_PlayerController::UpdateText_Implementation(const FString& SenderText, const FString& SenderName)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateText"));
	if (LobbyMenu_Widget) {
		LobbyMenu_Widget->UpdateChat(SenderText, SenderName);
	}
}

bool ALobby_PlayerController::StartCountDown_Validate()
{
	return true;
}
void ALobby_PlayerController::StartCountDown_Implementation()
{
	if (LobbyMenu_Widget) {
		UE_LOG(LogTemp, Warning, TEXT("StartCountDown: Ctrl"));
		LobbyMenu_Widget->StartCountDown();
		UGameInfo_Instance* Ins = Cast<UGameInfo_Instance>(GetGameInstance());
		ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
		Ins->PlayerTeamName = TeamState;
		Ins->MatchCount = State->GameSetting.MatchCount;
		Ins->MatchTime = State->GameSetting.MatchTime;
		Ins->PlayerStateName = RoomState;
		UE_LOG(LogTemp, Warning, TEXT("StartCountDown: Ctrl, %s"), *(Ins->PlayerTeamName));
	}
}
bool ALobby_PlayerController::StopCountDown_Validate()
{
	return true;
}
void ALobby_PlayerController::StopCountDown_Implementation()
{
	if (LobbyMenu_Widget) {
		LobbyMenu_Widget->StopCountDown();
	}
}

bool ALobby_PlayerController::Server_LeaveSession_Validate()
{
	return true;
}
void ALobby_PlayerController::Server_LeaveSession_Implementation()
{
	ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
	State->DelPlayer = PlayerSetting;
	State->AllPlayerInfo.Remove(PlayerSetting);
	State->AllPlayerController.Remove(this);
	State->OnRep_DelPlayerChange(); // ? ���������� �����ϰ� �ϱ� ����
	State->OnRep_AllPlayerInfoChange(); // ? ���������� �����ϰ� �ϱ� ����
	ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(State->AllPlayerController[0]);
	if (Ctrl)
	{
		Ctrl->Server_ChangeAllPlayerInfo(Ctrl->GetSettingID() - 1, 2, "Host");
	}
}


/*void ALobby_PlayerController::LoginPlayer(const FString& Team, const FString& State) // ����
{
	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("LoginPlayer"));
		TeamState = Team;
		RoomState = State;
		//SaveGameCheck(TeamState, RoomState); // �÷��̾ ������ �ִ� ���̺� �ҷ�����
	}
}*/




void ALobby_PlayerController::SetTeamState(FString Set)
{
	TeamState = Set;
}

void ALobby_PlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobby_PlayerController, TeamState);
	DOREPLIFETIME(ALobby_PlayerController, RoomState);
	DOREPLIFETIME(ALobby_PlayerController, SettingID);
	DOREPLIFETIME(ALobby_PlayerController, PlayerSetting);
}
