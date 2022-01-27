// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FindServer_UserWidget.generated.h"

/** 메뉴에서 Find Server에 해당합니다.
 * 서버를 찾고 리스트를 보여준뒤 플레이어가 선택한 서버로 들어가는 기능을 담당합니다.
 * 블루프린트에 대부분의 내용을 구현했습니다.
 */
UCLASS()
class DALOS_API UFindServer_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	TArray<FOnlineSessionSearchResult> Results;

	// 찾은 서버를 리스트에 추가합니다.
	UFUNCTION(BlueprintCallable)
	void AddFindServerList(class UServerList_UserWidget* widget, int i);

	// GameInstance를 통해 서버들을 찾는게 성공하면 발동되는 함수입니다.
	// 서버들을 Results에 넣습니다.
	UFUNCTION(BlueprintCallable)
	void FindServerSucceeded();

};
