// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TwoVersus_PlayerController.generated.h"

/** �÷��̾� ��Ʈ�ѷ�
 * ��ġ UI�� ���õ� ó���� �ַ� �մϴ�.
 * �÷��̾�� ���õ� ui�� AMultiPlayer_HUD���� ó���մϴ�.
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
	class ULobbyCount_UserWidget* Count_WB = nullptr;      // ���� ī��Ʈ �ٿ� ����
	UPROPERTY()
	class UWinResult_UserWidget* WinCount_WB = nullptr;	   // ��ġ�� ������ �¸� ī��Ʈ�� �����ִ� ����
	UPROPERTY()
	class UMatchState_UserWidget* MatchState_WB = nullptr; // ��ġ ���¸� �����ݴϴ�. (���� ��ġ �¸� ī��Ʈ, ��ġ �ð�)
	UPROPERTY()
	class UMatchHealth_UserWidget* MatchHP_WB = nullptr;   // �� ���� ��ü ü���� �����ݴϴ�.

	FTwoVersusControllerCheckDelegate CountDownStartCheck; // ���� ī��Ʈ �ٿ��� �����մϴ�. (5�� ī��Ʈ)
	FTwoVersusControllerCheckDelegate PlayerDeadCheck;	   // �÷��̾ �׾������� ������ GameMode�� �����ϴ�.
	FTwoVersusControllerCheckDelegate UpdateMatchHPCheck;  // �� ���� ��ü ü���� ������Ʈ�մϴ�.

	// �� Ŭ���̾�Ʈ�� �� ������ �����ͼ� ������ �� ������ �˷��ݴϴ�.
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_GetTeamName(); // Ŭ���̾�Ʈ�� �� ������ �������� 
	bool Client_GetTeamName_Validate();
	void Client_GetTeamName_Implementation();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_GetTeamName(const FString& Team); // ������ �� ������ ������ �˷��ֱ�
	bool Server_GetTeamName_Validate(const FString& Team);
	void Server_GetTeamName_Implementation(const FString& Team);

	// ��ġ�� ������ �� �۵��մϴ�.
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendWinResult(int Red, int Blue, const FString& WinTeam); // ��ġ�� ������ ��ġ ����� �˷��ݴϴ�.
	bool NetMulticast_SendWinResult_Validate(int Red, int Blue, const FString& WinTeam);
	void NetMulticast_SendWinResult_Implementation(int Red, int Blue, const FString& WinTeam);
	
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_EndWinResult(); // ��ġ����� �����ϴ�. (��ġ�� �ٽ� �����ϰų� �κ�� ���ư��� �۵��մϴ�.)
	bool NetMulticast_EndWinResult_Validate();
	void NetMulticast_EndWinResult_Implementation();

	// ��ġ ī��Ʈ �ٿ�
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_StartMatchCount(float EndTime); // ��ġ ī��Ʈ �ٿ� ����(5�� ī��Ʈ X)
	bool Client_StartMatchCount_Validate(float EndTime);
	void Client_StartMatchCount_Implementation(float EndTime);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndMatch();					    // ��ġ ī��Ʈ �ٿ��� �� ������ ������ �˸��ϴ�. (Ŭ���̾�Ʈ���� �˸� �ʿ䰡 ����.)
	bool Server_EndMatch_Validate();
	void Server_EndMatch_Implementation();

	// ��ġ ����
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_StartCountDown();   // ���� ī��Ʈ �ٿ� 
	bool Client_StartCountDown_Validate();
	void Client_StartCountDown_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendGameModeCount(); // Ŭ���̾�Ʈ�� �÷��̾ ������ �� �Ǹ� �������� �˷��ݴϴ�.
	bool Server_SendGameModeCount_Validate();
	void Server_SendGameModeCount_Implementation();


	UPROPERTY(Replicated)
	int RedTeamWinCount = 0;  // ���� �� �¸� Ƚ��
	UPROPERTY(Replicated)
	int BlueTeamWinCount = 0; // ��� �� �¸� Ƚ��


protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UPROPERTY(Replicated)
	FString TeamName = ""; // �÷��̾� �� �̸�
	UPROPERTY(Replicated)
	FString PlayerStateName = ""; // ȣ��Ʈ ���� Ȯ��
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
