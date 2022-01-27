// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Standing_PlayerDown.generated.h"

/** 서있는 상태 (하체)
 * 
 */
UCLASS()
class DALOS_API UStanding_PlayerDown : public UPlayerDownStateBase
{
	GENERATED_BODY()
	
public:
	UStanding_PlayerDown();

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player) override;  // 입력에 따른 상태 변환값을 반환하는 함수
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press) override;          // 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
	virtual void StateStart(class AMultiPlayerBase* Player) override;                    // 상태가 시작 할 때 작동하는 함수
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                   // Tick에서 작동하는 함수
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                      // 상태가 끝날 때 작동하는 함수
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed) override; // 상태가 바뀌기 시작할 때 작동하는 함수
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player) override;             // 상태가 바뀌는 중 Tick에서 작동하는 함수
	virtual UClass* GetState() override;                                                 // 상태의 UClass 반환

	// 마우스 축의 움직임에 따라 카메라의 회전(X축)을 구합니다.
	virtual void TurnAtRate(class AMultiPlayerBase* Player, float Rate) override;      
	// 플레이어의 움직임에 따라 카메라의 회전(X축)을 구합니다.
	virtual void PlayerMove(class AMultiPlayerBase* Player, float InputValue, float InputDirRight) override;

protected:

	class UCameraShakeBase* WalkShake; // 걸을 때마다 작동되는 카메라 흔들림

	float CapsuleRadius = 0.0f;
	float CapsuleHalfHeight = 0.0f;
	float MeshZLoc = 0.0f;

	float CurrentCapsuleRadius = 0.0f;
	float CurrentCapsuleHalfHeight = 0.0f;
	float CurrentMeshZLoc = 0.0f;

private:
	float CameraMoveChagneRoll = 0.0f; // 움직임에 따라 카메라 X축(좌, 우)회전 값
	float CameraTurnChagneRoll = 0.0f; // 시점에 따라 카메라 X축(좌, 우)회전 값

};
