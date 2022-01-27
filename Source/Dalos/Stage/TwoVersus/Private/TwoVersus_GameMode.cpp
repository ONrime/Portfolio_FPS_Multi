// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Stage/TwoVersus/Public/TwoVersus_GameMode.h"
#include "Dalos/Character/Public/Player/MultiPlayerBase.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerController.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_GameState.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerState.h"
#include "Dalos/Widget/Public/MultiPlayer_HUD.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerStart.h"
#include "Dalos/Game/Public/GameInfo_Instance.h"
#include "GameFramework/PlayerStart.h"

ATwoVersus_GameMode::ATwoVersus_GameMode()
{
	DefaultPawnClass = AMultiPlayerBase::StaticClass();
	PlayerControllerClass = ATwoVersus_PlayerController::StaticClass();
	GameStateClass = ATwoVersus_GameState::StaticClass();
	PlayerStateClass = ATwoVersus_PlayerState::StaticClass();
	HUDClass = AMultiPlayer_HUD::StaticClass();
	bUseSeamlessTravel = false;

	//bStartPlayersAsSpectators = 0; // 자동 스폰 막기
}

void ATwoVersus_GameMode::RestartPlayer(AController* NewPlayer)
{
	// 컨트롤러에 팀 등록
	ATwoVersus_PlayerController* Controller = Cast<ATwoVersus_PlayerController>(NewPlayer);
	ATwoVersus_GameState* State = Cast<ATwoVersus_GameState>(GameState);
	if (NewPlayer) {
		AllPlayerController.Add(Cast<ATwoVersus_PlayerController>(NewPlayer));
		UE_LOG(LogTemp, Warning, TEXT("AllPlayerController: %d"), AllPlayerController.Num());
	}
	UGameInfo_Instance* Ins = Cast<UGameInfo_Instance>(GetGameInstance());
	// 각 클라이언트의 플레이어 정보를 가져와 서버에 등록합니다.
	Controller->Client_GetTeamName(); // MatchCount, MatchTime 겹친다. 클라이언트에서 팀을 받아오고 서버로 보낸다.   // 살리기
	FString TeamName = Controller->GetTeamName();
	//FString TeamName = "";

	
	/*if (TeamName == "") {
		UE_LOG(LogTemp, Warning, TEXT("Team Empty"));
		if (RedTeamCount <= BlueTeamCount) {
			Controller->SetTeamName("Red");
			Controller->Server_GetTeamName("Red"); // 없어도 되는 부분?
			RedTeamCount++;
			UE_LOG(LogTemp, Warning, TEXT("Red"));
		}
		else {
			Controller->SetTeamName("Blue");
			Controller->Server_GetTeamName("Blue"); // 없어도 되는 부분?
			BlueTeamCount++;
			UE_LOG(LogTemp, Warning, TEXT("Blue"));
		}
	} // 지우기*/

	//Controller->SetTeamName("Red");
	// 매치 정보를 가져옵니다.
	Controller->SetMatchInfo(Ins->MatchCount, Ins->MatchTime); // ?
	
	UE_LOG(LogTemp, Warning, TEXT("RestartPlayer: MatchTime: %d"), Ins->MatchTime);
	/*if (RedTeamCount <= BlueTeamCount) {
		Controller->SetTeamName("Red");
		//RedTeamCount++;
		UE_LOG(LogTemp, Warning, TEXT("Red"));
	}
	else {
		Controller->SetTeamName("Blue");
		//BlueTeamCount++;
		UE_LOG(LogTemp, Warning, TEXT("Blue"));
	}*/
	Controller->RedTeamWinCount = Ins->GetRedTeamWinCount();///
	Controller->BlueTeamWinCount = Ins->GetBlueTeamWinCount();///
	//Ins->SetBlueTeamWinCount();
	//Controller->RedTeamWinCount = 0;///
	//Controller->BlueTeamWinCount = 0;///
	
	// 스타트 지점 등록
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATwoVersus_PlayerStart::StaticClass(), AllPlayerStart);
	State->AllPlayerStart = AllPlayerStart;
	State->AllPlayerController.Add(Controller);

	// 스폰하기 (리슨 서버)
	if (NewPlayer->IsLocalController()) SpawnToPlayer(Cast<ATwoVersus_PlayerController>(NewPlayer));
	SpawnToPlayer(Cast<ATwoVersus_PlayerController>(NewPlayer)); // 지우기
	//NewPlayer->Possess(SpawnDefaultPawnAtTransform(NewPlayer, AllPlayerStart[0]->GetTransform()));
	MatchEnd = false;
	UE_LOG(LogTemp, Warning, TEXT("RestartPlayer"));
}

