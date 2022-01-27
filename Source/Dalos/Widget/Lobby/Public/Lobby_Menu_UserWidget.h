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
	void FirstInPlayerList(const TArray<FPlayerInfo>& Settings); // 처음 리스트에 들어갈 때 (첫 로그인)
	void FirstInPlayerList_Implementation(const TArray<FPlayerInfo>& Settings);
	UFUNCTION(BlueprintNativeEvent)
	void AddPlayerList(FPlayerInfo Setting); // 추가로 들어오는 플레이어를 리스트에 추가
	void AddPlayerList_Implementation(FPlayerInfo Setting);
	UFUNCTION(BlueprintNativeEvent)
	void DelPlayerList(FPlayerInfo Setting); // 로비에서 나가면 플레이어를 리스트에 삭제
	void DelPlayerList_Implementation(FPlayerInfo Setting);
	UFUNCTION(BlueprintNativeEvent)
	void ChangeReady(int FirstID, const FString& Change); // Ready상태 바꾸기
	void ChangeReady_Implementation(int FirstID, const FString& Change);
	UFUNCTION(BlueprintNativeEvent)
	void ChangeTeam(FPlayerInfo Setting, bool RedCheck); // 팀 상태 바꾸기
	void ChangeTeam_Implementation(FPlayerInfo Setting, bool RedCheck);
	UFUNCTION(BlueprintNativeEvent)
	void ChangeRoom(); // 팀 상태 바꾸기
	void ChangeRoom_Implementation();
	UFUNCTION(BlueprintNativeEvent)
	void ChangeGameSetting(FGameSetting Setting); // 추가로 들어오는 플레이어를 리스트에 추가
	void ChangeGameSetting_Implementation(FGameSetting Setting);
	UFUNCTION(BlueprintNativeEvent)
	void UpdateChat(const FString& SenderText, const FString& SenderName); // Ready상태 바꾸기
	void UpdateChat_Implementation(const FString& SenderText, const FString& SenderName);
	UFUNCTION(BlueprintNativeEvent)
	void InitWidget(); // 블루 프린트에 있는 위젯과 연결(초기화)한다
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
