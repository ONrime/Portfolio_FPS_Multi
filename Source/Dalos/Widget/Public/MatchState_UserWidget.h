// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchState_UserWidget.generated.h"

/** 매치의 상태를 보여줍니다.
 * 매치의 남은 시간과 각 팀의 승리 횟수를 보여줍니다.
 */
UCLASS()
class DALOS_API UMatchState_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	float Count = 40.0f; // 남은 매치 시간
	UPROPERTY(BlueprintReadOnly)
	int RedTeamWinCount = 0; // 레드 팀 승리 횟수
	UPROPERTY(BlueprintReadOnly)
	int BlueTeamWinCount = 0; // 블루 팀 승리 횟수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESlateVisibility WidgetVis = ESlateVisibility::Hidden; // 이 위젯의 보이기 상태

	// 지정된 시간(매개 변수)에 따라 매치 타이머를 실행합니다.
	void StartTimer(float Time);
	// 매치 타이머를 정지합니다.
	void StopTimer();

private:
	// 타이머
	FTimerHandle MatchTimer;

	// 타이머가 끝나면 서버에 시간이 끝났음을 알립니다.
	void MatchCount();
	
};
