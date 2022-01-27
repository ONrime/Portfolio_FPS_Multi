// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TwoVersus_PlayerController.generated.h"

/** 플레이어 컨트롤러
 * 매치 UI와 관련된 처리를 주로 합니다.
 * 플레이어와 관련된 ui는 AMultiPlayer_HUD에서 처리합니다.
 */

DECLARE_DELEGATE(FTwoVersusControllerCheckDelegate)
DECLARE_DELEGATE_TwoParams(FTwoVersusControllerCheckTwoDelegate, int, int)

UCLASS()
class DALOS_API ATwoVersus_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATwoVersus_PlayerController();

	UPROPERTY()
	class ULobbyCount_UserWidget* Count_WB = nullptr;      // 시작 카운트 다운 위젯
	UPROPERTY()
	class UWinResult_UserWidget* WinCount_WB = nullptr;	   // 매치가 끝나고 승리 카운트를 보여주는 위젯
	UPROPERTY()
	class UMatchState_UserWidget* MatchState_WB = nullptr; // 매치 상태를 보여줍니다. (현재 매치 승리 카운트, 매치 시간)
	UPROPERTY()
	class UMatchHealth_UserWidget* MatchHP_WB = nullptr;   // 각 팀의 전체 체력을 보여줍니다.

	FTwoVersusControllerCheckDelegate CountDownStartCheck; // 시작 카운트 다운을 실행합니다. (5초 카운트)
	FTwoVersusControllerCheckDelegate PlayerDeadCheck;	   // 플레이어가 죽었는지를 서버의 GameMode에 보냅니다.
	FTwoVersusControllerCheckDelegate UpdateMatchHPCheck;  // 각 팀의 전체 체력을 업데이트합니다.

	// 각 클라이언트의 팀 정보를 가져와서 서버에 팀 정보를 알려줍니다.
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_GetTeamName(); // 클라이언트의 팀 정보를 가져오기 
	bool Client_GetTeamName_Validate();
	void Client_GetTeamName_Implementation();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_GetTeamName(const FString& Team); // 가져온 팀 정보를 서버에 알려주기
	bool Server_GetTeamName_Validate(const FString& Team);
	void Server_GetTeamName_Implementation(const FString& Team);

	// 매치가 끝났을 때 작동합니다.
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendWinResult(int Red, int Blue, const FString& WinTeam); // 매치가 끝나고 매치 결과를 알려줍니다.
	bool NetMulticast_SendWinResult_Validate(int Red, int Blue, const FString& WinTeam);
	void NetMulticast_SendWinResult_Implementation(int Red, int Blue, const FString& WinTeam);
	
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_EndWinResult(); // 매치결과를 가립니다. (매치가 다시 시작하거나 로비로 돌아갈때 작동합니다.)
	bool NetMulticast_EndWinResult_Validate();
	void NetMulticast_EndWinResult_Implementation();

	// 매치 카운트 다운
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_StartMatchCount(float EndTime); // 매치 카운트 다운 시작(5초 카운트 X)
	bool Client_StartMatchCount_Validate(float EndTime);
	void Client_StartMatchCount_Implementation(float EndTime);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndMatch();					    // 매치 카운트 다운이 다 끝나면 서버에 알립니다. (클라이언트에서 알릴 필요가 없다.)
	bool Server_EndMatch_Validate();
	void Server_EndMatch_Implementation();

	// 매치 시작
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_StartCountDown();   // 시작 카운트 다운 
	bool Client_StartCountDown_Validate();
	void Client_StartCountDown_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendGameModeCount(); // 클라이언트의 플레이어가 스폰이 다 되면 서버에게 알려줍니다.
	bool Server_SendGameModeCount_Validate();
	void Server_SendGameModeCount_Implementation();


	UPROPERTY(Replicated)
	int RedTeamWinCount = 0;  // 레드 팀 승리 횟수
	UPROPERTY(Replicated)
	int BlueTeamWinCount = 0; // 블루 팀 승리 횟수


protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UPROPERTY(Replicated)
	FString TeamName = ""; // 플레이어 팀 이름
	UPROPERTY(Replicated)
	FString PlayerStateName = ""; // 호스트 여부 확인
	UPROPERTY(Replicated)
	int MatchCount = 0;
	UPROPERTY(Replicated)
	int MatchTime = 0;
	TSubclassOf<class ULobbyCount_UserWidget> Count_Class;
	TSubclassOf<class UWinResult_UserWidget> WinCount_Class;
	TSubclassOf<class UMatchState_UserWidget> MatchCount_Class;
	TSubclassOf<class UMatchHealth_UserWidget> MatchHP_Class;


public:
	FString GetTeamName() { return TeamName; } void SetTeamName(FString Set) { TeamName = Set; }
	FString GetPlayerStateName() { return PlayerStateName; } void SetPlayerStateName(FString Set) { PlayerStateName = Set; }
	int GetMatchTime() { return MatchTime; } int GetMatchCount() { return MatchCount; }
	void SetMatchInfo(int Count, int Time);

private:

};
