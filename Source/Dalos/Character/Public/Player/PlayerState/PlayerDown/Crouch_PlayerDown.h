// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Crouch_PlayerDown.generated.h"

/**앉기 상태
 * 
 */
UCLASS()
class DALOS_API UCrouch_PlayerDown : public UPlayerDownStateBase
{
	GENERATED_BODY()
	
public:
	UCrouch_PlayerDown();

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player) override;  // 입력에 따른 상태 변환값을 반환하는 함수
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press) override;          // 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
	virtual void StateStart(class AMultiPlayerBase* Player) override;                    // 상태가 시작 할 때 작동하는 함수
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                   // Tick에서 작동하는 함수
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                      // 상태가 끝날 때 작동하는 함수
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed) override; // 상태가 바뀌기 시작할 때 작동하는 함수
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player) override;             // 상태가 바뀌는 중 Tick에서 작동하는 함수
	virtual UClass* GetState() override;												 // 상태의 UClass 반환

	// 앉기 상태에서 점프키를 누르면 일어서기 상태 객체를 반환합니다.
	virtual UPlayerDownStateBase* PlayerJump(class AMultiPlayerBase* Player) override;

protected:

private:

};
