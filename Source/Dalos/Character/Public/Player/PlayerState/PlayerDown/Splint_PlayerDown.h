// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Splint_PlayerDown.generated.h"

/**�޸� �� ����
 * 
 */
UCLASS()
class DALOS_API USplint_PlayerDown : public UPlayerDownStateBase
{
	GENERATED_BODY()
	
public:
	USplint_PlayerDown();

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player) override; // �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press) override;         // ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
	virtual UPlayerDownStateBase* ChangeState(EPlayerDownState State) override;         // ���� ���� ��ȯ(���ǿ� ���� ��ȯ�� �� �� ����մϴ�.)
	virtual void StateStart(class AMultiPlayerBase* Player) override;                   // ���°� ���� �� �� �۵��ϴ� �Լ�
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                  // Tick���� �۵��ϴ� �Լ�
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                     // ���°� ���� �� �۵��ϴ� �Լ�
	virtual UClass* GetState() override;                                                // ������ UClass ��ȯ

	// ���콺 ���� �����ӿ� ���� ī�޶��� ȸ��(X��)�� ���մϴ�.
	virtual void TurnAtRate(class AMultiPlayerBase* Player, float Rate) override;

protected:

	class UCameraShakeBase* SplintShake; // �޸� �� ī�޶� ����ũ

private:

	float CameraTurnChagneRoll = 0.0f; // ������ ���� ī�޶� X��(��, ��)ȸ�� ��

};
