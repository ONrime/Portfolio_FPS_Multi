// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Stage/Lobby/Public/Lobby_GameMode.h"
#include "Dalos/Stage/Lobby/Public/Lobby_GameState.h"
#include "Dalos/Stage/Lobby/Public/Lobby_PlayerController.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "Dalos/Game/Public/GameInfo_Instance.h"
#include "Dalos/DalosCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Dalos/Character/Public/Player/MultiPlayerBase.h"

ALobby_GameMode::ALobby_GameMode()
{
	DefaultPawnClass = ADalosCharacter::StaticClass();
	PlayerControllerClass = ALobby_PlayerController::StaticClass();
	GameStateClass = ALobby_GameState::StaticClass();
	bUseSeamlessTravel = false; // 매치의 새로운 맵을 로드하기 때문에 매끄럽지 않은 이동을 합니다.
}

/* 로그인 이후 이벤트를 발동하는 함수입니다.
* 로그인된 컨트롤러에게 팀과 방 상태를 전달합니다.
* GameState에 현재의 게임 세팅을 로그인된 컨트롤러를 전달합니다.
*/
/* 플레이어가 로비로 들어오면(로그인이 된) 초기화가 되는 과정
* 1. GameMode(서버)의 PostLogin에서 팀, 방 상태, ID를 설정
*
* 2. 설정된 정보를 컨트롤러(서버)에 전달 (SetInitSetting() 함수를 통해서)
*
* 3. SetInitSetting() 함수에서 전달 받은 정보(팀, 방 상태, ID)를 컨트롤러(클라이언트)에게 전달합니다. (Client_LoginPlayer() 함수를 통해)
*
* 4. 정보(팀, 방 상태, ID)를 전달 받은 클라이언트는 저장된 데이터(이름, 사진등)를 불러와 (SaveGameCheck() 함수를 통해)
*	 FPlayerInfo 구조체로 합쳐서 컨트롤러(서버)에 전달합니다.(Server_SendPlayerSetting() 함수를 통해)
*
* 5. 컨트롤러(서버)는 GameState에 전달하고 OnRep_AllPlayerInfoChange 함수를 통해 서버(리슨), 클라이언트의 UI를 초기화를 합니다.
*/
void ALobby_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("PostLogin"));
	ALobby_GameState* State = Cast<ALobby_GameState>(GameState);
	UGameInfo_Instance* Instance = Cast<UGameInfo_Instance>(GetGameInstance());

	// GameState에 컨트롤러, 서버 이름, 한계 인원 전달합니다.
	State->AllPlayerController.Add(NewPlayer);
	State->ServerName = Instance->serverName.ToString();
	State->MaxPlayers = Instance->maxPlayer; // 4
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), gameState->allPlayerStart);

	// 팀 분배 시키기
	FString TeamName = "";
	if (RedTeamCount <= BlueTeamCount)
	{ // 레드 팀
		RedTeamCount++;
		TeamName = "Red";
		UE_LOG(LogTemp, Warning, TEXT("Red"));
	}
	else 
	{// 블루 팀
		BlueTeamCount++;
		UE_LOG(LogTemp, Warning, TEXT("Blue"));
		TeamName = "Blue";
	}

	ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(NewPlayer);
	//if (Ctrl->IsLocalController()) Ctrl->SetupLobbyMenu_Implementation(gameState->serverName); // 각 클라이언트의 로비 메뉴 활성화

	// 방 상태를 설정합니다. (방장인지 손님인지 구분)
	FString HostName = "Guest";
	if (!IsHost) {
		IsHost = true;
		HostName = "Host";

		// 게임 세팅 초기화
		FGameSetting Setting; // 세팅 초기화
		Setting.MapName = "TestMap";
		Setting.GameModeName = "2 VS 2";
		Setting.MatchCount = 3;
		Setting.MatchTime = 20;
		State->GameSetting = Setting;
	}// 초기화 및 세이브 게임 정보 확인하고 서버에 업데이트 하기

	// 들어온 플레이어에게 기본 ID가 증정되고 ID로 구분해서 플레이어를 찾습니다.
	// 테스트용이기 때문에 간단한 수로만들었습니다.
	GiveID++; // 0에서 부터 증가
	Ctrl->SetInitSetting (TeamName, HostName, GiveID); // 클라이언트에게 부여된 팀과 방 상태를 전달합니다.

	//Ctrl->Client_LoginPlayer(TeamName, HostName, GiveID); // 클라이언트에 전달

	 // 각 컨트롤(클라이언트)에 플레이어 수 업데이트 및 플레이어 인포 전달
	//if (!Ctrl->IsLocalController()) Ctrl->SetupLobbyMenu(gameState->serverName); // 각 클라이언트의 로비 메뉴 활성화
	//Ctrl->UpdateLobbySettings(mapImage, mapName, mapTime); // 각 클라이언트의 기본 맵 이미지, 맵 이름, 맵 시간 전달 해서 설정

	UE_LOG(LogTemp, Warning, TEXT("PostLoginEnd"));

}

// 매치 시작 전 작동되는 카운트 다운 멈추기
void ALobby_GameMode::StopCountDown()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(CountDownTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(CountDownTimer);
		ALobby_GameState* State = Cast<ALobby_GameState>(GameState);
		if (State) {
			for (int i = 0; i < State->AllPlayerController.Num(); i++) 
			{
				ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(State->AllPlayerController[i]);
				if (Ctrl) Ctrl->StopCountDown();
			}
		}
	}
}

// 매치 시작 전 작동되는 카운트 다운 시작하기
void ALobby_GameMode::StartCountDown()
{
	CountNum = 5;
	GetWorld()->GetTimerManager().SetTimer(CountDownTimer, this, &ALobby_GameMode::MinCount, 1.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("StartCountDown"));

	// 각 플레이어의 타이머(UI)를 작동합니다.
	ALobby_GameState* State = Cast<ALobby_GameState>(GameState);
	if (State)
	{
		for (int i = 0; i < State->AllPlayerController.Num(); i++) 
		{
			ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(State->AllPlayerController[i]);
			if (Ctrl) Ctrl->StartCountDown();
		}
	}
}

// 카운트 다운 함수입니다.
void ALobby_GameMode::MinCount()
{
	CountNum -= 1;
	if (CountNum == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountDownTimer);
		ALobby_GameState* State = Cast<ALobby_GameState>(GameState);
		UE_LOG(LogTemp, Warning, TEXT("LaunchTheGame: %d"), State->AllPlayerController.Num());

		UGameInfo_Instance* Ins = Cast<UGameInfo_Instance>(GetGameInstance());
		ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(State->AllPlayerController[0]);

		// 일부 게임 설정을 저장하기위해 UGameInfo_Instance에 전달합니다.
		Ins->MatchCount= State->GameSetting.MatchCount;
		Ins->MatchTime= State->GameSetting.MatchTime;
		Ins->SetRedTeamWinCount(0);
		Ins->SetBlueTeamWinCount(0);
		UE_LOG(LogTemp, Warning, TEXT("LaunchTheGame: MatchCount %d"), Ins->MatchCount);

		// 매치를 시작합니다.
		GetWorld()->ServerTravel("/Game/Map/TestMap?Game=ATwoVersus_GameMode");
		//GetWorld()->ServerTravel("/Game/Map/TestMap?Game=ATwoVersus_GameMode");
		
	}
}
