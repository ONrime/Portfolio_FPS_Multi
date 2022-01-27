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

// ���� �������� ���� ü���� ����մϴ�.
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

// �ڵ� ȸ�� ���� (����� ���� �ʽ��ϴ�. ����� �ϼ� ������ �뷱���� �� �´� �Ǵ��ؼ� �� �����Դϴ�.)
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

// ü�� ȸ�� ���� �� �� �۵��ϴ� �Լ��Դϴ�.
void ATwoVersus_PlayerState::SetHeal()
{
	if (PlayerHP < 90.0f) {
		PlayerHP = FMath::Lerp(PlayerHP, 90.0f, HealTimeline.GetPlaybackPosition());
		//UE_LOG(LogTemp, Warning, TEXT("HealTimeline: %f"), HealTimeline.GetPlaybackPosition());
		//UE_LOG(LogTemp, Warning, TEXT("SetHeal_playerHP: %f"), PlayerHP);
	}
}

// ü�� ȸ�� ���� �� �۵��ϴ� �Լ��Դϴ�.
void ATwoVersus_PlayerState::SetHealFinish()
{
	//UE_LOG(LogTemp, Warning, TEXT("HealFinish: %f"), PlayerHP);
}

// ü���� ������Ʈ �Ǹ� �÷��̾�(��Ʈ�ѷ��� ���� == ��)�� ������Ʈ�ϴ� �Լ��Դϴ�.
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
