// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerStateBase.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/PlayerUpperStateBase.h"
#include "Dalos/Character/Public/Player/MultiPlayerBase.h"
#include "PlayerDownStateBase.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API UPlayerDownStateBase : public UPlayerStateBase
{
	GENERATED_BODY()
	
public:
	UPlayerDownStateBase();

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player); // ��ư �Է¿� ���� ���� ��ȯ
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press);         // ��ư �Է¿� ���� ���� ��ȯ(��Ƽ�÷��̿�) 
	/*��ư �Է¿� ���� ���� ��ȯ(��Ƽ�÷��̿�)
	* �ٸ� Ŭ���̾�Ʈ�� �Է°��� ������ ���ϱ� ������(Controller�� Replicated�� �ȵǱ� �����̴�.)
	* enum���� �Ѱܼ� �ٸ� Ŭ���̾�Ʈ���˾������� ����� ���� ���� �Լ��Դϴ�.
	*/

	virtual UPlayerDownStateBase* ChangeState(EPlayerDownState State);         // ���� ���� ��ȯ(���ǿ� ���� ��ȯ�� �� �� ����Ѵ�.)
	virtual void StateStart(class AMultiPlayerBase* Player) override;          // �� ���°� ������ �� �۵��Ǵ� �Լ��̴�.
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;         // �� ���¸��� Tick���� �۵��Ǵ� �Լ��̴�.
	virtual void StateEnd(class AMultiPlayerBase* Player) override;            // �� ���°� �������� �ؾ� �Ҷ� �۵��Ǵ� �Լ��̴�.
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed) override;    // �� ���°� �ٲ�� ������ �� �۵��Ǵ� �Լ��̴�.(Tick���� �۵�)
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player) override;   // �� ���°� �ٲ� �۵��Ǵ� �Լ��̴�.(Tick���� �۵�)
	virtual UClass* GetState() override;                                       // ������ StaticClass�� ��ȯ�Ѵ�.

	virtual void TurnAtRate(class AMultiPlayerBase* Player, float Rate);
	virtual void LookUpAtRate(class AMultiPlayerBase* Player, float Rate);
	virtual UPlayerDownStateBase* PlayerJump(class AMultiPlayerBase* Player);
	virtual void PlayerMove(class AMultiPlayerBase* Player, float InputValue, float InputDirRight);
	virtual void AnimInsUpdate(class AMultiPlayerBase* Player);

	EPlayerDownState GetEState() { return DownState; }
	float GetPelvisLocZ() { return PelvisLocZ; }

protected:
	UPlayerDownStateBase* temp;
	EPlayerDownState DownState;
	float PelvisLocZ = 0.0f;

private:

};
