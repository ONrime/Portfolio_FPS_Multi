// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Standing_PlayerDown.generated.h"

/** ���ִ� ���� (��ü)
 * 
 */
UCLASS()
class DALOS_API UStanding_PlayerDown : public UPlayerDownStateBase
{
	GENERATED_BODY()
	
public:
	UStanding_PlayerDown();

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player) override;  // �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press) override;          // ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
	virtual void StateStart(class AMultiPlayerBase* Player) override;                    // ���°� ���� �� �� �۵��ϴ� �Լ�
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                   // Tick���� �۵��ϴ� �Լ�
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                      // ���°� ���� �� �۵��ϴ� �Լ�
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed) override; // ���°� �ٲ�� ������ �� �۵��ϴ� �Լ�
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player) override;             // ���°� �ٲ�� �� Tick���� �۵��ϴ� �Լ�
	virtual UClass* GetState() override;                                                 // ������ UClass ��ȯ

	// ���콺 ���� �����ӿ� ���� ī�޶��� ȸ��(X��)�� ���մϴ�.
	virtual void TurnAtRate(class AMultiPlayerBase* Player, float Rate) override;      
	// �÷��̾��� �����ӿ� ���� ī�޶��� ȸ��(X��)�� ���մϴ�.
	virtual void PlayerMove(class AMultiPlayerBase* Player, float InputValue, float InputDirRight) override;

protected:

	class UCameraShakeBase* WalkShake; // ���� ������ �۵��Ǵ� ī�޶� ��鸲

	float CapsuleRadius = 0.0f;
	float CapsuleHalfHeight = 0.0f;
	float MeshZLoc = 0.0f;

	float CurrentCapsuleRadius = 0.0f;
	float CurrentCapsuleHalfHeight = 0.0f;
	float CurrentMeshZLoc = 0.0f;

private:
	float CameraMoveChagneRoll = 0.0f; // �����ӿ� ���� ī�޶� X��(��, ��)ȸ�� ��
	float CameraTurnChagneRoll = 0.0f; // ������ ���� ī�޶� X��(��, ��)ȸ�� ��

};
