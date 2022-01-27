// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HitCheck_UserWidget.generated.h"

/** 탄이 적을 맞추면 히트 표시를 하는 위젯입니다.
 * 
 */
UCLASS()
class DALOS_API UHitCheck_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 탄에 맞은 적이 죽으면 히트 표시를 빨간색으로 합니다.
	UFUNCTION(BlueprintNativeEvent)
	void RedCheck(bool check);
	virtual void RedCheck_Implementation(bool check);

};
