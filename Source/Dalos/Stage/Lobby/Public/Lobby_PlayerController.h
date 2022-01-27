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
	FPlayerInfo PlayerSetting; // �÷��̾� �⺻ ���� (�⺻ID, �� ����, �� ����, �غ� ���µ��� �⺻ ������ ������ �ֽ��ϴ�.

	TSubclassOf<class ULobby_Menu_UserWidget> Lobby_Menu_Class;

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	// ����Ʈ ���� ó�� *****************************************************************************************************
	/* �ʱ�ȭ �Լ��Դϴ�.
	* BeginPlay�� ��� ���ϴ� ������ ������ ������ Ŭ���̾�Ʈ�� ���� �ʿ��� ������ �ް� ����
	* �ʱ�ȭ�� �ϱ⸦ ���߱⿡ �ʱ�ȭ �Լ��� ���� ���� �����߽��ϴ�.
	* �α����� �ǰ� ������ Ŭ���̾�Ʈ�� ���� �̸�, �������� �÷��̾� ������ ������ AllPlayerInfo�� �߰��մϴ�.
	* �׶� AllPlayerInfo�߰��� �Ǹ鼭 �ٸ� Ŭ���̾�Ʈ���� �����Ͽ� ������ �����Ͱ� �� ���� �Ǿ��ٴ� ���� �˰Ե� Ŭ���̾�Ʈ��
	* ��Ʈ�ѷ��� ���� UI�� �ʱ�ȭ �մϴ�.
	*/
	void SetFirstPlayerList();
	void PlayerListAdd(FPlayerInfo Setting); // �÷��̾ �߰� �� �� ������ �ִ� �÷��̾���� ����Ʈ UI�� ������Ʈ �մϴ�.
	void DelPlayerInfo(FPlayerInfo Setting); // �÷��̾ ���� �� ������ �ִ� �÷��̾���� ����Ʈ UI�� ������Ʈ �մϴ�.

	/*********************************************************************************************************************/

	/* �÷��̾ �κ�� ������(�α����� ��) �ʱ�ȭ�� �Ǵ� ���� *****************************************************************
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

	void SetInitSetting(FString Team, FString Room, int FirstID); // GameMode�� ���� ��, �� ����, ID�� �޾� Ŭ���̾�Ʈ�� �����մϴ�.

	// ��, �� ����, ID�� ���� Ŭ���̾�Ʈ�� �ڱ⿡�� ����� ������(�÷��̾� �̸�, ������)�� �ҷ��ɴϴ�. 
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_LoginPlayer(const FString& TeamName, const FString& State, int FirstID);
	bool Client_LoginPlayer_Validate(const FString& TeamName, const FString& State, int FirstID);
	void Client_LoginPlayer_Implementation(const FString& TeamName, const FString& State, int FirstID);

	/* ������ ��Ʈ�ѷ��� ���� ���� �÷��̾� ������ GameState�� ����(����, Ŭ���̾�Ʈ)
	* ���� ���� GameState�� ����(���� ����)�� Ŭ���̾�Ʈ���� ������ �ʱ�ȭ �϶�� �˸��ϴ�.
	*/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendPlayerSetting(FPlayerInfo Setting);  // ������ ���̺� ����
	bool Server_SendPlayerSetting_Validate(FPlayerInfo Setting);
	void Server_SendPlayerSetting_Implementation(FPlayerInfo Setting);

	/*********************************************************************************************************************/


	/* �÷��̾� ������ �ٲ� �� �۵��ϴ� �Լ��Դϴ�. *****************************************************************************
	*/
	// �غ� ����, �� ����, �� ���°� �ٲ� �� �۵��˴ϴ�.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ChangeAllPlayerInfo(int Index, int Category, const FString& Change); // �ٲ� ���� ������ ����
	bool Server_ChangeAllPlayerInfo_Validate(int Index, int Category, const FString& Change);
	void Server_ChangeAllPlayerInfo_Implementation(int Index, int Category, const FString& Change);

	// �ٲ� ����(��, ��, �غ�)�� UI�� ������Ʈ �մϴ�.
	UFUNCTION(Client, Reliable, WithValidation)
	void Client_ChangePlayerInfo(FPlayerInfo Setting, int Category, const FString& Change); // �ٲ� ���� �� Ŭ���̾�Ʈ(����)���� ����
	bool Client_ChangePlayerInfo_Validate(FPlayerInfo Setting, int Category, const FString& Change);
	void Client_ChangePlayerInfo_Implementation(FPlayerInfo Setting, int Category, const FString& Change);

	/*********************************************************************************************************************/

	/* ����(��ġ) ������ ���� �� �� �۵��ϴ� �Լ��Դϴ�. *************************************************************************
	*/
	// ����(��ġ) ������ ����Ǹ� GameState�� �˸��ϴ�.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ChangeGameSetting(FGameSetting Setting); // �ٲ� ���� ���� ������ ����
	bool Server_ChangeGameSetting_Validate(FGameSetting Setting);
	void Server_ChangeGameSetting_Implementation(FGameSetting Setting);

	// GameState�� ���� ����Ǹ� �ٲ� ����(��ġ) ������ UI�� ������Ʈ �մϴ�.
	void ChangeGameSetting();

	/*********************************************************************************************************************/

	/* ä�ÿ� ���õ� �Լ��Դϴ�. *********************************************************************************************
	*/
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void GetChatMessage(const FString& TextToSend);						   // ������ ä�� ���� ����
	bool GetChatMessage_Validate(const FString& TextToSend);
	void GetChatMessage_Implementation(const FString& TextToSend);
	UFUNCTION(Client, Reliable, WithValidation)
	void UpdateText(const FString& SenderText, const FString& SenderName); // ��� �÷��̾� ��Ʈ�ѷ����� ü�� ������Ʈ
	bool UpdateText_Validate(const FString& SenderText, const FString& SenderName);
	void UpdateText_Implementation(const FString& SenderText, const FString& SenderName);

	/*********************************************************************************************************************/

	/* ī��Ʈ �ٿ�(��ġ ����) ���õ� �Լ��Դϴ�. *******************************************************************************
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
	void Server_LeaveSession(); // ���� ������
	bool Server_LeaveSession_Validate();
	void Server_LeaveSession_Implementation();


	//void LoginPlayer(const FString& TeamName, const FString& State); // (������, ���� ����) ������ ������ �����, ���� �ִٸ� �ʱ� ������ �ְ� ����Ʈ�� �ø���
	
	int GetSettingID() { return SettingID; }
	
	void SetTeamState(FString Set);

protected:
	/* ������ ���� ���� ��, �� ����, ID�� �ڱ⿡�� ����� ������(�÷��̾� �̸�, ������)�� ��ģ
	* FPlayerInfo ����ü�� �־ ������ �����մϴ�.
	*/
	void SaveGameCheck(const FString& Team, const FString& State, int FirstID); // ó���� ���̺� Ȯ���ϱ�

	UPROPERTY(Replicated)
	FString TeamState	 = "";
	UPROPERTY(ReplicatedUsing = OnRep_UpdateClinetFirstPlayerList)
	FString RoomState = "";
	UPROPERTY(Replicated)
	int SettingID = 0;

	// �� ���¸� UI�� ������Ʈ �մϴ�.
	UFUNCTION()
	void OnRep_UpdateClinetFirstPlayerList();

private:




};
