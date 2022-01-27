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

// �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
UPlayerUpperStateBase* UUnArmed_PlayerUpper::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // �÷��̾� ��Ʈ�ѷ� �Դϴ�.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // ��Ʈ�ѷ��� �Է��� �����ϴ� ��ü�Դϴ�.

	// �� ������ Ű�� �����ɴϴ�.
	TArray <FInputActionKeyMapping> ActionFirstGun = PlayerInput->GetKeysForAction(TEXT("FirstGun"));       // ù�� ° ���� Ű
	TArray <FInputActionKeyMapping> ActionSecondGun = PlayerInput->GetKeysForAction(TEXT("SecondGun"));     // �ι� ° ���� Ű
	TArray <FInputActionKeyMapping> ActionInteraction = PlayerInput->GetKeysForAction(TEXT("Interaction")); // ��ȣ�ۿ� Ű

	if (PlayerInput->IsPressed(ActionFirstGun[0].Key) || PlayerInput->IsPressed(ActionSecondGun[0].Key)
		|| PlayerInput->IsPressed(ActionInteraction[0].Key)) 
	{
		temp = NewObject<UArmed_PlayerUpper>(this, UArmed_PlayerUpper::StaticClass()); // ���� ���� ��ü ����
	}

	return temp;
}

// ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
UPlayerUpperStateBase* UUnArmed_PlayerUpper::SendHandleInput(EPlayerPress Press)
{
	if (Press == EPlayerPress::FIRSTGUN || Press == EPlayerPress::SECONDGUN
		|| Press == EPlayerPress::INTERACTION)
	{
		temp = NewObject<UArmed_PlayerUpper>(this, UArmed_PlayerUpper::StaticClass());
	}
	return temp;
}

// ���°� ���� �� �� �۵��ϴ� �Լ�
void UUnArmed_PlayerUpper::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	//UE_LOG(LogTemp, Warning, TEXT("UnArmed: StateStart"));
	AActor* EquipArm = Cast<AActor>(Player->GetEquipWeaponeArm());
	if (Player->IsLocallyControlled() && EquipArm)
	{
		EquipArm->SetActorHiddenInGame(true); // ����ִ� ���� ������
	}
}

// Tick���� �۵��ϴ� �Լ�
void UUnArmed_PlayerUpper::StateUpdate(AMultiPlayerBase* Player)
{
	//UE_LOG(LogTemp, Warning, TEXT("UnArmed: StateUpdate"));
}

// ���°� ���� �� �۵��ϴ� �Լ�
void UUnArmed_PlayerUpper::StateEnd(AMultiPlayerBase* Player)
{
	//UE_LOG(LogTemp, Warning, TEXT("UnArmed: StateEnd"));
	AActor* EquipArm = Cast<AActor>(Player->GetEquipWeaponeArm());
	if (Player->IsLocallyControlled() && EquipArm)
	{
		EquipArm->SetActorHiddenInGame(false); // �� ���� ���°� ������ ��� �ִ� ���� ���̱�
	}
}

// ������ UClass ��ȯ
UClass* UUnArmed_PlayerUpper::GetState()
{
	return UUnArmed_PlayerUpper::StaticClass();
}
