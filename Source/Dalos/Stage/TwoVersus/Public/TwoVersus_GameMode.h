// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TwoVersus_GameMode.generated.h"

/** ���� ��Ģ (2 �� 2)
 * 5���� ī��Ʈ �� ������ ���۵Ǹ�
 * �־��� �ð� ���� �ٸ� ���� �����ϸ� �¸�
 * �ð��� �� ������ �� �� ���� ����ִٸ� �� ��ü ü���� ���� ���� ���� �¸�
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

	// �÷��̾ ���� �� �� �۵��Ǵ� �Լ��Դϴ�.
	// ��Ʈ�ѷ��� ��ŸƮ ������ ������ GameState�� ����մϴ�.

	virtual void RestartPlayer(AController* NewPlayer) override;

	// �̹� ��ġ�� 5�� ī��Ʈ �ٿ��� �����մϴ�.
	void CountBeginPlayer();

	/* �÷��̾ ���� ���¶�� ���� ���� ������
	* �� ��ü�� �׾������� Ȯ���Ͽ� ������ ������ ���� �����ϴ� �Լ��Դϴ�.
	*/
	void CountPlayerDead(FString Team);

	void CountWin();

	/* ��ü�� �÷��̾� �������� �߿��� ������ ���� �����ϰ� ��Ʈ�ѷ��� �ο��ϴ� �Լ��Դϴ�.
	* Ŭ���̾�Ʈ�� �÷��̾� ��Ʈ�ѷ��� �����Ǿ� ���� �� �� ĳ���Ͱ� ������ �ǵ��� ��������ϴ�.
	* Ŭ���̾�Ʈ�� ������ ���� �÷��̾ �����Ǿ�� �ϱ� ������ Ŭ���̾�Ʈ�� ��Ʋ�η��� �����Ǿ� ���� �ϱ� �� ���� ĳ���Ͱ� �������� �ʰ� �߽��ϴ�.
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
	// GameEnd������ ������ ������ �κ�� ���ư��� �ƴϸ� �ٸ� ��ġ�� ���������� �����˴ϴ�.
	void WinResultEnd();

	FTimerHandle StartCountTimer;
	/* ���� ī��Ʈ �ٿ��� �� ������ �ൿ�� �����մϴ�.
	* ��ġ ī��Ʈ �ٿ��� �����մϴ�.
	*/
	void StartCountEnd();

	FTimerHandle GameCountTimer;
	void GameCountEnd();

};
