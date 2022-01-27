// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/PlayerUpperStateBase.h"
#include "Components/TimelineComponent.h"
#include "Armed_PlayerUpper.generated.h"

/**���� ����
 * 
 */
UCLASS()
class DALOS_API UArmed_PlayerUpper : public UPlayerUpperStateBase
{
	GENERATED_BODY()
	
public:
	UArmed_PlayerUpper();

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

private:
	UPROPERTY(EditAnywhere)
	UCurveFloat* ADSCurve; // ���� ���ؿ��� �������� ���ư� �� ī�޶� ������ �� �ƿ��� �˴ϴ�. �� �ƿ��� �ϱ����� ������ �ϴµ� �� �� Ŀ�긦 ����մϴ�.
	FTimeline ADSTimeline; // �ð� ���� �� �ƿ��� �մϴ�.
	UFUNCTION()
	void SetUnADS();       // �� �ƿ� ���� �� �� �۵��Ǵ� �Լ�
	UFUNCTION()
	void SetUnADSFinish(); // �� �ƿ��� ������ �۵��Ǵ� �Լ�

	class UCameraComponent* PlayerCamera;

};
