// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Splint_PlayerDown.generated.h"

/**달릴 때 상태
 * 
 */
UCLASS()
class DALOS_API USplint_PlayerDown : public UPlayerDownStateBase
{
	GENERATED_BODY()
	
public:
	USplint_PlayerDown();

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player) override; // 입력에 따른 상태 변환값을 반환하는 함수
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press) override;         // 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
	virtual UPlayerDownStateBase* ChangeState(EPlayerDownState State) override;         // 강제 상태 변환(조건에 따른 변환을 할 때 사용합니다.)
	virtual void StateStart(class AMultiPlayerBase* Player) override;                   // 상태가 시작 할 때 작동하는 함수
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                  // Tick에서 작동하는 함수
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                     // 상태가 끝날 때 작동하는 함수
	virtual UClass* GetState() override;                                                // 상태의 UClass 반환

	// 마우스 축의 움직임에 따라 카메라의 회전(X축)을 구합니다.
	virtual void TurnAtRate(class AMultiPlayerBase* Player, float Rate) override;

protected:

	class UCameraShakeBase* SplintShake; // 달릴 때 카메라 쉐이크

private:

	float CameraTurnChagneRoll = 0.0f; // 시점에 따라 카메라 X축(좌, 우)회전 값

};
