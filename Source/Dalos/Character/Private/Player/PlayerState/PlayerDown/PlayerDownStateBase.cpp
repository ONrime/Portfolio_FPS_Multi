// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"

UPlayerDownStateBase::UPlayerDownStateBase()
{
	PrimaryComponentTick.bCanEverTick = false; // Tick 작동 여부 X
	temp = nullptr;
	DownState = EPlayerDownState::STANDING;
}
UPlayerDownStateBase* UPlayerDownStateBase::HandleInput(AMultiPlayerBase* Player)
{
	return temp;
}

UPlayerDownStateBase* UPlayerDownStateBase::SendHandleInput(EPlayerPress Press)
{
	return temp;
}

UPlayerDownStateBase* UPlayerDownStateBase::ChangeState(EPlayerDownState State)
{
	return temp;
}

void UPlayerDownStateBase::StateStart(AMultiPlayerBase* Player)
{
	IsStart = true;
}
void UPlayerDownStateBase::StateUpdate(AMultiPlayerBase* Player)
{
}
void UPlayerDownStateBase::StateEnd(AMultiPlayerBase* Player)
{
}
void UPlayerDownStateBase::StateChangeStart(AMultiPlayerBase* Player, float Speed)
{
}

void UPlayerDownStateBase::StateChangeUpdate(AMultiPlayerBase* Player)
{
}

UClass* UPlayerDownStateBase::GetState()
{
	return nullptr;
}

void UPlayerDownStateBase::TurnAtRate(AMultiPlayerBase* Player, float Rate)
{
	if (Player->IsPlayerCameraTurn) {
		Player->SpringArm->bUsePawnControlRotation = true;
		float PitchRate = Rate * Player->BaseTurnRate * 0.008f;
		Player->AddControllerYawInput(PitchRate);
	}
	else { // 카메라 움직임 끄기
		//player->SpringArm->bUsePawnControlRotation = false;
	}
}

void UPlayerDownStateBase::LookUpAtRate(AMultiPlayerBase* Player, float Rate)
{
	if (Player->IsPlayerCameraTurn) {
		Player->SpringArm->bUsePawnControlRotation = true;
		float PitchRate = Rate * Player->BaseLookUpRate * 0.008f;
		Player->AddControllerPitchInput(PitchRate);
	}
	else { // 카메라 움직임 끄기
		//player->SpringArm->bUsePawnControlRotation = false;
	}
}

UPlayerDownStateBase* UPlayerDownStateBase::PlayerJump(AMultiPlayerBase* Player)
{
	Player->bPressedJump = true;
	Player->JumpKeyHoldTime = 0.0f;
	return nullptr;
}

void UPlayerDownStateBase::PlayerMove(AMultiPlayerBase* Player, float InputValue, float InputDirRight)
{

}

void UPlayerDownStateBase::AnimInsUpdate(AMultiPlayerBase* Player)
{
}
