// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "LobbyMenu_UserWidget.generated.h"

/** 로비 메뉴 위젯
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLobbyChatDelegate, FString, Sender, FString, Text);

UCLASS()
class DALOS_API ULobbyMenu_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FString serverName; // 서버 이름

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FLobbyChatDelegate UpdateChat; // 채팅 내용을 보낼 때 작동됩니다.

	UPROPERTY(Replicated, Category = LobbyInfo, EditAnywhere, BlueprintReadWrite)
	FString mapName; // 맵 이름
	UPROPERTY(Replicated, Category = LobbyInfo, EditAnywhere, BlueprintReadWrite)
	FString mapTime; // 정해진 시간
	UPROPERTY(Replicated, Category = LobbyInfo, EditAnywhere, BlueprintReadWrite)
	UTexture2D* mapImage; // 맵 이미지

	UPROPERTY(Replicated, Category = LobbyInfo, EditAnywhere, BlueprintReadWrite)
	FString playerDisplay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESlateVisibility WidgetVis = ESlateVisibility::Visible;

protected:

private:

};
