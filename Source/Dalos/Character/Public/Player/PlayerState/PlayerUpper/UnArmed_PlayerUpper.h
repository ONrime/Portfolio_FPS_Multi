// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/PlayerUpperStateBase.h"
#include "UnArmed_PlayerUpper.generated.h"

/**비 무장 상태
 * 
 */
UCLASS()
class DALOS_API UUnArmed_PlayerUpper : public UPlayerUpperStateBase
{
	GENERATED_BODY()
	
public:
	UUnArmed_PlayerUpper();

	virtual UPlayerUpperStateBase* HandleInput(class AMultiPlayerBase* Player) override; // 입력에 따른 상태 변환값을 반환하는 함수
	virtual UPlayerUpperStateBase* SendHandleInput(EPlayerPress Press) override;         // 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
	virtual void StateStart(class AMultiPlayerBase* Player) override;                    // 상태가 시작 할 때 작동하는 함수
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                   // Tick에서 작동하는 함수
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                      // 상태가 끝날 때 작동하는 함수
	virtual UClass* GetState() override;												 // 상태의 UClass 반환

protected:

private:


};
