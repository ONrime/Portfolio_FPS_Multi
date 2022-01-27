// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Widget/Public/MatchState_UserWidget.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerController.h"
#include "Kismet/GameplayStatics.h"

// ������ �ð�(�Ű� ����)�� ���� ��ġ Ÿ�̸Ӹ� �����մϴ�.
void UMatchState_UserWidget::StartTimer(float Time)
{
	WidgetVis = ESlateVisibility::Visible;
	Count = Time;
	GetWorld()->GetTimerManager().SetTimer(MatchTimer, this, &UMatchState_UserWidget::MatchCount, 1.0f, true);
}

// ��ġ Ÿ�̸Ӹ� �����մϴ�.
void UMatchState_UserWidget::StopTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("StopTimer: MatchState"));
	WidgetVis = ESlateVisibility::Hidden;
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(MatchTimer)) 
	{
		GetWorld()->GetTimerManager().ClearTimer(MatchTimer);
	}
}

// Ÿ�̸Ӱ� ������ ������ �ð��� �������� �˸��ϴ�.
void UMatchState_UserWidget::MatchCount()
{
	Count--;
	if (Count == 0)
	{
		// ��ġ �ص�
		WidgetVis = ESlateVisibility::Hidden;
		ATwoVersus_PlayerController* Ctrl = Cast<ATwoVersus_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		Ctrl->Server_EndMatch();
		GetWorld()->GetTimerManager().ClearTimer(MatchTimer);
	}
}
