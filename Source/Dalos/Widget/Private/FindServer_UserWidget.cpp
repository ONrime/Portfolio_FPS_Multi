// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Widget/Public/FindServer_UserWidget.h"
#include "Dalos/Widget/Public/ServerList_UserWidget.h"
#include "Dalos/Game/Public/GameInfo_Instance.h"

// GameInstance를 통해 서버들을 찾는게 성공하면 발동되는 함수입니다.
// 서버들을 Results에 넣습니다.
void UFindServer_UserWidget::FindServerSucceeded()
{
	UGameInfo_Instance* Ins = Cast<UGameInfo_Instance>(GetGameInstance());
	if (Ins && Ins->Results.Num() > 0)
	{
		Results = Ins->Results;
	}
}

// 찾은 서버를 리스트에 추가합니다.
void UFindServer_UserWidget::AddFindServerList(UServerList_UserWidget* widget, int i)
{
	if (widget) {
		widget->SearchResult = Results[i];
		widget->SelectNum = i;
	}
}
