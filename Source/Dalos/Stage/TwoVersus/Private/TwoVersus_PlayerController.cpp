// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerController.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_GameMode.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerState.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_GameState.h"
#include "Dalos/Character/Public/Player/MultiPlayerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Dalos/Widget/Public/LobbyCount_UserWidget.h"
#include "Dalos/Widget/Public/WinResult_UserWidget.h"
#include "Dalos/Widget/Public/MatchState_UserWidget.h"
#include "Dalos/Widget/Public/MatchHealth_UserWidget.h"
#include "Dalos/Widget/Public/MultiPlayer_HUD.h"
#include "Dalos/Game/Public/GameInfo_Instance.h"

ATwoVersus_PlayerController::ATwoVersus_PlayerController()
{
	static ConstructorHelpers::FClassFinder<ULobbyCount_UserWidget>COUNTDOWN_WIDGET(TEXT("WidgetBlueprint'/Game/UI/Levels/BeginPlayCount.BeginPlayCount_C'"));
	if (COUNTDOWN_WIDGET.Succeeded()) Count_Class = COUNTDOWN_WIDGET.Class;
	static ConstructorHelpers::FClassFinder<UWinResult_UserWidget>WINCOUNT_WIDGET(TEXT("WidgetBlueprint'/Game/UI/Levels/WinCount.WinCount_C'"));
	if (COUNTDOWN_WIDGET.Succeeded()) WinCount_Class = WINCOUNT_WIDGET.Class;
	static ConstructorHelpers::FClassFinder<UMatchState_UserWidget>MATCHSTATE_WIDGET(TEXT("WidgetBlueprint'/Game/UI/Player/MatchState.MatchState_C'"));
	if (MATCHSTATE_WIDGET.Succeeded()) MatchCount_Class = MATCHSTATE_WIDGET.Class;
	static ConstructorHelpers::FClassFinder<UMatchHealth_UserWidget>MATCHHP_WIDGET(TEXT("WidgetBlueprint'/Game/UI/Player/MatchHP.MatchHP_C'"));
	if (MATCHHP_WIDGET.Succeeded()) MatchHP_Class = MATCHHP_WIDGET.Class;
}

void ATwoVersus_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetShowMouseCursor(false);          // 마우스 커서 가리기
	SetInputMode(FInputModeGameOnly()); // 컨트롤러만 인식하게 하기

	if (IsLocalController()) 
	{
		Server_SendGameModeCount(); // 클라이언트의 플레이어가 스폰이 다 되면 서버의 GameMode에게 알려줍니다.
		//Client_GetTeamName();
		UE_LOG(LogTemp, Warning, TEXT("BeginePlay_C_C"));
		WinCount_WB = CreateWidget<UWinResult_UserWidget>(this, WinCount_Class);
		MatchState_WB = CreateWidget<UMatchState_UserWidget>(this, MatchCount_Class);
		if (MatchState_WB) {
			MatchState_WB->AddToViewport();
		}
	}
	
}

/* 딜리게이트를 바인딩 합니다. */
void ATwoVersus_PlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* 딜리게이트 설정 */
	// 시작 카운트 다운을 실행합니다. (5초 카운트)
	CountDownStartCheck.BindLambda([this]()->void 
	{
		Client_StartCountDown();
		UE_LOG(LogTemp, Warning, TEXT("CountDownStartCheck"));
	});

	// 플레이어가 죽었는지를 서버의 GameMode에 보냅니다.
	PlayerDeadCheck.BindLambda([this]()->void 
	{
		if (HasAuthority()) {
			ATwoVersus_GameMode* GameMode = Cast<ATwoVersus_GameMode>(UGameplayStatics::GetGameMode(this));
			GameMode->CountPlayerDead(TeamName);
		}
	});

	// 각 팀의 전체 체력을 업데이트합니다.
	UpdateMatchHPCheck.BindLambda([this]()-> void 
	{
		ATwoVersus_GameState* GameState = Cast<ATwoVersus_GameState>(UGameplayStatics::GetGameState(this));
		GameState->ChangeTeamHPCheck.Execute();
		MatchHP_WB->RedTeamHP = GameState->GetRedTeamHP();
		MatchHP_WB->BlueTeamHP = GameState->GetBlueTeamHP();
		UE_LOG(LogTemp, Warning, TEXT("UpdateMatchHPCheck"));
	});
}

// 클라이언트의 팀 정보를 가져오기 

bool ATwoVersus_PlayerController::Client_GetTeamName_Validate()
{
	return true;
}
void ATwoVersus_PlayerController::Client_GetTeamName_Implementation()
{

	UGameInfo_Instance* Ins = Cast<UGameInfo_Instance>(GetGameInstance());
	TeamName = Ins->PlayerTeamName;
	PlayerStateName = Ins->PlayerStateName;
	MatchCount = Ins->MatchCount;
	MatchTime = Ins->MatchTime;
	Server_GetTeamName(TeamName);
	UE_LOG(LogTemp, Warning, TEXT("Client_GetTeamName: %s"), *TeamName);
	UE_LOG(LogTemp, Warning, TEXT("Client_GetTeamName: %d"), Ins->MatchTime);
}

