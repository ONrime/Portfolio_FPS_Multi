// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Sliding_PlayerDown.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API USliding_PlayerDown : public UPlayerDownStateBase
{
	GENERATED_BODY()
	
public:
	USliding_PlayerDown();

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player) override; // �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press) override;         // ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
	virtual UPlayerDownStateBase* ChangeState(EPlayerDownState State) override;         // ���� ���� ��ȯ(���ǿ� ���� ��ȯ�� �� �� ����մϴ�.)
	virtual void StateStart(class AMultiPlayerBase* Player) override;                   // ���°� ���� �� �� �۵��ϴ� �Լ�
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                  // Tick���� �۵��ϴ� �Լ�
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                     // ���°� ���� �� �۵��ϴ� �Լ�
	virtual UClass* GetState() override;                                                // ������ UClass ��ȯ

private:
	float GetSlidingAngle(FVector Loc, FVector Dir, FVector& SlidingDir); // �ٴڿ� Trace�� ���� �ٴ��� ���� �����̵� ������ ���մϴ�.
	float SlidingAngle = 0.0f; // �����̵��� �� �� �ٴڿ� ���� ����
	float BaseForce = 3.0f;    // �����̵��� ������ ���� �о����� ���� �޶�����, �� ���� ��Ÿ���� �����Դϴ�.
	FVector ForceDir = FVector::ZeroVector; // �����̵��Ǵ� �����Դϴ�.

};
