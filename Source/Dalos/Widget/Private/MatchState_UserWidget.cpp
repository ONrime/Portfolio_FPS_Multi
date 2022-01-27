// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Widget/Public/MatchState_UserWidget.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerController.h"
#include "Kismet/GameplayStatics.h"

// 지정된 시간(매개 변수)에 따라 매치 타이머를 실행합니다.
void UMatchState_UserWidget::StartTimer(float Time)
{
	WidgetVis = ESlateVisibility::Visible;
	Count = Time;
	GetWorld()->GetTimerManager().SetTimer(MatchTimer, this, &UMatchState_UserWidget::MatchCount, 1.0f, true);
}

// 매치 타이머를 정지합니다.
void UMatchState_UserWidget::StopTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("StopTimer: MatchState"));
	WidgetVis = ESlateVisibility::Hidden;
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(MatchTimer)) 
	{
		GetWorld()->GetTimerManager().ClearTimer(MatchTimer);
	}
}

// 타이머가 끝나면 서버에 시간이 끝났음을 알립니다.
void UMatchState_UserWidget::MatchCount()
{
	Count--;
	if (Count == 0)
	{
		// 매치 앤드
		WidgetVis = ESlateVisibility::Hidden;
		ATwoVersus_PlayerController* Ctrl = Cast<ATwoVersus_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		Ctrl->Server_EndMatch();
		GetWorld()->GetTimerManager().ClearTimer(MatchTimer);
	}
}
