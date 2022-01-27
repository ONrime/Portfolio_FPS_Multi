// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Prone_PlayerDown.generated.h"

/**엎드리기 상태
 * 
 */
UCLASS()
class DALOS_API UProne_PlayerDown : public UPlayerDownStateBase
{
	GENERATED_BODY()

public:
	UProne_PlayerDown();

	virtual UPlayerDownStateBase* HandleInput(class AMultiPlayerBase* Player) override;  // 입력에 따른 상태 변환값을 반환하는 함수
	virtual UPlayerDownStateBase* SendHandleInput(EPlayerPress Press) override;          // 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
	virtual void StateStart(class AMultiPlayerBase* Player) override;                    // 상태가 시작 할 때 작동하는 함수
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                   // Tick에서 작동하는 함수
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                      // 상태가 끝날 때 작동하는 함수
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed) override; // 상태가 바뀌기 시작할 때 작동하는 함수
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player) override;             // 상태가 바뀌는 중 Tick에서 작동하는 함수
	virtual UClass* GetState() override;                                                 // 상태의 UClass 반환

	// 마우스 축의 움직임에 따라 엎드렸을 때 상체 회전(Z축) 값을 구합니다.
	virtual void TurnAtRate(class AMultiPlayerBase* Player, float Rate) override;
	// 엎드렸을 때 마우스 축의 움직임(상, 하)에 따라 지정한 한계지점까지만 카메라가 움직이게 합니다.
	virtual void LookUpAtRate(class AMultiPlayerBase* Player, float Rate) override;

protected:
	FRotator ProneActorRot = FRotator::ZeroRotator;
	float ProneYaw = 0.0f; // 엎드렸을 때 상체 회전(Z축) 값
	bool IsTurn = false;   // 엎드리기에서 누워있기 or 누워있기에서 엎드리기로 바꿔야되는지를 나타냅니다.
	int ProneBack = 0;     // 누워있는 상태인지를 알려줍니다.

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
