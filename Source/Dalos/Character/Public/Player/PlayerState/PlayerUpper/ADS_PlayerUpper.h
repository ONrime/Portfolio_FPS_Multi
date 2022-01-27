// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/PlayerUpperStateBase.h"
#include "Components/TimelineComponent.h"
#include "ADS_PlayerUpper.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API UADS_PlayerUpper : public UPlayerUpperStateBase
{
	GENERATED_BODY()
	
public:
	UADS_PlayerUpper();

	virtual UPlayerUpperStateBase* HandleInput(class AMultiPlayerBase* Player) override; // �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
	virtual UPlayerUpperStateBase* SendHandleInput(EPlayerPress Press) override;         // ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
	virtual void StateStart(class AMultiPlayerBase* Player) override;                    // ���°� ���� �� �� �۵��ϴ� �Լ�
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                   // Tick���� �۵��ϴ� �Լ�
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                      // ���°� ���� �� �۵��ϴ� �Լ�
	virtual UClass* GetState() override;												 // ������ UClass ��ȯ

	// ��� (����� �ܹ�, ����, 3���� ������ �����ϰ��ֽ��ϴ�.)
	virtual void PlayerFire(class AMultiPlayerBase* Player, class AWeaponeBase* Equip, bool& IsAuto, float& Count, FVector Loc, FRotator Rot, FVector BulletLoc) override;
	virtual bool HandUpTracer(class AMultiPlayerBase* Player) override;  // ��ֹ��� ������ ���� �÷��� �ϴ��� �˷��ִ� �Լ�

protected:
	/*���� ������ �ϱ����� ����, �Ÿ�(�÷��̾�� ���� �𼭸����� �Ÿ�)�� ��ȯ�ϴ� �Լ��Դϴ�.
	* Trace�� ���۵Ǵ� ����(Start)�� ����(Dir)�� �˷��ָ� �� ������ Trace�� ���� ������ �Ÿ��� ���մϴ�.
	*/
	bool CoverTracer(class AMultiPlayerBase* Player, FVector Start, float& Angle, float& Distance, float Dir);
	// �׽�Ʈ�� (�ɾ��� �� ���� ���������� �ϴ� ���, �ϼ� X)
	bool CoverUpTracer(class AMultiPlayerBase* Player, FVector Start);

private:
	UPROPERTY(EditAnywhere)
	UCurveFloat* ADSCurve; // ī�޶� ���� ������ �� �� ����ϴ� Ŀ���Դϴ�. 
	FTimeline ADSTimeline; // �ð� ���� �� ���� �մϴ�.
	UFUNCTION()
	void SetADS();		   // ������ ���� �Ҷ� ����Ǵ� �Լ��Դϴ�.
	UFUNCTION()
	void SetADSFinish();   // ������ ���� �� ����Ǵ� �Լ��Դϴ�.

	class UCameraComponent* PlayerCamera = nullptr;
	FRotator CoverRot = FRotator::ZeroRotator; // ���� ���� �� ���� ���� Rotation
};