// 이번 매치의 5초 카운트 다운을 시작합니다.
void ATwoVersus_GameMode::CountBeginPlayer()
{
	// 각 플레이어가 클라이언트의 월드에서 스폰이 되어 BeginPlay를 실행하면 (준비가 완료가 된 상태)
	// 이 함수를 실행하여 현재 시작한 플레이어를 카운트합니다.
	BeginPlayer++;
	UE_LOG(LogTemp, Warning, TEXT("BeginPlayer: %d"), BeginPlayer);

	// 모든 플레이어가 준비가 완료가 되면 5초 카운트 다운을 실행합니다.
	// 각 플레이어의 UI도 실행합니다.
	if (BeginPlayer == AllPlayerController.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("AllPlayerControllerC: %d"), AllPlayerController.Num());
		for (int i = 0; i < AllPlayerController.Num(); i++)
		{
			if(AllPlayerController[i]->CountDownStartCheck.IsBound())AllPlayerController[i]->CountDownStartCheck.Execute();
		}
		GetWorldTimerManager().SetTimer(StartCountTimer, this, &ATwoVersus_GameMode::StartCountEnd, 5.0f, true);
	}
}

/* 플레이어가 죽은 상태라는 것을 전달 받으면
* 팀 전체가 죽었는지를 확인하여 게임을 끝낼지 말지 결정하는 함수입니다.
*/ 
void ATwoVersus_GameMode::CountPlayerDead(FString Team)
{
	UE_LOG(LogTemp, Warning, TEXT("Match End WinEnd: %d"), WinEnd);
	//GetWorldTimerManager().ClearTimer(GameCountTimer);

	UGameInfo_Instance* Ins = Cast<UGameInfo_Instance>(GetGameInstance());
	FString EndWinTeam = "";

	if (Team == "Red")
	{ // 죽인 플레이어가 레드 팀이면
		RedTeamCount--;
		UE_LOG(LogTemp, Warning, TEXT("Match End RedTeamCount: %d"), RedTeamCount);
		if (RedTeamCount == 0)
		{
			// 매치 앤드
			Ins->SetBlueTeamWinCount(Ins->GetBlueTeamWinCount() + 1);
			//BlueTeamWinCount++;
			UE_LOG(LogTemp, Warning, TEXT("Match End Blue: %d"), Ins->GetBlueTeamWinCount());
			UE_LOG(LogTemp, Warning, TEXT("Match End Blue: %d"), Ins->GetRedTeamWinCount());
			if (WinEnd == Ins->GetBlueTeamWinCount())
			{
				EndWinTeam = "BLUE";
				GameEnd = true;
			}
			for (int i = 0; i < AllPlayerController.Num(); i++)
			{
				// 각 플레이어의 결과 화면을 켜라
				AllPlayerController[i]->NetMulticast_SendWinResult(Ins->GetRedTeamWinCount(), Ins->GetBlueTeamWinCount(), EndWinTeam);
			}
			GetWorld()->GetTimerManager().SetTimer(WinResultTimer, this, &ATwoVersus_GameMode::WinResultEnd, 4.0f, true);
		}
	}
	else 
	{ // 죽인 플레이어가 블루 팀이면
		BlueTeamCount--;
		UE_LOG(LogTemp, Warning, TEXT("Match End BlueTeamCount: %d"), BlueTeamCount);
		if (BlueTeamCount == 0) 
		{
			// 매치 앤드
			Ins->SetRedTeamWinCount(Ins->GetRedTeamWinCount() + 1);
			//RedTeamWinCount++;
			UE_LOG(LogTemp, Warning, TEXT("Match End Red: %d"), Ins->GetRedTeamWinCount());
			UE_LOG(LogTemp, Warning, TEXT("Match End Red: %d"), Ins->GetBlueTeamWinCount());
			if (WinEnd == Ins->GetRedTeamWinCount()) 
			{
				EndWinTeam = "RED";
				GameEnd = true;
			}
			for (int i = 0; i < AllPlayerController.Num(); i++)
			{
				// 각 플레이어의 결과 화면을 켜라
				AllPlayerController[i]->NetMulticast_SendWinResult(Ins->GetRedTeamWinCount(), Ins->GetBlueTeamWinCount(), EndWinTeam);
			}
			// 4초 후에 새로운 게임 실행
			GetWorld()->GetTimerManager().SetTimer(WinResultTimer, this, &ATwoVersus_GameMode::WinResultEnd, 4.0f, true);
		}
	}
	
}

