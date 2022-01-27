// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrossHair_UserWidget.generated.h"

/** 십자선 위젯입니다.
 * 
 */
UCLASS()
class DALOS_API UCrossHair_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cross")
	ESlateVisibility corssHairVis = ESlateVisibility::Visible; // 일반상태에서는 보이지만 특수 상태(달리기, 넘어가기 등)에서는 가립니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cross")
	bool IsRed = false; // 적을 가리킬 때는 빨간색으로 변합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cross")
	float CrossHairSpread = 0.0f; // 십자선 퍼짐 정도

};
