// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Stage/TwoVersus/Public/TwoVersus_GameState.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerState.h"
#include "Net/UnrealNetwork.h"

ATwoVersus_GameState::ATwoVersus_GameState()
{

}

// PlayerState로 부터 각 플레이어의 체력을 가져와 팀 전체 체력을 구합니다.
void ATwoVersus_GameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ChangeTeamHPCheck.BindLambda([this]()->void
	{
		RedTeamHP = 0;
		BlueTeamHP = 0;
		for (int i = 0; i < PlayerArray.Num(); i++)
		{
			ATwoVersus_PlayerState* State = Cast<ATwoVersus_PlayerState>(PlayerArray[i]);
			if (State->TeamName == "Red")
			{
				RedTeamHP += State->GetPlyaerHP();
			}
			else
			{
				BlueTeamHP += State->GetPlyaerHP();
			}
		}
	});

}
void ATwoVersus_GameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATwoVersus_GameState, AllPlayerController);
	DOREPLIFETIME(ATwoVersus_GameState, AllPlayerStart);
	DOREPLIFETIME(ATwoVersus_GameState, RedTeamHP);
	DOREPLIFETIME(ATwoVersus_GameState, BlueTeamHP);

}