void ATwoVersus_GameMode::CountWin() // 타이머가 끝난뒤 승패 결정
{
	UGameInfo_Instance* Ins = Cast<UGameInfo_Instance>(GetGameInstance());
	if (!MatchEnd) {
		MatchEnd = true;
		FString EndWinTeam = "";
		ATwoVersus_GameState* State = Cast<ATwoVersus_GameState>(GameState);
		if (State->GetRedTeamHP() > State->GetBlueTeamHP())
		{
			//RedTeamWinCount++;
			Ins->SetRedTeamWinCount(Ins->GetRedTeamWinCount() + 1);
			UE_LOG(LogTemp, Warning, TEXT("Match End Red: %d"), Ins->GetRedTeamWinCount());

			// 승리 횟수가 과반수를 넘으면 게임을 끝냅니다.
			if (WinEnd == Ins->GetRedTeamWinCount()) 
			{
				EndWinTeam = "RED";
				GameEnd = true;
			}
		}
		else if (State->GetRedTeamHP() < State->GetBlueTeamHP()) 
		{
			//BlueTeamWinCount++;
			Ins->SetBlueTeamWinCount(Ins->GetBlueTeamWinCount() + 1);
			UE_LOG(LogTemp, Warning, TEXT("Match End Blue: %d"), Ins->GetBlueTeamWinCount());

			// 승리 횟수가 과반수를 넘으면 게임을 끝냅니다.
			if (WinEnd == Ins->GetBlueTeamWinCount()) 
			{
				EndWinTeam = "BLUE";
				GameEnd = true;
			}
		}
		for (int i = 0; i < AllPlayerController.Num(); i++) 
		{
			AllPlayerController[i]->NetMulticast_SendWinResult(Ins->GetRedTeamWinCount(), Ins->GetBlueTeamWinCount(), EndWinTeam);
		}

		// 4초후에 게임을 끝냅니다.(GameEnd에따라 게임을 끝내고 로비로 돌아갈지 아니면 다른 매치를 시작할지가 결정됩니다.)
		GetWorld()->GetTimerManager().SetTimer(WinResultTimer, this, &ATwoVersus_GameMode::WinResultEnd, 4.0f, true);
	}
}

/* 전체의 플레이어 시작지점 중에서 가능한 곳에 스폰하고 컨트롤러를 부여하는 함수입니다.
* 클라이언트의 플레이어 컨트롤러가 스폰되어 시작 할 때 캐릭터가 스폰이 되도록 만들었습니다.
* 클라이언트의 정보를 토대로 플레이어가 스폰되어야 하기 때문에 클라이언트의 컨틀로러가 스폰되어 시작 하기 전 까지 캐릭터가 스폰되지 않게 했습니다.
*/
void ATwoVersus_GameMode::SpawnToPlayer(ATwoVersus_PlayerController* Ctrl)
{
	// 스폰하기
	for (int i = 0; i < AllPlayerStart.Num(); i++)
	{
		// 
		ATwoVersus_PlayerStart* PlayerStart = Cast<ATwoVersus_PlayerStart>(AllPlayerStart[i]);
		if (Ctrl->GetTeamName() == PlayerStart->TeamName && !(PlayerStart->IsUse))
		{
			AMultiPlayerBase* Player = Cast<AMultiPlayerBase>(SpawnDefaultPawnAtTransform(Ctrl, PlayerStart->GetTransform()));
			Player->TeamName = PlayerStart->TeamName;
			Player->OnRep_TeamNameChange(); // 서버의 플레이어에도 업데이트를 하기위해
			Ctrl->Possess(Player);
			break;
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("SpawnToPlayer"));
}

// GameEnd에따라 게임을 끝내고 로비로 돌아갈지 아니면 다른 매치를 시작할지가 결정됩니다.
void ATwoVersus_GameMode::WinResultEnd()
{
	for (int i = 0; i < AllPlayerController.Num(); i++) 
	{
		AllPlayerController[i]->NetMulticast_EndWinResult(); // 각 플레이어의 결과 화면을 끄기
	}

	if (GameEnd)
	{
		UE_LOG(LogTemp, Warning, TEXT("End"));
		GetWorld()->ServerTravel("/Game/Map/LobyMap?Game=ALoby_GameModeBase"); // 로비로 돌아가기
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ReStart"));
		RestartGame(); // 새로운 매치 시작 
	}
}

/* 시작 카운트 다운이 끝 날때의 행동을 지정합니다.
* 매치 카운트 다운을 시작합니다.
*/
void ATwoVersus_GameMode::StartCountEnd()
{
	ATwoVersus_GameState* State = Cast<ATwoVersus_GameState>(GameState);
	State->ChangeTeamHPCheck.Execute();
	UGameInfo_Instance* Ins = Cast<UGameInfo_Instance>(GetGameInstance());
	float TimeEnd = 0.0f;

	// 호스트의 매치 시간을 가져와서 실행
	for (int i = 0; i < AllPlayerController.Num(); i++)
	{
		if (AllPlayerController[i]->GetPlayerStateName() == "Host") 
		{
			TimeEnd = AllPlayerController[i]->GetMatchTime();
		}
	}
	if (TimeEnd == 0.0f) TimeEnd = 40.0f; // 0이면 40초로

	GetWorldTimerManager().SetTimer(GameCountTimer, this, &ATwoVersus_GameMode::GameCountEnd, TimeEnd, true);

	// 각 플레이어에게 매치 시간 전달 후 타이머 실행
	for (int i = 0; i < AllPlayerController.Num(); i++)
	{
		ATwoVersus_PlayerController* Ctrl = Cast<ATwoVersus_PlayerController>(AllPlayerController[i]);
		UE_LOG(LogTemp, Warning, TEXT("StartCountEnddd: %d"), Ins->MatchCount);
		WinEnd = FMath::CeilToInt((float)Ins->MatchCount / 2.0f);
		Ctrl->Client_StartMatchCount(Ins->MatchTime);
	}

	GetWorldTimerManager().ClearTimer(StartCountTimer);
}

void ATwoVersus_GameMode::GameCountEnd()
{
	//CountWin();
}
