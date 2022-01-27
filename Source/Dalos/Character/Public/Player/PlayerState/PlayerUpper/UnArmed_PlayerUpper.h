// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/PlayerUpperStateBase.h"
#include "UnArmed_PlayerUpper.generated.h"

/**�� ���� ����
 * 
 */
UCLASS()
class DALOS_API UUnArmed_PlayerUpper : public UPlayerUpperStateBase
{
	GENERATED_BODY()
	
public:
	UUnArmed_PlayerUpper();

	virtual UPlayerUpperStateBase* HandleInput(class AMultiPlayerBase* Player) override; // �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
	virtual UPlayerUpperStateBase* SendHandleInput(EPlayerPress Press) override;         // ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
	virtual void StateStart(class AMultiPlayerBase* Player) override;                    // ���°� ���� �� �� �۵��ϴ� �Լ�
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                   // Tick���� �۵��ϴ� �Լ�
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                      // ���°� ���� �� �۵��ϴ� �Լ�
	virtual UClass* GetState() override;												 // ������ UClass ��ȯ

protected:

private:


};
