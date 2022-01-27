// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHitCheck_UserWidget.generated.h"

/** 플레이어가 적에게 공격을 당하면 공격을 한 위치가 어디인지를 보여주는 위젯입니다.
 * 
 */
UCLASS()
class DALOS_API UPlayerHitCheck_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 공격한 적의 위치를 업데이트합니다.
	UFUNCTION(BlueprintNativeEvent)
	void HitLocCheck(FVector loc);
	virtual void HitLocCheck_Implementation(FVector loc);

};
