// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Lobby_GameMode.generated.h"

/**로비 게임 모드
 * 로비에 관련되 규칙 관리 (타이머)
 * 매치 시작시 매치의 새로운 맵을 로드하기 때문에 매끄럽지 않은 이동을 합니다.
 */
UCLASS()
class DALOS_API ALobby_GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ALobby_GameMode();

	/* 로그인 이후 이벤트를 발동하는 함수입니다.
	* 로그인된 컨트롤러에게 팀과 방 상태를 전달합니다.
	* GameState에 현재의 게임 세팅을 로그인된 컨트롤러를 전달합니다.
	*/
	// 로그인 이후 이벤트를 발동하는 함수입니다. 플레이어 컨트롤러에서 로비를 구성하고 플레이어 스폰
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 매치 시작 전 작동되는 카운트 다운 멈추기
	void StopCountDown();
	// 매치 시작 전 작동되는 카운트 다운 시작하기
	void StartCountDown();

protected:

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	int RedTeamCount = 0;  // 현재 로그인된 레드 팀 인원 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	int BlueTeamCount = 0; // 현재 로그인된 블루 팀 인원 수
	bool IsHost = false;   // 이 로비에 호스트가 존재하는지를 알려주는 변수입니다.
	int GiveID = 0;        // 컨트롤러에 부여되는 기본ID (이 ID로 플레이어를 구분합니다.)

	// 매치 시작 전 작동되는 카운트 다운 타이머
	FTimerHandle CountDownTimer;
	void MinCount();  // 카운트 다운 함수입니다.
	int CountNum = 5; // 카운트를 세는 변수입니다.

};
