// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Widget/Public/ServerList_UserWidget.h"

// 찾은 서버 목록을 해당하는 데이터를 뽑아내어 초기화를 합니다.
void UServerList_UserWidget::ServerListAdd()
{
	FName SessionName = FName(*(SearchResult.Session.SessionSettings.Settings.FindRef("SESSION_NAME").Data.ToString()));
	//ServerName = FName(*(SearchResult.Session.OwningUserName)).ToString();
	ServerName = SessionName.ToString();
	if (ServerName == "") ServerName = "NoBody";
	PlayerNum = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
	FName ModeName = FName(*(SearchResult.Session.SessionSettings.Settings.FindRef("GAMEMODE_NAME").Data.ToString()));
	GameModeName = ModeName.ToString();
	Ping = SearchResult.PingInMs;
}
