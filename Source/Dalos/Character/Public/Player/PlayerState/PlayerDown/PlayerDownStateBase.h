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

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player); // 버튼 입력에 따른 상태 변환
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press);         // 버튼 입력에 따른 상태 변환(멀티플레이용) 
	/*버튼 입력에 따른 상태 변환(멀티플레이용)
	* 다른 클라이언트에 입력값을 전달을 못하기 떄문에(Controller는 Replicated가 안되기 떄문이다.)
	* enum값을 넘겨서 다른 클라이언트가알아차리게 만들기 위해 만든 함수입니다.
	*/

	virtual UPlayerDownStateBase* ChangeState(EPlayerDownState State);         // 강제 상태 변환(조건에 따른 변환을 할 때 사용한다.)
	virtual void StateStart(class AMultiPlayerBase* Player) override;          // 각 상태가 시작할 때 작동되는 함수이다.
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;         // 각 상태마다 Tick에서 작동되는 함수이다.
	virtual void StateEnd(class AMultiPlayerBase* Player) override;            // 각 상태가 마무리를 해야 할때 작동되는 함수이다.
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed) override;    // 각 상태가 바뀌기 시작할 때 작동되는 함수이다.(Tick에서 작동)
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player) override;   // 각 상태가 바뀔때 작동되는 함수이다.(Tick에서 작동)
	virtual UClass* GetState() override;                                       // 상태의 StaticClass를 반환한다.

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
