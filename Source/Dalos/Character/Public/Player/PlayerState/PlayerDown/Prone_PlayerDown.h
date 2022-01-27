// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Prone_PlayerDown.generated.h"

/**���帮�� ����
 * 
 */
UCLASS()
class DALOS_API UProne_PlayerDown : public UPlayerDownStateBase
{
	GENERATED_BODY()

public:
	UProne_PlayerDown();

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player) override;  // �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press) override;          // ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
	virtual void StateStart(class AMultiPlayerBase* Player) override;                    // ���°� ���� �� �� �۵��ϴ� �Լ�
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                   // Tick���� �۵��ϴ� �Լ�
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                      // ���°� ���� �� �۵��ϴ� �Լ�
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed) override; // ���°� �ٲ�� ������ �� �۵��ϴ� �Լ�
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player) override;             // ���°� �ٲ�� �� Tick���� �۵��ϴ� �Լ�
	virtual UClass* GetState() override;                                                 // ������ UClass ��ȯ

	// ���콺 ���� �����ӿ� ���� ������� �� ��ü ȸ��(Z��) ���� ���մϴ�.
	virtual void TurnAtRate(class AMultiPlayerBase* Player, float Rate) override;
	// ������� �� ���콺 ���� ������(��, ��)�� ���� ������ �Ѱ����������� ī�޶� �����̰� �մϴ�.
	virtual void LookUpAtRate(class AMultiPlayerBase* Player, float Rate) override;

protected:
	FRotator ProneActorRot = FRotator::ZeroRotator;
	float ProneYaw = 0.0f; // ������� �� ��ü ȸ��(Z��) ��
	bool IsTurn = false;   // ���帮�⿡�� �����ֱ� or �����ֱ⿡�� ���帮��� �ٲ�ߵǴ����� ��Ÿ���ϴ�.
	int ProneBack = 0;     // �����ִ� ���������� �˷��ݴϴ�.

	class UBoxComponent* Box = nullptr;
	FRotator BoxRot = FRotator::ZeroRotator;

	float CapsuleRadius = 0.0f;
	float CapsuleHalfHeight = 0.0f;
	float MeshZLoc = 0.0f;

	float CurrentCapsuleRadius = 0.0f;
	float CurrentCapsuleHalfHeight = 0.0f;
	float CurrentMeshZLoc = 0.0f;

private:


};
