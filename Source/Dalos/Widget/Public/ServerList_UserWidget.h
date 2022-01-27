// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "ServerList_UserWidget.generated.h"

/** 서버 찾기를 하여 찾은 서버 목록에서 행을 나타냅니다.
 * 
 */
UCLASS()
class DALOS_API UServerList_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	FString ServerName = "";	   // 서버의 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	int PlayerNum = 0;			   // 현재 있는 플레이어 인원 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	int Ping = 20;				   // 현재 핑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	int SelectNum = 0;			   // 본인의 서버 목록 순서 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	FString GameModeName = "2vs2"; // 게임 규칙(모드)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	FString State = "true";

	//찾은 서버
	FOnlineSessionSearchResult SearchResult;

	// 찾은 서버에서 해당하는 데이터를 뽑아내어 초기화를 합니다.
	UFUNCTION(BlueprintCallable)
	void ServerListAdd();

};
