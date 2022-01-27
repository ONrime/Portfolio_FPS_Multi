// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "TwoVersus_PlayerStart.generated.h"

/** 플레이어 스타트 지점을 나타내는 오브젝트입니다.
 * 플레이어가 스폰되고 각자의 팀 이름에 해당하는 스타트 지점으로 옮겨집니다.
 */
UCLASS()
class DALOS_API ATwoVersus_PlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	FString TeamName = "Red"; // 스타트 지점이 해당되는 팀

	bool IsUse = false; // 사용되었는지를 나타냅니다. (1회용)

};
