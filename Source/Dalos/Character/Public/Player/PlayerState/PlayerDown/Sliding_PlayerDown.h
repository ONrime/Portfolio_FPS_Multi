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

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player) override; // 입력에 따른 상태 변환값을 반환하는 함수
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press) override;         // 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
	virtual UPlayerDownStateBase* ChangeState(EPlayerDownState State) override;         // 강제 상태 변환(조건에 따른 변환을 할 때 사용합니다.)
	virtual void StateStart(class AMultiPlayerBase* Player) override;                   // 상태가 시작 할 때 작동하는 함수
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                  // Tick에서 작동하는 함수
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                     // 상태가 끝날 때 작동하는 함수
	virtual UClass* GetState() override;                                                // 상태의 UClass 반환

private:
	float GetSlidingAngle(FVector Loc, FVector Dir, FVector& SlidingDir); // 바닥에 Trace를 쏴서 바닥의 따른 슬라이딩 각도를 구합니다.
	float SlidingAngle = 0.0f; // 슬라이딩을 할 때 바닥에 따른 각도
	float BaseForce = 3.0f;    // 슬라이딩의 각도에 따라 밀어지는 힘이 달라지고, 그 힘을 나타내는 변수입니다.
	FVector ForceDir = FVector::ZeroVector; // 슬라이딩되는 방향입니다.

};
