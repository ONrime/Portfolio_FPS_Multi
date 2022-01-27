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
	{ // 서버(리슨)일때 작동 클라이언트는 복제 되고 나서 작동
		UE_LOG(LogTemp, Warning, TEXT("BeginPlay: %s, %s"), *TeamState, *RoomState); // 테스트 용

		ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		// 테스트용 
		ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(Ctrl));
		UE_LOG(LogTemp, Warning, TEXT("OnRep_UpdateClinetFirstPlayerList: %d"), State->AllPlayerInfo.Num());

		//if (!LobbyMenu_Widget) LobbyMenu_Widget = CreateWidget<ULobby_Menu_UserWidget>(Ctrl, Lobby_Menu_Class);
		SetInputMode(FInputModeUIOnly()); // UI만 컨트롤러에 동작하게 합니다.
	}
}

void ALobby_PlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

/* 초기화 함수입니다.
* BeginPlay를 사용 안하는 이유는 서버가 완전히 클라이언트로 부터 필요한 정보를 받고 나서 
* 초기화를 하기를 원했기에 초기화 함수를 따로 만들어서 실행했습니다.
* 로그인이 되고 서버가 클라이언트로 부터 이름, 사진등의 플레이어 정보를 받으면 AllPlayerInfo를 추가합니다.
* 그때 AllPlayerInfo추가가 되면서 다른 클라이언트에게 전달하여 서버에 데이터가 잘 전달 되었다는 것을 알게된 클라이언트가
* 컨트롤러를 통해 UI를 초기화 합니다.
*/
void ALobby_PlayerController::SetFirstPlayerList()
{
	UE_LOG(LogTemp, Warning, TEXT("SetFirstPlayerList"));
	if (!LobbyMenu_Widget && IsLocalController()) LobbyMenu_Widget = CreateWidget<ULobby_Menu_UserWidget>(this, Lobby_Menu_Class);
	if (LobbyMenu_Widget) 
	{
		ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
		LobbyMenu_Widget->FirstInPlayerList(State->AllPlayerInfo); // 자기 화면에 리스트 전달
		LobbyMenu_Widget->AddToViewport();
		LobbyMenu_Widget->InitWidget();
		SetInputMode(FInputModeUIOnly()); // UI만 컨트롤러에 동작하게 합니다.
		SetShowMouseCursor(true);
		if (HasAuthority()) State->OnRep_GameSettingChange();
	}
}

// 플레이어가 추가 될 때 기존에 있는 플레이어들의 리스트 UI를 업데이트 합니다.
void ALobby_PlayerController::PlayerListAdd(FPlayerInfo Setting)
{
	if (IsLocalController())
	{
		if (!LobbyMenu_Widget) LobbyMenu_Widget = CreateWidget<ULobby_Menu_UserWidget>(this, Lobby_Menu_Class);
		LobbyMenu_Widget->AddPlayerList(Setting);
	}
}

// 플레이어가 나갈 때 기존에 있는 플레이어들의 리스트 UI를 업데이트 합니다.
void ALobby_PlayerController::DelPlayerInfo(FPlayerInfo Setting)
{
	if (IsLocalController())
	{
		//UE_LOG(LogTemp, Warning, TEXT("DelPlayerInfo"));
		LobbyMenu_Widget->DelPlayerList(Setting);
	}
}

// GameMode로 부터 팀, 방 상태, ID를 받아 클라이언트에 전달합니다.
void ALobby_PlayerController::SetInitSetting(FString Team, FString Room, int FirstID) // 서버에서 동작
{
	TeamState = Team;
	RoomState = Room;
	SettingID = FirstID;
	Client_LoginPlayer(TeamState, RoomState, SettingID); // 클라이언트에 전달
	//SaveGameCheck(TeamState, RoomState, FirstID); // 플레이어가 가지고 있는 세이브 불러오기
}

// 팀, 방 상태, ID를 받은 클라이언트는 자기에게 저장된 데이터(플레이어 이름, 사진등)를 불러옵니다. 
bool ALobby_PlayerController::Client_LoginPlayer_Validate(const FString& TeamName, const FString& State, int FirstID)
{
	return true;
}
void ALobby_PlayerController::Client_LoginPlayer_Implementation(const FString& TeamName, const FString& State, int FirstID)
{
	UE_LOG(LogTemp, Warning, TEXT("Client_LoginPlayer: %d"), FirstID);
	SaveGameCheck(TeamName, State, FirstID); // 플레이어가 가지고 있는 세이브 불러오기
}

