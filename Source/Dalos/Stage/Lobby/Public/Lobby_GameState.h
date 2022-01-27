// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "Lobby_GameState.generated.h"

/** ����(�κ�)�� ��ü ����
 * 
 */
UCLASS()
class DALOS_API ALobby_GameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ALobby_GameState();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings)
	TArray<class APlayerController*> AllPlayerController; // ����� �κ� �����ϴ� ��� ��Ʈ�ѷ�
	/* ���� �κ� �����ϴ� ��� �÷��̾��� �⺻ ����
	* �⺻ID, �� ����, �� ����, �غ� ���µ��� �⺻ ������ ������ �ֽ��ϴ�.
	*/
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_AllPlayerInfoChange, BlueprintReadWrite, Category = ServerSettings)
	TArray<FPlayerInfo> AllPlayerInfo;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings)
	TArray<class AActor*> AllPlayerStart;				  // ��� �÷��̾� ��ŸƮ ����
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings)
	FString ServerName;									  // �� ������ �̸�
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings)
	int MaxPlayers;										  // �� ������ �ο��� �� �ִ� �ο�
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings)
	int CurrentPlayers = 0;								  // ���� �κ� ���� �÷��̾� �ο�
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_GameSettingChange, BlueprintReadWrite, Category = ServerSettings)
	FGameSetting GameSetting;							  // ���� ����
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_DelPlayerChange, BlueprintReadWrite, Category = ServerSettings)
	FPlayerInfo DelPlayer;								  // ������ �÷��̾�

	/* �÷��̾��� ������ �ٲ�ٸ� �۵��Ǵ� �Լ��Դϴ�.
	* ��Ȯ���� ���� ���³�, �غ� ���°� �ٲ� �� �۵��Ǿ� �ٸ� Ŭ���̾�Ʈ���� �����ϴ� �Լ��Դϴ�.
	*/
	UFUNCTION(NetMultiCast, Reliable, WithValidation)
	void NetMultiCast_ChangePlayerInfo(int Index, int Category, const FString& Change);
	bool NetMultiCast_ChangePlayerInfo_Validate(int Index, int Category, const FString& Change);
	void NetMultiCast_ChangePlayerInfo_Implementation(int Index, int Category, const FString& Change);
	
	/* AllPlayerInfo�� �ٲ� �� �۵�
	* �α��� �Ǿ��� �� �۵��Ǹ� ó�� ���� ��, �߰��� ���� ��, ���� ���� ó��(��ü �ο��� ���� ���϶���)�� �մϴ�.
	*/
	UFUNCTION()
	void OnRep_AllPlayerInfoChange();
	UFUNCTION()
	void OnRep_GameSettingChange(); // ȣ��Ʈ�� ���� ���� ������ ����ǰ� Ŭ���̾�Ʈ�� ������Ʈ �� �� �۵��Ǵ� �Լ��Դϴ�.
	UFUNCTION()
	void OnRep_DelPlayerChange();   // ���� ���� ó���� �մϴ�.
	
	//UPROPERTY(Replicated)
	int AddPlayerInfoNum = 0;


protected:

private:

};
