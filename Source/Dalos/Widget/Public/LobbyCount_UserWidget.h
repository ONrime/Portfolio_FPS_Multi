// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyCount_UserWidget.generated.h"

/** 로비에서 매치를 시작할 때 카운트 다운을 하는 위젯입니다.
 * 
 */

DECLARE_DELEGATE(FLobbyCountDelegate);

UCLASS()
class DALOS_API ULobbyCount_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CountDown")
	ESlateVisibility countVis = ESlateVisibility::Hidden; // 모두 준비상태면 공개합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CountDown")
	int countNum = 5; // 5초에서 부터 시작합니다

	UFUNCTION(BlueprintCallable)
	void PlayCountDown();
	void MinCount();
	UFUNCTION(BlueprintCallable)
	void StopCountDown();

	FLobbyCountDelegate CountDownEndCheck;

private:
	FTimerHandle countDownTimer;

};
