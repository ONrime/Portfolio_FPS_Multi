// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "LobbyMenu_UserWidget.generated.h"

/** �κ� �޴� ����
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLobbyChatDelegate, FString, Sender, FString, Text);

UCLASS()
class DALOS_API ULobbyMenu_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FString serverName; // ���� �̸�

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FLobbyChatDelegate UpdateChat; // ä�� ������ ���� �� �۵��˴ϴ�.

	UPROPERTY(Replicated, Category = LobbyInfo, EditAnywhere, BlueprintReadWrite)
	FString mapName; // �� �̸�
	UPROPERTY(Replicated, Category = LobbyInfo, EditAnywhere, BlueprintReadWrite)
	FString mapTime; // ������ �ð�
	UPROPERTY(Replicated, Category = LobbyInfo, EditAnywhere, BlueprintReadWrite)
	UTexture2D* mapImage; // �� �̹���

	UPROPERTY(Replicated, Category = LobbyInfo, EditAnywhere, BlueprintReadWrite)
	FString playerDisplay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESlateVisibility WidgetVis = ESlateVisibility::Visible;

protected:

private:

};
