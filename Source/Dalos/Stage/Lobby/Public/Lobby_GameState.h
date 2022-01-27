// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "Lobby_GameState.generated.h"

/** 게임(로비)의 전체 관리
 * 
 */
UCLASS()
class DALOS_API ALobby_GameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ALobby_GameState();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings)
	TArray<class APlayerController*> AllPlayerController; // 현재로 로비에 존재하는 모든 컨트롤러
	/* 현재 로비에 존재하는 모든 플레이어의 기본 정보
	* 기본ID, 방 상태, 팀 상태, 준비 상태등의 기본 정보를 가지고 있습니다.
	*/
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_AllPlayerInfoChange, BlueprintReadWrite, Category = ServerSettings)
	TArray<FPlayerInfo> AllPlayerInfo;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings)
	TArray<class AActor*> AllPlayerStart;				  // 모든 플레이어 스타트 지점
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings)
	FString ServerName;									  // 이 서버의 이름
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings)
	int MaxPlayers;										  // 이 서버가 인용할 수 있는 인원
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings)
	int CurrentPlayers = 0;								  // 현재 로비에 들어온 플레이어 인원
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_GameSettingChange, BlueprintReadWrite, Category = ServerSettings)
	FGameSetting GameSetting;							  // 게임 세팅
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_DelPlayerChange, BlueprintReadWrite, Category = ServerSettings)
	FPlayerInfo DelPlayer;								  // 나가는 플레이어

	/* 플레이어의 정보가 바뀐다면 작동되는 함수입니다.
	* 정확히는 팀의 상태나, 준비 상태가 바뀔 때 작동되어 다른 클라이언트에게 전달하는 함수입니다.
	*/
	UFUNCTION(NetMultiCast, Reliable, WithValidation)
	void NetMultiCast_ChangePlayerInfo(int Index, int Category, const FString& Change);
	bool NetMultiCast_ChangePlayerInfo_Validate(int Index, int Category, const FString& Change);
	void NetMultiCast_ChangePlayerInfo_Implementation(int Index, int Category, const FString& Change);
	
	/* AllPlayerInfo가 바뀔 때 작동
	* 로그인 되었을 때 작동되며 처음 들어올 떄, 추가로 들어올 때, 나갈 때의 처리(전체 인원의 수를 줄일때만)를 합니다.
	*/
	UFUNCTION()
	void OnRep_AllPlayerInfoChange();
	UFUNCTION()
	void OnRep_GameSettingChange(); // 호스트에 의해 게임 세팅이 변경되고 클라이언트에 업데이트 될 때 작동되는 함수입니다.
	UFUNCTION()
	void OnRep_DelPlayerChange();   // 나갈 떄의 처리를 합니다.
	
	//UPROPERTY(Replicated)
	int AddPlayerInfoNum = 0;


protected:

private:

};
