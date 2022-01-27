// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Crouch_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Standing_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Splint_PlayerDown.h"
#include "Character/Public/Player/PlayerState/PlayerDown/Prone_PlayerDown.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"

UCrouch_PlayerDown::UCrouch_PlayerDown() 
{
	temp = nullptr;
	DownState = EPlayerDownState::CROUCH;
	PelvisLocZ = 20.0f; // ��� ��ġ (FootIK����� ���� �ִ� �����Դϴ�.)
}

// �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
UPlayerDownStateBase* UCrouch_PlayerDown::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // �÷��̾� ��Ʈ�ѷ� �Դϴ�.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // ��Ʈ�ѷ��� �Է��� �����ϴ� ��ü�Դϴ�.

	// �� ������ Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionCrouch = PlayerInput->GetKeysForAction(TEXT("Crouch")); // �ɱ� Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionSplint = PlayerInput->GetKeysForAction(TEXT("Splint")); // �޸��� Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionJump = PlayerInput->GetKeysForAction(TEXT("Jump"));	 // ���� Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionProne = PlayerInput->GetKeysForAction(TEXT("Prone"));   // ���帮�� Ű�� �����ɴϴ�.

	if (PlayerInput->IsPressed(ActionCrouch[0].Key) || PlayerInput->IsPressed(ActionJump[0].Key))
	{
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass()); // ���ֱ� ���� ��ü�� ����ϴ�.
	}
	else if (PlayerInput->IsPressed(ActionSplint[0].Key))
	{
		temp = NewObject<USplint_PlayerDown>(this, USplint_PlayerDown::StaticClass()); // �޸��� ���� ��ü�� ����ϴ�.
	}
	else if (PlayerInput->IsPressed(ActionProne[0].Key))
	{
		temp = NewObject<UProne_PlayerDown>(this, UProne_PlayerDown::StaticClass()); // ���帮�� ���� ��ü�� ����ϴ�.
	}

	return temp;
}

// ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
UPlayerDownStateBase* UCrouch_PlayerDown::SendHandleInput(EPlayerPress Press)
{
	if (Press == EPlayerPress::CROUCH || Press == EPlayerPress::JUMP) {
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass());
	}
	else if (Press == EPlayerPress::SPLINT) {
		temp = NewObject<USplint_PlayerDown>(this, USplint_PlayerDown::StaticClass());
	}
	else if (Press == EPlayerPress::PRONE) {
		temp = NewObject<UProne_PlayerDown>(this, UProne_PlayerDown::StaticClass());
	}
	return temp;
}

// ���°� ���� �� �� �۵��ϴ� �Լ�
void UCrouch_PlayerDown::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	UE_LOG(LogTemp, Warning, TEXT("Crouch: StateStart"));
	IsChangeState = false;
	Player->IsMove = true;
	Player->PlayerSpeed = 50.0f;
}

// Tick���� �۵��ϴ� �Լ�
void UCrouch_PlayerDown::StateUpdate(AMultiPlayerBase* Player)
{
	// ���� ��ȯ�� �۵�
	if (IsChangeState) {
		StateChangeUpdate(Player);
	}
	else {
		FRotator InterpToAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();
		Player->CameraPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, -5.0f) / 2.0f;
		float PelvisZ = 20.0f;

		if (Player->GetUpperState()->GetEState() != EPlayerUpperState::ADS)
		{
			//  ī�޶� ȸ��
			if (Player->FollowCamera->GetRelativeRotation().Yaw != 0.0f)
			{
				float nowRoll = 0.0f;
				FRotator NowFollowCameraRot = Player->FollowCamera->GetRelativeRotation();
				Player->FollowCamera->SetRelativeRotation(FRotator(NowFollowCameraRot.Pitch, NowFollowCameraRot.Yaw
					, FMath::FInterpTo(NowFollowCameraRot.Roll, nowRoll, GetWorld()->DeltaTimeSeconds, 5.0f)));
			}
		}
	}
}

// ���°� ���� �� �۵��ϴ� �Լ�
void UCrouch_PlayerDown::StateEnd(AMultiPlayerBase* Player)
{
	//UE_LOG(LogTemp, Warning, TEXT("Crouch: StateEnd"));
}

// ���°� �ٲ�� ������ �� �۵��ϴ� �Լ�
void UCrouch_PlayerDown::StateChangeStart(AMultiPlayerBase* Player, float Speed)
{
	UE_LOG(LogTemp, Warning, TEXT("Crouch: StateChangeStart"));
	if (Speed != 0.0f) {
		ChangeSpeed = Speed;
	}
}

// ���°� �ٲ�� �� Tick���� �۵��ϴ� �Լ�
void UCrouch_PlayerDown::StateChangeUpdate(AMultiPlayerBase* Player)
{
	FRotator InterpToAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();
	Player->CameraPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, -5.0f) / 2.0f;
	float PelvisZ = 20.0f;

	if (Player->GetUpperState()->GetEState() != EPlayerUpperState::ADS)
	{
		//  ī�޶� ȸ��
		if (Player->FollowCamera->GetRelativeRotation().Yaw != 0.0f)
		{
			float nowRoll = 0.0f;
			FRotator NowFollowCameraRot = Player->FollowCamera->GetRelativeRotation();
			Player->FollowCamera->SetRelativeRotation(FRotator(NowFollowCameraRot.Pitch, NowFollowCameraRot.Yaw
				, FMath::FInterpTo(NowFollowCameraRot.Roll, nowRoll, GetWorld()->DeltaTimeSeconds, 5.0f)));
		}
	}
}

// ������ UClass ��ȯ
UClass* UCrouch_PlayerDown::GetState()
{
	return UCrouch_PlayerDown::StaticClass();
}

// �ɱ� ���¿��� ����Ű�� ������ �Ͼ�� ���� ��ü�� ��ȯ�մϴ�.
UPlayerDownStateBase* UCrouch_PlayerDown::PlayerJump(AMultiPlayerBase* Player)
{
	return NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass());
}
