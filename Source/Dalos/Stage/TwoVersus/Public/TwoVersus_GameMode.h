// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TwoVersus_GameMode.generated.h"

/** 게임 규칙 (2 대 2)
 * 5초의 카운트 후 게임이 시작되며
 * 주어진 시간 내로 다른 팀을 제거하면 승리
 * 시간이 다 끝났을 때 두 팀이 살아있다면 팀 전체 체력이 가장 높은 팀이 승리
 * 
 */
UCLASS()
class DALOS_API ATwoVersus_GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ATwoVersus_GameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	TArray<class ATwoVersus_PlayerController*> AllPlayerController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	TArray<class AActor*> AllPlayerStart;

	// 플레이어가 입장 할 때 작동되는 함수입니다.
	// 컨트롤러와 스타트 지점을 가져와 GameState에 등록합니다.

	virtual void RestartPlayer(AController* NewPlayer) override;

	// 이번 매치의 5초 카운트 다운을 시작합니다.
	void CountBeginPlayer();

	/* 플레이어가 죽은 상태라는 것을 전달 받으면
	* 팀 전체가 죽었는지를 확인하여 게임을 끝낼지 말지 결정하는 함수입니다.
	*/
	void CountPlayerDead(FString Team);

	void CountWin();

	/* 전체의 플레이어 시작지점 중에서 가능한 곳에 스폰하고 컨트롤러를 부여하는 함수입니다.
	* 클라이언트의 플레이어 컨트롤러가 스폰되어 시작 할 때 캐릭터가 스폰이 되도록 만들었습니다.
	* 클라이언트의 정보를 토대로 플레이어가 스폰되어야 하기 때문에 클라이언트의 컨틀로러가 스폰되어 시작 하기 전 까지 캐릭터가 스폰되지 않게 했습니다.
	*/
	void SpawnToPlayer(class ATwoVersus_PlayerController* Ctrl);

	int GetRedTeamWinCount() { return RedTeamWinCount; } int GetBlueTeamWinCount() { return BlueTeamWinCount; }
	void SetRedTeamCount(int Set) { RedTeamCount = Set; } void SetBlueTeamCount(int Set) { BlueTeamCount = Set; }
	int GetRedTeamCount() { return RedTeamCount; } int GetBlueTeamCount() { return BlueTeamCount; }

protected:

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	int32 RedTeamCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	int32 BlueTeamCount = 0;
	UPROPERTY()
	int RedTeamWinCount = 0;
	UPROPERTY()
	int BlueTeamWinCount = 0;
	int BeginPlayer = 0;
	int WinEnd = 2;

	bool GameEnd = false;
	bool MatchEnd = false;

	FTimerHandle WinResultTimer;
	// GameEnd에따라 게임을 끝내고 로비로 돌아갈지 아니면 다른 매치를 시작할지가 결정됩니다.
	void WinResultEnd();

	FTimerHandle StartCountTimer;
	/* 시작 카운트 다운이 끝 날때의 행동을 지정합니다.
	* 매치 카운트 다운을 시작합니다.
	*/
	void StartCountEnd();

	FTimerHandle GameCountTimer;
	void GameCountEnd();

};
