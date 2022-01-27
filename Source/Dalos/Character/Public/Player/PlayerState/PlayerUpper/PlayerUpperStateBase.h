// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerStateBase.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Dalos/Character/Public/Player/MultiPlayerBase.h"
#include "PlayerUpperStateBase.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API UPlayerUpperStateBase : public UPlayerStateBase
{
	GENERATED_BODY()
	
public:
	UPlayerUpperStateBase();

	virtual UPlayerUpperStateBase* HandleInput(class AMultiPlayerBase* Player); // ��ư �Է¿� ���� ���� ��ȯ
	virtual UPlayerUpperStateBase* SendHandleInput(EPlayerPress press);         // ��ư �Է¿� ���� ���� ��ȯ(��Ƽ�÷��̿�)    
	/*��ư �Է¿� ���� ���� ��ȯ(��Ƽ�÷��̿�)
	* �ٸ� Ŭ���̾�Ʈ�� �Է°��� ������ ���ϱ� ������(Controller�� Replicated�� �ȵǱ� �����̴�.)
	* enum���� �Ѱܼ� �ٸ� Ŭ���̾�Ʈ���˾������� ����� ���� ���� �Լ��Դϴ�.
	*/

	virtual UPlayerUpperStateBase* ChangeState(EPlayerUpperState State);        // ���� ���� ��ȯ(���ǿ� ���� ��ȯ�� �� �� ����Ѵ�.)
	virtual void StateStart(class AMultiPlayerBase* Player) override;           // �� ���°� ������ �� �۵��Ǵ� �Լ��̴�.
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;          // �� ���¸��� Tick���� �۵��Ǵ� �Լ��̴�.
	virtual void StateEnd(class AMultiPlayerBase* Player) override;             // �� ���°� �������� �ؾ� �Ҷ� �۵��Ǵ� �Լ��̴�.
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed) override;    // �� ���°� �ٲ�� ������ �� �۵��Ǵ� �Լ��̴�.(Tick���� �۵�)
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player) override;    // �� ���°� �ٲ� �۵��Ǵ� �Լ��̴�.(Tick���� �۵�)
	virtual UClass* GetState() override;                                        // ������ StaticClass�� ��ȯ�Ѵ�.
	
	// ���
	virtual void PlayerFire(class AMultiPlayerBase* Player, class AWeaponeBase* Equip, bool& IsAuto, float& Count, FVector Loc, FRotator Rot, FVector BulletLoc);
	virtual bool HandUpTracer(class AMultiPlayerBase* Player); // ��ֹ��� ������ ���� �÷��� �ϴ��� �˷��ִ� �Լ�

	EPlayerUpperState GetEState() { return UpperState; }
	float GetCoverLocZ() { return CoverLocZ; }

protected:
	UPlayerUpperStateBase* temp;
	EPlayerUpperState UpperState; // ��ü ���� (Enum) 
	float CoverLocZ = 0.0f;       // ���� ���� �Ҷ��� ī�޶� ��ġ

private:

	
};
