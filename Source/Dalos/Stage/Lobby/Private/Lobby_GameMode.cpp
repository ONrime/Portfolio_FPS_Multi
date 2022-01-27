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
	bUseSeamlessTravel = false; // ��ġ�� ���ο� ���� �ε��ϱ� ������ �Ų����� ���� �̵��� �մϴ�.
}

/* �α��� ���� �̺�Ʈ�� �ߵ��ϴ� �Լ��Դϴ�.
* �α��ε� ��Ʈ�ѷ����� ���� �� ���¸� �����մϴ�.
* GameState�� ������ ���� ������ �α��ε� ��Ʈ�ѷ��� �����մϴ�.
*/
/* �÷��̾ �κ�� ������(�α����� ��) �ʱ�ȭ�� �Ǵ� ����
* 1. GameMode(����)�� PostLogin���� ��, �� ����, ID�� ����
*
* 2. ������ ������ ��Ʈ�ѷ�(����)�� ���� (SetInitSetting() �Լ��� ���ؼ�)
*
* 3. SetInitSetting() �Լ����� ���� ���� ����(��, �� ����, ID)�� ��Ʈ�ѷ�(Ŭ���̾�Ʈ)���� �����մϴ�. (Client_LoginPlayer() �Լ��� ����)
*
* 4. ����(��, �� ����, ID)�� ���� ���� Ŭ���̾�Ʈ�� ����� ������(�̸�, ������)�� �ҷ��� (SaveGameCheck() �Լ��� ����)
*	 FPlayerInfo ����ü�� ���ļ� ��Ʈ�ѷ�(����)�� �����մϴ�.(Server_SendPlayerSetting() �Լ��� ����)
*
* 5. ��Ʈ�ѷ�(����)�� GameState�� �����ϰ� OnRep_AllPlayerInfoChange �Լ��� ���� ����(����), Ŭ���̾�Ʈ�� UI�� �ʱ�ȭ�� �մϴ�.
*/
void ALobby_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("PostLogin"));
	ALobby_GameState* State = Cast<ALobby_GameState>(GameState);
	UGameInfo_Instance* Instance = Cast<UGameInfo_Instance>(GetGameInstance());

	// GameState�� ��Ʈ�ѷ�, ���� �̸�, �Ѱ� �ο� �����մϴ�.
	State->AllPlayerController.Add(NewPlayer);
	State->ServerName = Instance->serverName.ToString();
	State->MaxPlayers = Instance->maxPlayer; // 4
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), gameState->allPlayerStart);

	// �� �й� ��Ű��
	FString TeamName = "";
	if (RedTeamCount <= BlueTeamCount)
	{ // ���� ��
		RedTeamCount++;
		TeamName = "Red";
		UE_LOG(LogTemp, Warning, TEXT("Red"));
	}
	else 
	{// ��� ��
		BlueTeamCount++;
		UE_LOG(LogTemp, Warning, TEXT("Blue"));
		TeamName = "Blue";
	}

	ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(NewPlayer);
	//if (Ctrl->IsLocalController()) Ctrl->SetupLobbyMenu_Implementation(gameState->serverName); // �� Ŭ���̾�Ʈ�� �κ� �޴� Ȱ��ȭ

	// �� ���¸� �����մϴ�. (�������� �մ����� ����)
	FString HostName = "Guest";
	if (!IsHost) {
		IsHost = true;
		HostName = "Host";

		// ���� ���� �ʱ�ȭ
		FGameSetting Setting; // ���� �ʱ�ȭ
		Setting.MapName = "TestMap";
		Setting.GameModeName = "2 VS 2";
		Setting.MatchCount = 3;
		Setting.MatchTime = 20;
		State->GameSetting = Setting;
	}// �ʱ�ȭ �� ���̺� ���� ���� Ȯ���ϰ� ������ ������Ʈ �ϱ�

	// ���� �÷��̾�� �⺻ ID�� �����ǰ� ID�� �����ؼ� �÷��̾ ã���ϴ�.
	// �׽�Ʈ���̱� ������ ������ ���θ�������ϴ�.
	GiveID++; // 0���� ���� ����
	Ctrl->SetInitSetting (TeamName, HostName, GiveID); // Ŭ���̾�Ʈ���� �ο��� ���� �� ���¸� �����մϴ�.

	//Ctrl->Client_LoginPlayer(TeamName, HostName, GiveID); // Ŭ���̾�Ʈ�� ����

	 // �� ��Ʈ��(Ŭ���̾�Ʈ)�� �÷��̾� �� ������Ʈ �� �÷��̾� ���� ����
	//if (!Ctrl->IsLocalController()) Ctrl->SetupLobbyMenu(gameState->serverName); // �� Ŭ���̾�Ʈ�� �κ� �޴� Ȱ��ȭ
	//Ctrl->UpdateLobbySettings(mapImage, mapName, mapTime); // �� Ŭ���̾�Ʈ�� �⺻ �� �̹���, �� �̸�, �� �ð� ���� �ؼ� ����

	UE_LOG(LogTemp, Warning, TEXT("PostLoginEnd"));

}

// ��ġ ���� �� �۵��Ǵ� ī��Ʈ �ٿ� ���߱�
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

// ��ġ ���� �� �۵��Ǵ� ī��Ʈ �ٿ� �����ϱ�
void ALobby_GameMode::StartCountDown()
{
	CountNum = 5;
	GetWorld()->GetTimerManager().SetTimer(CountDownTimer, this, &ALobby_GameMode::MinCount, 1.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("StartCountDown"));

	// �� �÷��̾��� Ÿ�̸�(UI)�� �۵��մϴ�.
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

// ī��Ʈ �ٿ� �Լ��Դϴ�.
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

		// �Ϻ� ���� ������ �����ϱ����� UGameInfo_Instance�� �����մϴ�.
		Ins->MatchCount= State->GameSetting.MatchCount;
		Ins->MatchTime= State->GameSetting.MatchTime;
		Ins->SetRedTeamWinCount(0);
		Ins->SetBlueTeamWinCount(0);
		UE_LOG(LogTemp, Warning, TEXT("LaunchTheGame: MatchCount %d"), Ins->MatchCount);

		// ��ġ�� �����մϴ�.
		GetWorld()->ServerTravel("/Game/Map/TestMap?Game=ATwoVersus_GameMode");
		//GetWorld()->ServerTravel("/Game/Map/TestMap?Game=ATwoVersus_GameMode");
		
	}
}