// 가져온 팀 정보를 서버에 알려주기
bool ATwoVersus_PlayerController::Server_GetTeamName_Validate(const FString& Team)
{
	return true;
}
void ATwoVersus_PlayerController::Server_GetTeamName_Implementation(const FString& Team)
{
	TeamName = Team;
	ATwoVersus_GameMode* GameMode = Cast<ATwoVersus_GameMode>(UGameplayStatics::GetGameMode(this));
	if (Team == "Red") 
	{
		GameMode->SetRedTeamCount(GameMode->GetRedTeamCount() + 1);
	}else{ GameMode->SetBlueTeamCount(GameMode->GetBlueTeamCount() + 1); }

	GameMode->SpawnToPlayer(this);

	UE_LOG(LogTemp, Warning, TEXT("Server_GetTeamName: %s"), *TeamName);
	UE_LOG(LogTemp, Warning, TEXT("Server_GetTeamName:RedTeamCount %d"), GameMode->GetRedTeamCount());
	UE_LOG(LogTemp, Warning, TEXT("Server_GetTeamName:BlueTeamCount %d"), GameMode->GetBlueTeamCount());
}

// 시작 카운트 다운 
bool ATwoVersus_PlayerController::Client_StartCountDown_Validate()
{
	return true;
}
void ATwoVersus_PlayerController::Client_StartCountDown_Implementation()
{
	if (IsLocalController() && !Count_WB)
	{
		Count_WB = CreateWidget<ULobbyCount_UserWidget>(this, Count_Class);
		Count_WB->CountDownEndCheck.BindLambda([this]()->void
		{
			AMultiPlayerBase* Player = Cast<AMultiPlayerBase>(GetPawn());
			Player->CountDownEndCheck.Execute();
			Count_WB->RemoveFromParent();
		});
		Count_WB->AddToViewport();
		Count_WB->PlayCountDown();
	}
}
bool ATwoVersus_PlayerController::Server_SendGameModeCount_Validate()
{
	return true;
}
void ATwoVersus_PlayerController::Server_SendGameModeCount_Implementation()
{
	ATwoVersus_GameMode* GameMode = Cast<ATwoVersus_GameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode) GameMode->CountBeginPlayer();
	//UE_LOG(LogTemp, Warning, TEXT("SendGameModeCount"));
}

// 매치가 끝나고 매치 결과를 알려줍니다.
bool ATwoVersus_PlayerController::NetMulticast_SendWinResult_Validate(int Red, int Blue, const FString& WinTeam)
{
	return true;
}
void ATwoVersus_PlayerController::NetMulticast_SendWinResult_Implementation(int Red, int Blue, const FString& WinTeam)
{
	if (IsLocalController() && WinCount_WB)
	{
		WinCount_WB->SetRedWin(Red);
		WinCount_WB->SetBlueWin(Blue);
		if (WinTeam != "")
		{
			WinCount_WB->ResultText = WinTeam;
			WinCount_WB->SetResultTextColor();
		}
		WinCount_WB->AddToViewport();
	}
}

// 매치결과를 가립니다. (매치가 다시 시작하거나 로비로 돌아갈때 작동합니다.)
bool ATwoVersus_PlayerController::NetMulticast_EndWinResult_Validate()
{
	return true;
}
void ATwoVersus_PlayerController::NetMulticast_EndWinResult_Implementation()
{
	if (IsLocalController()) {
		WinCount_WB->ResultTextVis = ESlateVisibility::Hidden;
		WinCount_WB->RemoveFromParent();
	}
}

// 매치 카운트 다운 시작(5초 카운트 X)
bool ATwoVersus_PlayerController::Client_StartMatchCount_Validate(float EndTime)
{
	return true;
}
void ATwoVersus_PlayerController::Client_StartMatchCount_Implementation(float EndTime)
{
	if (MatchState_WB)
	{
		MatchState_WB->Count = (float)MatchTime;
		MatchState_WB->StartTimer(EndTime);
		MatchState_WB->RedTeamWinCount = RedTeamWinCount;
		MatchState_WB->BlueTeamWinCount = BlueTeamWinCount;
		UE_LOG(LogTemp, Warning, TEXT("Client_StartMatchCount: %f"), MatchState_WB->Count);
		UE_LOG(LogTemp, Warning, TEXT("Client_StartMatchCount: %d"), MatchTime);
	}
	// 만들기 체력 ui
	MatchHP_WB = CreateWidget<UMatchHealth_UserWidget>(this, MatchHP_Class);
	if (MatchHP_WB)
	{
		MatchHP_WB->AddToViewport();
		ATwoVersus_GameState* GameState = Cast<ATwoVersus_GameState>(UGameplayStatics::GetGameState(this));
		GameState->ChangeTeamHPCheck.Execute();
		MatchHP_WB->RedTeamHP = GameState->GetRedTeamHP();
		MatchHP_WB->BlueTeamHP = GameState->GetBlueTeamHP();
	}
}

// 매치 카운트 다운이 다 끝나면 서버에 알립니다. (클라이언트에서 알릴 필요가 없다.)
bool ATwoVersus_PlayerController::Server_EndMatch_Validate()
{
	return true;
}
void ATwoVersus_PlayerController::Server_EndMatch_Implementation() 
{
	ATwoVersus_GameMode* GameMode = Cast<ATwoVersus_GameMode>(UGameplayStatics::GetGameMode(this));
	GameMode->CountWin();
}

void ATwoVersus_PlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATwoVersus_PlayerController, RedTeamWinCount);
	DOREPLIFETIME(ATwoVersus_PlayerController, BlueTeamWinCount);
	DOREPLIFETIME(ATwoVersus_PlayerController, TeamName);
	DOREPLIFETIME(ATwoVersus_PlayerController, PlayerStateName);
	DOREPLIFETIME(ATwoVersus_PlayerController, MatchCount);
	DOREPLIFETIME(ATwoVersus_PlayerController, MatchTime);
}

void ATwoVersus_PlayerController::SetMatchInfo(int Count, int Time)
{
	MatchCount = Count;
	MatchTime = Time;
}
