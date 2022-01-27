// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Lobby_GameMode.generated.h"

/**�κ� ���� ���
 * �κ� ���õ� ��Ģ ���� (Ÿ�̸�)
 * ��ġ ���۽� ��ġ�� ���ο� ���� �ε��ϱ� ������ �Ų����� ���� �̵��� �մϴ�.
 */
UCLASS()
class DALOS_API ALobby_GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ALobby_GameMode();

	/* �α��� ���� �̺�Ʈ�� �ߵ��ϴ� �Լ��Դϴ�.
	* �α��ε� ��Ʈ�ѷ����� ���� �� ���¸� �����մϴ�.
	* GameState�� ������ ���� ������ �α��ε� ��Ʈ�ѷ��� �����մϴ�.
	*/
	// �α��� ���� �̺�Ʈ�� �ߵ��ϴ� �Լ��Դϴ�. �÷��̾� ��Ʈ�ѷ����� �κ� �����ϰ� �÷��̾� ����
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// ��ġ ���� �� �۵��Ǵ� ī��Ʈ �ٿ� ���߱�
	void StopCountDown();
	// ��ġ ���� �� �۵��Ǵ� ī��Ʈ �ٿ� �����ϱ�
	void StartCountDown();

protected:

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	int RedTeamCount = 0;  // ���� �α��ε� ���� �� �ο� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	int BlueTeamCount = 0; // ���� �α��ε� ��� �� �ο� ��
	bool IsHost = false;   // �� �κ� ȣ��Ʈ�� �����ϴ����� �˷��ִ� �����Դϴ�.
	int GiveID = 0;        // ��Ʈ�ѷ��� �ο��Ǵ� �⺻ID (�� ID�� �÷��̾ �����մϴ�.)

	// ��ġ ���� �� �۵��Ǵ� ī��Ʈ �ٿ� Ÿ�̸�
	FTimerHandle CountDownTimer;
	void MinCount();  // ī��Ʈ �ٿ� �Լ��Դϴ�.
	int CountNum = 5; // ī��Ʈ�� ���� �����Դϴ�.

};
