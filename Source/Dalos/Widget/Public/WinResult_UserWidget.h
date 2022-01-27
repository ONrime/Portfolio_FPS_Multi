// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WinResult_UserWidget.generated.h"

/** 매치 승리 결과를 알려주는 위젯입니다.
 * ResultTextVis로 승리한 팀이 결정되면 승리한 팀의 텍스트를 보여줍니다.
 */
UCLASS()
class DALOS_API UWinResult_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	void SetRedWin(int set) { RedWin = set; } void SetBlueWin(int set) { BlueWin = set; }

	// 가린 텍스트를 가릴지 공개 할지 결정(승리 팀 텍스트)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESlateVisibility ResultTextVis = ESlateVisibility::Hidden;

	// 승리한 팀
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ResultText = "";

	// 승리한 팀에 맞게 텍스트 색이 변경되는 함수입니다.
	// 혹시 또 몰라 BlueprintNativeEvent로 코드에서도 추가 작업을 할 수 있게 했습니다.
	UFUNCTION(BlueprintNativeEvent)
	void SetResultTextColor();
	virtual void SetResultTextColor_Implementation();

protected:
	// 각 팀별로 승리 횟수를 보여줍니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Count")
	int RedWin = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Count")
	int BlueWin = 0;
};
