// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionMenu_UserWidget.generated.h"

/** 옵션 메뉴(현재 사용X)
 * 연습용 입니다.
 */
UCLASS()
class DALOS_API UOptionMenu_UserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSettings")
	ESlateVisibility WelcomeMessageVis = ESlateVisibility::Visible;

protected:

private:

};
