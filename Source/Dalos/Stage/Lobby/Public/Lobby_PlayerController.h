// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "Lobby_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API ALobby_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
public:
	ALobby_PlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class ULobby_Menu_UserWidget* LobbyMenu_Widget = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerInfo", Replicated)
	FPlayerInfo PlayerSetting; // 플레이어 기본 정보 (기본ID, 방 상태, 팀 상태, 준비 상태등의 기본 정보를 가지고 있습니다.

	TSubclassOf<class ULobby_Menu_UserWidget> Lobby_Menu_Class;

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	// 리스트 관련 처리 *****************************************************************************************************
	/* 초기화 함수입니다.
	* BeginPlay를 사용 안하는 이유는 서버가 완전히 클라이언트로 부터 필요한 정보를 받고 나서
	* 초기화를 하기를 원했기에 초기화 함수를 따로 만들어서 실행했습니다.
	* 로그인이 되고 서버가 클라이언트로 부터 이름, 사진등의 플레이어 정보를 받으면 AllPlayerInfo를 추가합니다.
	* 그때 AllPlayerInfo추가가 되면서 다른 클라이언트에게 전달하여 서버에 데이터가 잘 전달 되었다는 것을 알게된 클라이언트가
	* 컨트롤러를 통해 UI를 초기화 합니다.
	*/
	void SetFirstPlayerList();
	void PlayerListAdd(FPlayerInfo Setting); // 플레이어가 추가 될 때 기존에 있는 플레이어들의 리스트 UI를 업데이트 합니다.
	void DelPlayerInfo(FPlayerInfo Setting); // 플레이어가 나갈 때 기존에 있는 플레이어들의 리스트 UI를 업데이트 합니다.

	/*********************************************************************************************************************/

	/* 플레이어가 로비로 들어오면(로그인이 된) 초기화가 되는 과정 *****************************************************************
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

	void SetInitSetting(FString Team, FString Room, int FirstID); // GameMode로 부터 팀, 방 상태, ID를 받아 클라이언트에 전달합니다.

	// 팀, 방 상태, ID를 받은 클라이언트는 자기에게 저장된 데이터(플레이어 이름, 사진등)를 불러옵니다. 
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_LoginPlayer(const FString& TeamName, const FString& State, int FirstID);
	bool Client_LoginPlayer_Validate(const FString& TeamName, const FString& State, int FirstID);
	void Client_LoginPlayer_Implementation(const FString& TeamName, const FString& State, int FirstID);

	/* 서버의 컨트롤러는 전달 받은 플레이어 정보를 GameState에 전달(서버, 클라이언트)
	* 전달 받은 GameState는 서버(리슨 서버)와 클라이언트에게 위젯을 초기화 하라고 알립니다.
	*/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendPlayerSetting(FPlayerInfo Setting);  // 서버에 세이브 전달
	bool Server_SendPlayerSetting_Validate(FPlayerInfo Setting);
	void Server_SendPlayerSetting_Implementation(FPlayerInfo Setting);

	/*********************************************************************************************************************/


	/* 플레이어 정보가 바뀔 때 작동하는 함수입니다. *****************************************************************************
	*/
	// 준비 상태, 팀 상태, 방 상태가 바뀔 때 작동됩니다.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ChangeAllPlayerInfo(int Index, int Category, const FString& Change); // 바뀐 세팅 서버에 전달
	bool Server_ChangeAllPlayerInfo_Validate(int Index, int Category, const FString& Change);
	void Server_ChangeAllPlayerInfo_Implementation(int Index, int Category, const FString& Change);

	// 바뀐 정보(팀, 방, 준비)를 UI에 업데이트 합니다.
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_ChangePlayerInfo(FPlayerInfo Setting, int Category, const FString& Change); // 바뀐 세팅 각 클라이언트(개인)에게 전달
	bool Client_ChangePlayerInfo_Validate(FPlayerInfo Setting, int Category, const FString& Change);
	void Client_ChangePlayerInfo_Implementation(FPlayerInfo Setting, int Category, const FString& Change);

	/*********************************************************************************************************************/

	/* 게임(매치) 세팅이 변경 될 때 작동하는 함수입니다. *************************************************************************
	*/
	// 게임(매치) 세팅이 변경되면 GameState에 알립니다.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ChangeGameSetting(FGameSetting Setting); // 바뀐 게임 세팅 서버에 전달
	bool Server_ChangeGameSetting_Validate(FGameSetting Setting);
	void Server_ChangeGameSetting_Implementation(FGameSetting Setting);

	// GameState를 통해 실행되며 바뀐 게임(매치) 세팅을 UI에 업데이트 합니다.
	void ChangeGameSetting();

	/*********************************************************************************************************************/

	/* 채팅에 관련된 함수입니다. *********************************************************************************************
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void GetChatMessage(const FString& TextToSend);						   // 서버에 채팅 내용 전달
	bool GetChatMessage_Validate(const FString& TextToSend);
	void GetChatMessage_Implementation(const FString& TextToSend);
	UFUNCTION(Client, Reliable, WithValidation)
	void UpdateText(const FString& SenderText, const FString& SenderName); // 모든 플레이어 컨트롤러에게 체팅 업데이트
	bool UpdateText_Validate(const FString& SenderText, const FString& SenderName);
	void UpdateText_Implementation(const FString& SenderText, const FString& SenderName);

	/*********************************************************************************************************************/

	/* 카운트 다운(매치 시작) 관련된 함수입니다. *******************************************************************************
	*/
	UFUNCTION(Client, Reliable, WithValidation)
	void StartCountDown();
	bool StartCountDown_Validate();
	void StartCountDown_Implementation();
	UFUNCTION(Client, Reliable, WithValidation)
	void StopCountDown();
	bool StopCountDown_Validate();
	void StopCountDown_Implementation();

	/*********************************************************************************************************************/

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_LeaveSession(); // 세션 나가기
	bool Server_LeaveSession_Validate();
	void Server_LeaveSession_Implementation();


	//void LoginPlayer(const FString& TeamName, const FString& State); // (서버용, 리슨 서버) 위젯이 없으면 만들고, 만약 있다면 초기 정보를 넣고 리스트에 올리기
	
	int GetSettingID() { return SettingID; }
	
	void SetTeamState(FString Set);

protected:
	/* 서버로 부터 받은 팀, 방 상태, ID와 자기에게 저장된 데이터(플레이어 이름, 사진등)를 합친
	* FPlayerInfo 구조체에 넣어서 서버에 전달합니다.
	*/
	void SaveGameCheck(const FString& Team, const FString& State, int FirstID); // 처음에 세이브 확인하기

	UPROPERTY(Replicated)
	FString TeamState	 = "";
	UPROPERTY(ReplicatedUsing = OnRep_UpdateClinetFirstPlayerList)
	FString RoomState = "";
	UPROPERTY(Replicated)
	int SettingID = 0;

	// 방 상태를 UI에 업데이트 합니다.
	UFUNCTION()
	void OnRep_UpdateClinetFirstPlayerList();

private:




};