/* 서버로 부터 받은 팀, 방 상태, ID와 자기에게 저장된 데이터(플레이어 이름, 사진등)를 합친
* FPlayerInfo 구조체에 넣어서 서버에 전달합니다.
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
		Server_SendPlayerSetting(PlayerSetting); // 서버에 플레이어 정보 전달, 게임 스테이트에 전달(전체)
	}
	// 만약 없으면 세이브 하는 기능?
}

/* 서버의 컨트롤러는 전달 받은 플레이어 정보를 GameState에 전달(서버, 클라이언트)
* 전달 받은 GameState는 서버(리슨 서버)와 클라이언트에게 위젯을 초기화 하라고 알립니다.
*/
bool ALobby_PlayerController::Server_SendPlayerSetting_Validate(FPlayerInfo Setting)
{
	return true;
}
void ALobby_PlayerController::Server_SendPlayerSetting_Implementation(FPlayerInfo Setting)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_SendPlayerSetting"));
	PlayerSetting = Setting; // 서버에 설정 전달
	ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
	//State->AddPlayerInfoCount = State->AddPlayerInfoCount + 1;
	State->AllPlayerInfo.Add(Setting);
	State->OnRep_AllPlayerInfoChange(); // 서버(리슨 서버)에서도 반응하게 하기 위해

	//State->NetMultiCast_AddAllPlayerInfo(Setting); // 게임 스테이트에 세이브 추가
}

/* 플레이어 정보가 바뀔 때 작동하는 함수입니다.
* 준비 상태, 팀 상태, 방 상태가 바뀔 때 작동됩니다.
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
	{ // 0 이면 Ready를 1 이면 팀을 바꾼다
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
						ReadyCount++; // 전체 준비 인원을 카운트 합니다.
					}
				}

				// 전체 준비 인원을 검사하여 매치 시작 카운트 다운을 할 지 안할지 결정합니다.
				if (ReadyCount >= State->AllPlayerInfo.Num())
				{ // 카운트 다운
					GameMode->StartCountDown();
				}
				else
				{ // 카운트 다운 도중에 누가 준비를 풀면 카운트 다운을 초기화 합니다.
					GameMode->StopCountDown();
				}
			}
		}
	}
	else if (Category == 1)
	{ // 1 이면 팀을 바꾼다
		// 팀이 바뀌면 다른 플레이어의 리스트도 변경된 팀을 전달합니다.
		for (int i = 0; i < State->AllPlayerInfo.Num(); i++)
		{
			// ID를 통해 플레이어를 구분하여 바뀐 플레이어를 찾습니다.
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
	{ // 2 이면 방 상태를 바꾼다
		for (int i = 0; i < State->AllPlayerInfo.Num(); i++)
		{
			// ID를 통해 플레이어를 구분하여 바뀐 플레이어를 찾습니다.
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
		// 바꿔 정보를 UI에 업데이트 합니다.
		ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(State->AllPlayerController[i]);
		Ctrl->Client_ChangePlayerInfo(State->AllPlayerInfo[Point], Category, Change); // ui를 바꾸기 위해
	}
}

// 바뀐 정보(팀, 방, 준비)를 UI에 업데이트 합니다.
bool ALobby_PlayerController::Client_ChangePlayerInfo_Validate(FPlayerInfo Setting, int Category, const FString& Change)
{
	return true;
}
void ALobby_PlayerController::Client_ChangePlayerInfo_Implementation(FPlayerInfo Setting, int Category, const FString& Change)
{
	if (LobbyMenu_Widget) {
		ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
		if (Category == 0)
		{ // 준비 상태
			LobbyMenu_Widget->ChangeReady(Setting.settingID, Change);
			UE_LOG(LogTemp, Warning, TEXT("Client_ChangePlayerInfo: %d"), Setting.settingID);
		}
		else if (Category == 1) 
		{ // 팀 상태
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
		{ // 방 상태

		}
	}
}

// 게임(매치) 세팅이 변경되면 GameState에 알립니다.
bool ALobby_PlayerController::Server_ChangeGameSetting_Validate(FGameSetting Setting)
{
	return true;
}
void ALobby_PlayerController::Server_ChangeGameSetting_Implementation(FGameSetting Setting)
{
	ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
	State->GameSetting = Setting;
}

// GameState를 통해 실행되며 바뀐 게임(매치) 세팅을 UI에 업데이트 합니다.
void ALobby_PlayerController::ChangeGameSetting()
{
	if (IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("ChangeGameSetting: Ctrl"));
		ALobby_GameState* State = Cast<ALobby_GameState>(UGameplayStatics::GetGameState(this));
		LobbyMenu_Widget->ChangeGameSetting(State->GameSetting);
	}
}

// 방 상태를 UI에 업데이트 합니다.
void ALobby_PlayerController::OnRep_UpdateClinetFirstPlayerList()
{
	// 클라이언트
	if (IsLocalController())
	{
		if (RoomState == "Host" && LobbyMenu_Widget)
		{
			LobbyMenu_Widget->ChangeRoom();
		}
	}
}

// 서버에 채팅 내용 전달
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

// 모든 플레이어 컨트롤러에게 체팅 업데이트
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
	State->OnRep_DelPlayerChange(); // ? 서버에서도 반응하게 하기 위해
	State->OnRep_AllPlayerInfoChange(); // ? 서버에서도 반응하게 하기 위해
	ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(State->AllPlayerController[0]);
	if (Ctrl)
	{
		Ctrl->Server_ChangeAllPlayerInfo(Ctrl->GetSettingID() - 1, 2, "Host");
	}
}


/*void ALobby_PlayerController::LoginPlayer(const FString& Team, const FString& State) // 삭제
{
	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("LoginPlayer"));
		TeamState = Team;
		RoomState = State;
		//SaveGameCheck(TeamState, RoomState); // 플레이어가 가지고 있는 세이브 불러오기
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
