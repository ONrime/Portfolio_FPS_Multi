// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/UnArmed_PlayerUpper.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/Armed_PlayerUpper.h"
#include "Dalos/Widget/Public/MultiPlayer_HUD.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"

UUnArmed_PlayerUpper::UUnArmed_PlayerUpper()
{
	temp = nullptr;
	UpperState = EPlayerUpperState::UNARMED;
}

// 입력에 따른 상태 변환값을 반환하는 함수
UPlayerUpperStateBase* UUnArmed_PlayerUpper::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // 플레이어 컨트롤러 입니다.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // 컨트롤러의 입력을 관리하는 객체입니다.

	// 각 지정된 키를 가져옵니다.
	TArray <FInputActionKeyMapping> ActionFirstGun = PlayerInput->GetKeysForAction(TEXT("FirstGun"));       // 첫번 째 무기 키
	TArray <FInputActionKeyMapping> ActionSecondGun = PlayerInput->GetKeysForAction(TEXT("SecondGun"));     // 두번 째 무기 키
	TArray <FInputActionKeyMapping> ActionInteraction = PlayerInput->GetKeysForAction(TEXT("Interaction")); // 상호작용 키

	if (PlayerInput->IsPressed(ActionFirstGun[0].Key) || PlayerInput->IsPressed(ActionSecondGun[0].Key)
		|| PlayerInput->IsPressed(ActionInteraction[0].Key)) 
	{
		temp = NewObject<UArmed_PlayerUpper>(this, UArmed_PlayerUpper::StaticClass()); // 무장 상태 객체 생성
	}

	return temp;
}

// 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
UPlayerUpperStateBase* UUnArmed_PlayerUpper::SendHandleInput(EPlayerPress Press)
{
	if (Press == EPlayerPress::FIRSTGUN || Press == EPlayerPress::SECONDGUN
		|| Press == EPlayerPress::INTERACTION)
	{
		temp = NewObject<UArmed_PlayerUpper>(this, UArmed_PlayerUpper::StaticClass());
	}
	return temp;
}

// 상태가 시작 할 때 작동하는 함수
void UUnArmed_PlayerUpper::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	//UE_LOG(LogTemp, Warning, TEXT("UnArmed: StateStart"));
	AActor* EquipArm = Cast<AActor>(Player->GetEquipWeaponeArm());
	if (Player->IsLocallyControlled() && EquipArm)
	{
		EquipArm->SetActorHiddenInGame(true); // 들고있는 무기 가리기
	}
}

// Tick에서 작동하는 함수
void UUnArmed_PlayerUpper::StateUpdate(AMultiPlayerBase* Player)
{
	//UE_LOG(LogTemp, Warning, TEXT("UnArmed: StateUpdate"));
}

// 상태가 끝날 때 작동하는 함수
void UUnArmed_PlayerUpper::StateEnd(AMultiPlayerBase* Player)
{
	//UE_LOG(LogTemp, Warning, TEXT("UnArmed: StateEnd"));
	AActor* EquipArm = Cast<AActor>(Player->GetEquipWeaponeArm());
	if (Player->IsLocallyControlled() && EquipArm)
	{
		EquipArm->SetActorHiddenInGame(false); // 비 무장 상태가 끝나면 들고 있는 무기 보이기
	}
}

// 상태의 UClass 반환
UClass* UUnArmed_PlayerUpper::GetState()
{
	return UUnArmed_PlayerUpper::StaticClass();
}
