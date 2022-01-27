// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchHealth_UserWidget.generated.h"

/** 매치 내의 전체 팀 체력을 보여줍니다.
 * 
 */
UCLASS()
class DALOS_API UMatchHealth_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	int RedTeamHP = 0;
	UPROPERTY(BlueprintReadOnly)
	int BlueTeamHP = 0;

private:


};
