// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Widget/Public/ServerList_UserWidget.h"

// ã�� ���� ����� �ش��ϴ� �����͸� �̾Ƴ��� �ʱ�ȭ�� �մϴ�.
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
