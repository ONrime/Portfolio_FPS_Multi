// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Crouch_PlayerDown.generated.h"

/**�ɱ� ����
 * 
 */
UCLASS()
class DALOS_API UCrouch_PlayerDown : public UPlayerDownStateBase
{
	GENERATED_BODY()
	
public:
	UCrouch_PlayerDown();

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player) override;  // �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press) override;          // ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
	virtual void StateStart(class AMultiPlayerBase* Player) override;                    // ���°� ���� �� �� �۵��ϴ� �Լ�
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                   // Tick���� �۵��ϴ� �Լ�
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                      // ���°� ���� �� �۵��ϴ� �Լ�
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed) override; // ���°� �ٲ�� ������ �� �۵��ϴ� �Լ�
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player) override;             // ���°� �ٲ�� �� Tick���� �۵��ϴ� �Լ�
	virtual UClass* GetState() override;												 // ������ UClass ��ȯ

	// �ɱ� ���¿��� ����Ű�� ������ �Ͼ�� ���� ��ü�� ��ȯ�մϴ�.
	virtual UPlayerDownStateBase* PlayerJump(class AMultiPlayerBase* Player) override;

protected:

private:

};
