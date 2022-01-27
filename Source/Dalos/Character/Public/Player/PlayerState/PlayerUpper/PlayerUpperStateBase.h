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

	virtual UPlayerUpperStateBase* HandleInput(class AMultiPlayerBase* Player); // 버튼 입력에 따른 상태 변환
	virtual UPlayerUpperStateBase* SendHandleInput(EPlayerPress press);         // 버튼 입력에 따른 상태 변환(멀티플레이용)    
	/*버튼 입력에 따른 상태 변환(멀티플레이용)
	* 다른 클라이언트에 입력값을 전달을 못하기 떄문에(Controller는 Replicated가 안되기 떄문이다.)
	* enum값을 넘겨서 다른 클라이언트가알아차리게 만들기 위해 만든 함수입니다.
	*/

	virtual UPlayerUpperStateBase* ChangeState(EPlayerUpperState State);        // 강제 상태 변환(조건에 따른 변환을 할 때 사용한다.)
	virtual void StateStart(class AMultiPlayerBase* Player) override;           // 각 상태가 시작할 때 작동되는 함수이다.
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;          // 각 상태마다 Tick에서 작동되는 함수이다.
	virtual void StateEnd(class AMultiPlayerBase* Player) override;             // 각 상태가 마무리를 해야 할때 작동되는 함수이다.
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed) override;    // 각 상태가 바뀌기 시작할 때 작동되는 함수이다.(Tick에서 작동)
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player) override;    // 각 상태가 바뀔때 작동되는 함수이다.(Tick에서 작동)
	virtual UClass* GetState() override;                                        // 상태의 StaticClass를 반환한다.
	
	// 사격
	virtual void PlayerFire(class AMultiPlayerBase* Player, class AWeaponeBase* Equip, bool& IsAuto, float& Count, FVector Loc, FRotator Rot, FVector BulletLoc);
	virtual bool HandUpTracer(class AMultiPlayerBase* Player); // 장애물을 만나면 손을 올려야 하는지 알려주는 함수

	EPlayerUpperState GetEState() { return UpperState; }
	float GetCoverLocZ() { return CoverLocZ; }

protected:
	UPlayerUpperStateBase* temp;
	EPlayerUpperState UpperState; // 상체 상태 (Enum) 
	float CoverLocZ = 0.0f;       // 엄폐 조준 할때의 카메라 위치

private:

	
};
