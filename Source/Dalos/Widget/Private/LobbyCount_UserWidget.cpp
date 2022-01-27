// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Widget/Public/LobbyCount_UserWidget.h"

void ULobbyCount_UserWidget::PlayCountDown()
{
	countVis = ESlateVisibility::Visible;
	countNum = 5;
	GetWorld()->GetTimerManager().SetTimer(countDownTimer, this, &ULobbyCount_UserWidget::MinCount, 1.0f, true);
}

void ULobbyCount_UserWidget::StopCountDown()
{
	countVis = ESlateVisibility::Hidden;
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(countDownTimer)) {
		GetWorld()->GetTimerManager().ClearTimer(countDownTimer);
	}
	//GetWorld()->GetTimerManager().ClearTimer(countDownTimer);
}

void ULobbyCount_UserWidget::MinCount()
{
	countNum -= 1;
	if (countNum == 0) {
		GetWorld()->GetTimerManager().ClearTimer(countDownTimer);
		if(CountDownEndCheck.IsBound()) CountDownEndCheck.Execute();
	}
}
