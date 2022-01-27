// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TwoVersus_GameState.generated.h"

/** 게임 전체에 관련된 데이터를 처리합니다.
 * 매치 승리 횟 수, 팀 전체 체력, 모든 플레이어 컨트롤러, 모든 시작지점등
 */

DECLARE_DELEGATE(FGameStateCheckDelegate)

UCLASS()
class DALOS_API ATwoVersus_GameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ATwoVersus_GameState();
	// PlayerState로 부터 각 플레이어의 체력을 가져와 팀 전체 체력을 구합니다.
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	TArray<class APlayerController*> AllPlayerController; // 모든 플레이어 컨트롤러
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	TArray<class AActor*> AllPlayerStart;  // 모든 시작지점

	FGameStateCheckDelegate ChangeTeamHPCheck;
	int GetRedTeamHP() { return RedTeamHP; } int GetBlueTeamHP() { return BlueTeamHP; }

protected:
	UPROPERTY(Replicated)
	int RedTeamHP = 0;  // 레드 팀 전체 체력
	UPROPERTY(Replicated)
	int BlueTeamHP = 0; // 블루 팀 전체 체력

public:
	int16 RedTeamWin = 0;
	int16 BlueTeamWin = 0;

private:


};
