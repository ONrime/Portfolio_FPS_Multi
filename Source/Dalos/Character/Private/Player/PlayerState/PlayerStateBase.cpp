// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerState/PlayerStateBase.h"

UPlayerStateBase::UPlayerStateBase()
{
	PrimaryComponentTick.bCanEverTick = false; // Tick �۵� ���� X
}

void UPlayerStateBase::StateStart(AMultiPlayerBase* Player)
{
}
void UPlayerStateBase::StateUpdate(AMultiPlayerBase* Player)
{
}
void UPlayerStateBase::StateEnd(AMultiPlayerBase* Player)
{
}
void UPlayerStateBase::StateChangeStart(AMultiPlayerBase* Player, float Speed)
{
}
void UPlayerStateBase::StateChangeUpdate(AMultiPlayerBase* Player)
{
}

UClass* UPlayerStateBase::GetState()
{
	return nullptr;
}
