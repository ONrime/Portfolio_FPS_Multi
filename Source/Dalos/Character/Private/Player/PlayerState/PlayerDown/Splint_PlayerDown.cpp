// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Splint_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Standing_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Sliding_PlayerDown.h"
#include "Dalos/Widget/Public/MultiPlayer_HUD.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Dalos/CameraShake/Public/Player_Splint_CameraShake.h"

USplint_PlayerDown::USplint_PlayerDown()
{
	temp = nullptr;
	DownState = EPlayerDownState::SPLINT;
	PelvisLocZ = 60.0f; // ��� ��ġ (FootIK����� ���� �ִ� �����Դϴ�.)
}

// �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
UPlayerDownStateBase* USplint_PlayerDown::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // �÷��̾� ��Ʈ�ѷ� �Դϴ�.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // ��Ʈ�ѷ��� �Է��� �����ϴ� ��ü�Դϴ�.

	// �� ������ Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionCrouch = PlayerInput->GetKeysForAction(TEXT("Crouch")); // �ɱ� Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionSplint = PlayerInput->GetKeysForAction(TEXT("Splint")); // �޸��� Ű�� �����ɴϴ�.

	if (PlayerInput->IsPressed(ActionCrouch[0].Key)) 
	{ // �ɱ� Ű�� �����ٸ�
		temp = NewObject<USliding_PlayerDown>(this, USliding_PlayerDown::StaticClass()); // �����̵� ���� ��ü�� ����ϴ�.
	}
	else if (PlayerInput->IsPressed(ActionSplint[0].Key)) 
	{ // �޸��� Ű�� �����ٸ�
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass()); // ���ֱ� ���� ��ü�� ����ϴ�.
	}
	return temp;
}

// �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ� (���� �� �ٸ� Ŭ���̾�Ʈ�� �����ϴ� �뵵�Դϴ�.)
UPlayerDownStateBase* USplint_PlayerDown::SendHandleInput(EPlayerPress Press)
{
	if (Press == EPlayerPress::CROUCH) 
	{
		temp = NewObject<USliding_PlayerDown>(this, USliding_PlayerDown::StaticClass()); // �����̵� ���� ��ü�� ����ϴ�.
	}
	else if (Press == EPlayerPress::SPLINT) 
	{
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass()); // ���ֱ� ���� ��ü�� ����ϴ�.
	}

	return temp;
}

// ���� ���� ��ȯ(���ǿ� ���� ��ȯ�� �� �� ����մϴ�.)
UPlayerDownStateBase* USplint_PlayerDown::ChangeState(EPlayerDownState State)
{
	// �޸��� �ִٸ� �ӵ��� �پ��� �޸��� ���¿��� ���ֱ� ���·� ������ �� ���˴ϴ�.
	if (State == EPlayerDownState::STANDING) 
	{
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass());
	}
	return temp;
}

// ���°� ���� �� �� �۵��ϴ� �Լ�
void USplint_PlayerDown::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	//UE_LOG(LogTemp, Warning, TEXT("Splint: StateStart"));
	Player->PlayerSpeed = 120.0f; // �÷��̾� �ӵ�
	Player->CameraPitch = 0.0f;   // �޸��� ���� ���� ���콺�� ��, �Ʒ��� ���� ī�޶� ��ġ ��ȯ ���� �մϴ�.
	if (Player->IsLocallyControlled()) 
	{ // �޸��� ���� ���� ���ڼ��� �����ϴ�,
		if(Player->GetHUD()->CrossHairHideCheck.IsBound()) Player->GetHUD()->CrossHairHideCheck.Execute(true);
	}
}

// Tick���� �۵��ϴ� �Լ�
void USplint_PlayerDown::StateUpdate(AMultiPlayerBase* Player)
{
	// ī�޶�
	 //  ī�޶� ȸ��
	if (Player->IsLocallyControlled()) 
	{ // ī�޶� ����ũ �۵�
		//SplintShake = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(UPlayer_Splint_CameraShake::StaticClass(), 1.0f);
		SplintShake = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(UPlayer_Splint_CameraShake::StaticClass(), 1.0f);
	}
	FRotator CurrentFollowCameraRot = Player->FollowCamera->GetRelativeRotation();
	Player->FollowCamera->SetRelativeRotation(FRotator(CurrentFollowCameraRot.Pitch, CurrentFollowCameraRot.Yaw
		, FMath::FInterpTo(CurrentFollowCameraRot.Roll, CameraTurnChagneRoll, GetWorld()->DeltaTimeSeconds, 5.0f)));

}

// ���°� ���� �� �۵��ϴ� �Լ�
void USplint_PlayerDown::StateEnd(AMultiPlayerBase* Player)
{
	//UE_LOG(LogTemp, Warning, TEXT("Splint: StateEnd"));
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StopCameraShake(SplintShake);
	if (Player->IsLocallyControlled()) 
	{ // ���ڼ� �ٽ� �ѱ�
		if (Player->GetHUD()->CrossHairHideCheck.IsBound()) Player->GetHUD()->CrossHairHideCheck.Execute(false);
	}
}

// ������ UClass ��ȯ
UClass* USplint_PlayerDown::GetState()
{
	return USplint_PlayerDown::StaticClass();
}

// ���콺 ���� �����ӿ� ���� ī�޶��� ȸ��(X��)�� ���մϴ�.
void USplint_PlayerDown::TurnAtRate(AMultiPlayerBase* Player, float Rate)
{
	Super::TurnAtRate(Player, Rate);
	CameraTurnChagneRoll = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(-2.2f, 2.2f), Rate);
}
