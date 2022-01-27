// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Stage/Lobby/Public/Lobby_GameState.h"
#include "Dalos/Stage/Lobby/Public/Lobby_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Net/UnrealNetwork.h"

ALobby_GameState::ALobby_GameState()
{

}

/* 플레이어의 정보가 바뀐다면 작동되는 함수입니다.
* 정확히는 팀의 상태나, 준비 상태가 바뀔 때 작동되어 다른 클라이언트에게 전달하는 함수입니다.
*/
bool ALobby_GameState::NetMultiCast_ChangePlayerInfo_Validate(int Index, int Category, const FString& Change)
{
	return true;
}
void ALobby_GameState::NetMultiCast_ChangePlayerInfo_Implementation(int Index, int Category, const FString& Change)
{
	// 리플리케이티드(복제)가 되어있지만 빠르게 전달 시키기 위해 rpc를 썼다. (더 느리다)
	//AllPlayerInfo.Add(Setting); // 서버, 클라이언트에 플레이어 정보 전달
	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("AddAllPlayerInfo: Server"));
	}else UE_LOG(LogTemp, Warning, TEXT("AddAllPlayerInfo: Client"));

	ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (Category == 0) { // 0 이면 Ready를 1 이면 팀을 바꾼다
		AllPlayerInfo[Index].playerReadyStatus = Change;
	}
	else {
		AllPlayerInfo[Index].playerTeamStatus = Change;
	}
}

/* AllPlayerInfo가 바뀔 때 작동
* 로그인 되었을 때 작동되며 처음 들어올 떄, 추가로 들어올 때, 나갈 때의 처리(전체 인원의 수를 줄일때만)를 합니다.
*/
void ALobby_GameState::OnRep_AllPlayerInfoChange()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_AllPlayerInfoChange:%d "), AddPlayerInfoNum);
	ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("HasAuthority"));
	}

	/* 로그인이 되고 서버가 클라이언트로 부터 이름, 사진등의 플레이어 정보를 받으면 AllPlayerInfo를 추가합니다.
	* 그때 AllPlayerInfo추가가 되면서 다른 클라이언트에게 전달하여 서버에 데이터가 잘 전달 되었다는 것을 알게된 클라이언트가
	* 컨트롤러를 통해 UI를 초기화 합니다.
	*/
	if (AddPlayerInfoNum == 0)
	{ // 초기화
		AddPlayerInfoNum = AllPlayerInfo.Num();
		Ctrl->SetFirstPlayerList();
		UE_LOG(LogTemp, Warning, TEXT("AddPlayerInfoNum_First: %d"), AddPlayerInfoNum);
		return;
	}

	if (AddPlayerInfoNum < AllPlayerInfo.Num()) { // 추가(플레이어가 들어올 때)
		AddPlayerInfoNum = AllPlayerInfo.Num();
		Ctrl->PlayerListAdd(AllPlayerInfo[AddPlayerInfoNum - 1]); // 0812문제 ?Ctrl->GetSettingID() - 1
		/*UE_LOG(LogTemp, Warning, TEXT("PlayerAdd: %s"), *(AllPlayerInfo[AddPlayerInfoNum - 1].playerName));
		UE_LOG(LogTemp, Warning, TEXT("PlayerAdd1: %s"), *(AllPlayerInfo[0].playerName));
		UE_LOG(LogTemp, Warning, TEXT("PlayerAdd2: %s"), *(AllPlayerInfo[1].playerName));
		UE_LOG(LogTemp, Warning, TEXT("PlayerAdd3: %d"), AddPlayerInfoNum - 1);
		UE_LOG(LogTemp, Warning, TEXT("PlayerAdd4: %d"), Ctrl->GetSettingID() - 1);*/
	}
	else if (AddPlayerInfoNum > AllPlayerInfo.Num()) { // 삭제(플레이어가 나갈 때)
		UE_LOG(LogTemp, Warning, TEXT("PlayerDel"));
		AddPlayerInfoNum = AllPlayerInfo.Num();
	}
	

}

// 호스트에 의해 게임 세팅이 변경되고 클라이언트에 업데이트 될 때 작동되는 함수입니다.
void ALobby_GameState::OnRep_GameSettingChange()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_GameSettingChange"));
	ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Ctrl->ChangeGameSetting();
}

// 나갈 떄의 처리를 합니다.
void ALobby_GameState::OnRep_DelPlayerChange()
{
	ALobby_PlayerController* Ctrl = Cast<ALobby_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Ctrl->DelPlayerInfo(DelPlayer);
}

void ALobby_GameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobby_GameState, AllPlayerController);
	DOREPLIFETIME(ALobby_GameState, AllPlayerInfo);
	DOREPLIFETIME(ALobby_GameState, AllPlayerStart);
	DOREPLIFETIME(ALobby_GameState, ServerName);
	DOREPLIFETIME(ALobby_GameState, MaxPlayers);
	DOREPLIFETIME(ALobby_GameState, CurrentPlayers);
	//DOREPLIFETIME(ALobby_GameState, AddPlayerInfoNum);
	DOREPLIFETIME(ALobby_GameState, GameSetting);
	DOREPLIFETIME(ALobby_GameState, DelPlayer);

}