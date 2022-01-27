// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FindServer_UserWidget.generated.h"

/** �޴����� Find Server�� �ش��մϴ�.
 * ������ ã�� ����Ʈ�� �����ص� �÷��̾ ������ ������ ���� ����� ����մϴ�.
 * �������Ʈ�� ��κ��� ������ �����߽��ϴ�.
 */
UCLASS()
class DALOS_API UFindServer_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	TArray<FOnlineSessionSearchResult> Results;

	// ã�� ������ ����Ʈ�� �߰��մϴ�.
	UFUNCTION(BlueprintCallable)
	void AddFindServerList(class UServerList_UserWidget* widget, int i);

	// GameInstance�� ���� �������� ã�°� �����ϸ� �ߵ��Ǵ� �Լ��Դϴ�.
	// �������� Results�� �ֽ��ϴ�.
	UFUNCTION(BlueprintCallable)
	void FindServerSucceeded();

};
