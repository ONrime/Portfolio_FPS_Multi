// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/PlayerUpperStateBase.h"
#include "Dalos/Weapone/Public/WeaponeBase.h"

UPlayerUpperStateBase::UPlayerUpperStateBase()
{
	PrimaryComponentTick.bCanEverTick = false; // Tick 작동 여부 X
	temp = nullptr;
	UpperState = EPlayerUpperState::UNARMED;
}
UPlayerUpperStateBase* UPlayerUpperStateBase::HandleInput(AMultiPlayerBase* Player)
{
	return temp;
}

UPlayerUpperStateBase* UPlayerUpperStateBase::SendHandleInput(EPlayerPress Player)
{
	return temp;
}

UPlayerUpperStateBase* UPlayerUpperStateBase::ChangeState(EPlayerUpperState State)
{
	return temp;
}

void UPlayerUpperStateBase::StateStart(AMultiPlayerBase* Player)
{
	IsStart = true;
}
void UPlayerUpperStateBase::StateUpdate(AMultiPlayerBase* Player)
{
}
void UPlayerUpperStateBase::StateEnd(AMultiPlayerBase* Player)
{
}

void UPlayerUpperStateBase::StateChangeStart(AMultiPlayerBase* Player, float Speed)
{
}

void UPlayerUpperStateBase::StateChangeUpdate(AMultiPlayerBase* Player)
{
}

UClass* UPlayerUpperStateBase::GetState()
{
	return nullptr;
}

void UPlayerUpperStateBase::PlayerFire(AMultiPlayerBase* Player, AWeaponeBase* Equip, bool& IsAuto, float& Count, FVector Loc, FRotator Rot, FVector BulletLoc)
{
}

bool UPlayerUpperStateBase::HandUpTracer(AMultiPlayerBase* Player)
{
	return false;
}


