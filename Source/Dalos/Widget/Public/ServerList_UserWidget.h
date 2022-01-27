// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "ServerList_UserWidget.generated.h"

/** ���� ã�⸦ �Ͽ� ã�� ���� ��Ͽ��� ���� ��Ÿ���ϴ�.
 * 
 */
UCLASS()
class DALOS_API UServerList_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	FString ServerName = "";	   // ������ �̸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	int PlayerNum = 0;			   // ���� �ִ� �÷��̾� �ο� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	int Ping = 20;				   // ���� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	int SelectNum = 0;			   // ������ ���� ��� ���� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	FString GameModeName = "2vs2"; // ���� ��Ģ(���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Server)
	FString State = "true";

	//ã�� ����
	FOnlineSessionSearchResult SearchResult;

	// ã�� �������� �ش��ϴ� �����͸� �̾Ƴ��� �ʱ�ȭ�� �մϴ�.
	UFUNCTION(BlueprintCallable)
	void ServerListAdd();

};
