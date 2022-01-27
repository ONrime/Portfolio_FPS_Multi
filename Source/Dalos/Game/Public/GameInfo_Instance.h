// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FindSessionsCallbackProxy.h"
#include "GameInfo_Instance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class GAMEMODETYPE : uint8 {
	TWOVERSUS,
	TEAMDEATHMACH
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSessionDelegate);

UCLASS()
class DALOS_API UGameInfo_Instance : public UGameInstance
{
	GENERATED_BODY()
	

public:
	UGameInfo_Instance();

protected:
	IOnlineSessionPtr sessionInterface;
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;

	virtual void Init() override;
	virtual void OnCreateSessionComplete(FName server_Name, bool succeeded);
	virtual void OnCreateSessionComplete_Lobby(FName serverName, bool succeeded);
	virtual void OnUpdateSessionComplete(FName ServerName, bool Succeeded);
	virtual void OnJoinSessionComplete(FName server_Name, EOnJoinSessionCompleteResult::Type type);
	virtual void OnFindSessionsComplete(bool succeeded);
	virtual void OnDestroySessionComplete(FName ServerName, bool succeeded);
	//virtual void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	virtual void HandleNetworkError(ENetworkFailure::Type FailureType, bool IsServer); // ��Ʈ��ũ ���� ó�� �� �޼��� ���
	virtual void HandleTravelError(ETravelFailure::Type FailureType); // �̵��� ���� ó�� �� �޼��� ���

	UFUNCTION(BlueprintCallable)
	void CreateServer(int32 PlayerNum, FName ServerName, FName GameModeName, bool LanCheck);

public:
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FSessionDelegate findSessionFaild;
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FSessionDelegate findSessionSucceeded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu", meta = (AllowPrivateAccess = "true"))
	class UUserWidget* MainMenu_WB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu", meta = (AllowPrivateAccess = "true"))
	class UUserWidget* HostMenu_WB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu", meta = (AllowPrivateAccess = "true"))
	class UFindServer_UserWidget* ServerMenu_WB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu", meta = (AllowPrivateAccess = "true"))
	class UOptionMenu_UserWidget* OptionMenu_WB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu", meta = (AllowPrivateAccess = "true"))
	class UUserWidget* LodingScreen_WB;

	TSubclassOf<class UUserWidget> MainMenu_Class;
	TSubclassOf<class UUserWidget> HostMenu_Class;
	TSubclassOf<class UUserWidget> ServerMenu_Class;
	TSubclassOf<class UUserWidget> OptionMenu_Class;
	TSubclassOf<class UUserWidget> LodingScreen_Class;

	UFUNCTION(BlueprintCallable, Category = "Event")
	void ShowMainMenu();  // ���� �޴� ȭ�� ��ȯ and ���콺 Ȱ��ȭ
	UFUNCTION(BlueprintCallable, Category = "Event")
	void ShowHostMenu();  // ȣ��Ʈ �޴� ȭ�� ��ȯ and ���콺 Ȱ��ȭ
	UFUNCTION(BlueprintCallable, Category = "Event")
	void ShowServerMenu();  // ���� �޴� ȭ�� ��ȯ and ���콺 Ȱ��ȭ
	UFUNCTION(BlueprintCallable, Category = "Event")
	void ShowOptionMenu();  // �ɼ� �޴� ȭ�� ��ȯ and ���콺 Ȱ��ȭ
	UFUNCTION(BlueprintCallable, Category = "Event")
	void LaunchLobby(int32 PlayerNum, FName ServerName, FName GameModeName, bool LanCheck, bool IsDedicated);  // �κ� �����ϰ� ȣ��Ʈ�� �����Ѵ�.
	UFUNCTION(BlueprintCallable, Category = "Event")
	void JoinServer(bool DedicatedCheck);  // ������ ����
	UFUNCTION(BlueprintCallable, Category = "Event")
	void FindServer(bool LanCheck, bool DedicatedCheck);  // ���� ã��
	UFUNCTION(BlueprintCallable, Category = "Event")
	void ShowLodingScreen(); // �䱸�� �ε� ȭ�� ���
		//UFUNCTION(BlueprintCallable, Category = "Event")
	void Destroy_SessionCaller(class APlayerController PC); // ȣ�� �� ���� �ı�
	UFUNCTION(BlueprintCallable, Category = "Event")
	void Check_SaveGame();  // ���� ���� Ȯ�� �ϱ�
	UFUNCTION(BlueprintCallable, Category = "Event")
	void ChoiseJoinServer(int num);
	UFUNCTION(BlueprintCallable, Category = "Event")
	void DestroySessionAndLeaveGame();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ServerSetting", meta = (AllowPrivateAccess = "true"))
	int32 maxPlayer = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ServerSetting", meta = (AllowPrivateAccess = "true"))
	FName serverName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ServerSetting", meta = (AllowPrivateAccess = "true"))
	GAMEMODETYPE GameMode = GAMEMODETYPE::TWOVERSUS;

	TArray<FOnlineSessionSearchResult> Results;

	FString PlayerTeamName = "Blue";
	FString PlayerStateName = "";
	int MatchCount = 3;
	int MatchTime = 120; // �� �ٲٱ�

	int GetRedTeamWinCount() { return RedTeamWinCount; } int GetBlueTeamWinCount() { return BlueTeamWinCount; }
	void SetRedTeamWinCount(int Set) { RedTeamWinCount = Set; } void SetBlueTeamWinCount(int Set) { BlueTeamWinCount = Set; }

private:
	FName lobbyName = "LobyMap";
	FString playerSettingsSave = "PlayerSettingsSave";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Check", meta = (AllowPrivateAccess = "true"))
	bool IsCreateSaveFile = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Check", meta = (AllowPrivateAccess = "true"))
	bool IsLan = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Check", meta = (AllowPrivateAccess = "true"))
	bool IsFindServer = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Check", meta = (AllowPrivateAccess = "true"))
	int32 sessionsNum = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Check", meta = (AllowPrivateAccess = "true"))
	int32 sessionsSelectNum = 0;

	UFUNCTION(BlueprintPure)
	FString NetErrorToString(ENetworkFailure::Type FailureType);
	UFUNCTION(BlueprintPure)
	FString TravelErrorToString(ETravelFailure::Type FailureType);

	int RedTeamWinCount = 0;
	int BlueTeamWinCount = 0;

	FName JoinServerName = "";

};