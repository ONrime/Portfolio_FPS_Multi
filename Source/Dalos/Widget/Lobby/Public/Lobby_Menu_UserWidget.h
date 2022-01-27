// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "Lobby_Menu_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API ULobby_Menu_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintNativeEvent)
	void FirstInPlayerList(const TArray<FPlayerInfo>& Settings); // ó�� ����Ʈ�� �� �� (ù �α���)
	void FirstInPlayerList_Implementation(const TArray<FPlayerInfo>& Settings);
	UFUNCTION(BlueprintNativeEvent)
	void AddPlayerList(FPlayerInfo Setting); // �߰��� ������ �÷��̾ ����Ʈ�� �߰�
	void AddPlayerList_Implementation(FPlayerInfo Setting);
	UFUNCTION(BlueprintNativeEvent)
	void DelPlayerList(FPlayerInfo Setting); // �κ񿡼� ������ �÷��̾ ����Ʈ�� ����
	void DelPlayerList_Implementation(FPlayerInfo Setting);
	UFUNCTION(BlueprintNativeEvent)
	void ChangeReady(int FirstID, const FString& Change); // Ready���� �ٲٱ�
	void ChangeReady_Implementation(int FirstID, const FString& Change);
	UFUNCTION(BlueprintNativeEvent)
	void ChangeTeam(FPlayerInfo Setting, bool RedCheck); // �� ���� �ٲٱ�
	void ChangeTeam_Implementation(FPlayerInfo Setting, bool RedCheck);
	UFUNCTION(BlueprintNativeEvent)
	void ChangeRoom(); // �� ���� �ٲٱ�
	void ChangeRoom_Implementation();
	UFUNCTION(BlueprintNativeEvent)
	void ChangeGameSetting(FGameSetting Setting); // �߰��� ������ �÷��̾ ����Ʈ�� �߰�
	void ChangeGameSetting_Implementation(FGameSetting Setting);
	UFUNCTION(BlueprintNativeEvent)
	void UpdateChat(const FString& SenderText, const FString& SenderName); // Ready���� �ٲٱ�
	void UpdateChat_Implementation(const FString& SenderText, const FString& SenderName);
	UFUNCTION(BlueprintNativeEvent)
	void InitWidget(); // ��� ����Ʈ�� �ִ� ������ ����(�ʱ�ȭ)�Ѵ�
	void InitWidget_Implementation();

	void StartCountDown();
	void StopCountDown();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class ULobby_PlayerList_UserWidget* PlayerList_Widget = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class ULobbyCount_UserWidget* StateAndCountDown_Widget = nullptr;

	

	UFUNCTION(BlueprintCallable)
	void PlayerClickReadyButton();
	UFUNCTION(BlueprintCallable)
	void PlayerClickTeamButton(bool RedCheck);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	FString ReadyState = "NotReady";

};
