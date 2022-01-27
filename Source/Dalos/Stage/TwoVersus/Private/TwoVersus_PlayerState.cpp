// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerState.h"
#include "Dalos/Character/Public/Player/MultiPlayerBase.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_GameMode.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

ATwoVersus_PlayerState::ATwoVersus_PlayerState() 
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerHP = 100.0f;

	static ConstructorHelpers::FObjectFinder<UCurveFloat>HEAL_CURVE(TEXT("CurveFloat'/Game/Curve/HealChange.HealChange'"));
	if (HEAL_CURVE.Succeeded()) HealCurve = HEAL_CURVE.Object;
}

// 들어온 데미지에 따라 체력을 계산합니다.
void ATwoVersus_PlayerState::DamageToHP(float damage)
{
	PlayerHP = PlayerHP - damage;
	if (PlayerHP < 0.0f) PlayerHP = 0.0f;
}

void ATwoVersus_PlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ATwoVersus_PlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATwoVersus_PlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HealTimeline.TickTimeline(DeltaTime);
	//if(HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("playerHP1: %f"), PlayerHP);
}

// 자동 회복 관련 (현재는 쓰지 않습니다. 기능은 완성 하지만 밸런스에 안 맞다 판단해서 뺀 상태입니다.)
void ATwoVersus_PlayerState::StartHeal()
{
	if (HealCurve != nullptr) 
	{
		UE_LOG(LogTemp, Warning, TEXT("StartHeal"));
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, "SetHeal");
		FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, "SetHealFinish");
		HealTimeline.AddInterpFloat(HealCurve, TimelineCallback);
		HealTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
		HealTimeline.SetPlayRate(0.05f);
		HealTimeline.PlayFromStart();
	}
}
void ATwoVersus_PlayerState::StopHeal()
{
	HealTimeline.Stop();
}

// 체력 회복 시작 할 때 작동하는 함수입니다.
void ATwoVersus_PlayerState::SetHeal()
{
	if (PlayerHP < 90.0f) {
		PlayerHP = FMath::Lerp(PlayerHP, 90.0f, HealTimeline.GetPlaybackPosition());
		//UE_LOG(LogTemp, Warning, TEXT("HealTimeline: %f"), HealTimeline.GetPlaybackPosition());
		//UE_LOG(LogTemp, Warning, TEXT("SetHeal_playerHP: %f"), PlayerHP);
	}
}

// 체력 회복 끝날 때 작동하는 함수입니다.
void ATwoVersus_PlayerState::SetHealFinish()
{
	//UE_LOG(LogTemp, Warning, TEXT("HealFinish: %f"), PlayerHP);
}

// 체력이 업데이트 되면 플레이어(컨트롤러가 없는 == 적)에 업데이트하는 함수입니다.
void ATwoVersus_PlayerState::OnRep_UpdatePlayerHP()
{
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_UpdatePlayerHP"));
	Player = Cast<AMultiPlayerBase>(GetPawn());
	if (Player) Player->CheckPlayerHP(PlayerHP);
}

void ATwoVersus_PlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATwoVersus_PlayerState, PlayerHP);
	DOREPLIFETIME(ATwoVersus_PlayerState, TeamName);

}
