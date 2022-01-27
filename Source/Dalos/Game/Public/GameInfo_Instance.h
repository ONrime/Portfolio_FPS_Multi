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
	virtual void HandleNetworkError(ENetworkFailure::Type FailureType, bool IsServer); // 네트워크 에러 처리 및 메세지 출력
	virtual void HandleTravelError(ETravelFailure::Type FailureType); // 이동시 에러 처리 및 메세지 출력

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
	void ShowMainMenu();  // 메인 메뉴 화면 전환 and 마우스 활성화
	UFUNCTION(BlueprintCallable, Category = "Event")
	void ShowHostMenu();  // 호스트 메뉴 화면 전환 and 마우스 활성화
	UFUNCTION(BlueprintCallable, Category = "Event")
	void ShowServerMenu();  // 서버 메뉴 화면 전환 and 마우스 활성화
	UFUNCTION(BlueprintCallable, Category = "Event")
	void ShowOptionMenu();  // 옵션 메뉴 화면 전환 and 마우스 활성화
	UFUNCTION(BlueprintCallable, Category = "Event")
	void LaunchLobby(int32 PlayerNum, FName ServerName, FName GameModeName, bool LanCheck, bool IsDedicated);  // 로비를 시작하고 호스트를 세팅한다.
	UFUNCTION(BlueprintCallable, Category = "Event")
	void JoinServer(bool DedicatedCheck);  // 서버에 들어가기
	UFUNCTION(BlueprintCallable, Category = "Event")
	void FindServer(bool LanCheck, bool DedicatedCheck);  // 서버 찾기
	UFUNCTION(BlueprintCallable, Category = "Event")
	void ShowLodingScreen(); // 요구시 로딩 화면 출력
		//UFUNCTION(BlueprintCallable, Category = "Event")
	void Destroy_SessionCaller(class APlayerController PC); // 호출 시 세션 파괴
	UFUNCTION(BlueprintCallable, Category = "Event")
	void Check_SaveGame();  // 저장 파일 확인 하기
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
	int MatchTime = 120; // 꼭 바꾸기

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