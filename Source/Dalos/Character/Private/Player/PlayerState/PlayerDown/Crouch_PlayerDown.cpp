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
	PelvisLocZ = 20.0f; // 골반 위치 (FootIK계산을 위해 있는 변수입니다.)
}

// 입력에 따른 상태 변환값을 반환하는 함수
UPlayerDownStateBase* UCrouch_PlayerDown::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // 플레이어 컨트롤러 입니다.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // 컨트롤러의 입력을 관리하는 객체입니다.

	// 각 지정된 키를 가져옵니다.
	TArray<FInputActionKeyMapping> ActionCrouch = PlayerInput->GetKeysForAction(TEXT("Crouch")); // 앉기 키를 가져옵니다.
	TArray<FInputActionKeyMapping> ActionSplint = PlayerInput->GetKeysForAction(TEXT("Splint")); // 달리기 키를 가져옵니다.
	TArray<FInputActionKeyMapping> ActionJump = PlayerInput->GetKeysForAction(TEXT("Jump"));	 // 점프 키를 가져옵니다.
	TArray<FInputActionKeyMapping> ActionProne = PlayerInput->GetKeysForAction(TEXT("Prone"));   // 엎드리기 키를 가져옵니다.

	if (PlayerInput->IsPressed(ActionCrouch[0].Key) || PlayerInput->IsPressed(ActionJump[0].Key))
	{
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass()); // 서있기 상태 객체를 만듭니다.
	}
	else if (PlayerInput->IsPressed(ActionSplint[0].Key))
	{
		temp = NewObject<USplint_PlayerDown>(this, USplint_PlayerDown::StaticClass()); // 달리기 상태 객체를 만듭니다.
	}
	else if (PlayerInput->IsPressed(ActionProne[0].Key))
	{
		temp = NewObject<UProne_PlayerDown>(this, UProne_PlayerDown::StaticClass()); // 엎드리기 상태 객체를 만듭니다.
	}

	return temp;
}

// 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
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

// 상태가 시작 할 때 작동하는 함수
void UCrouch_PlayerDown::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	UE_LOG(LogTemp, Warning, TEXT("Crouch: StateStart"));
	IsChangeState = false;
	Player->IsMove = true;
	Player->PlayerSpeed = 50.0f;
}

// Tick에서 작동하는 함수
void UCrouch_PlayerDown::StateUpdate(AMultiPlayerBase* Player)
{
	// 상태 변환시 작동
	if (IsChangeState) {
		StateChangeUpdate(Player);
	}
	else {
		FRotator InterpToAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();
		Player->CameraPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, -5.0f) / 2.0f;
		float PelvisZ = 20.0f;

		if (Player->GetUpperState()->GetEState() != EPlayerUpperState::ADS)
		{
			//  카메라 회전
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

// 상태가 끝날 때 작동하는 함수
void UCrouch_PlayerDown::StateEnd(AMultiPlayerBase* Player)
{
	//UE_LOG(LogTemp, Warning, TEXT("Crouch: StateEnd"));
}

// 상태가 바뀌기 시작할 때 작동하는 함수
void UCrouch_PlayerDown::StateChangeStart(AMultiPlayerBase* Player, float Speed)
{
	UE_LOG(LogTemp, Warning, TEXT("Crouch: StateChangeStart"));
	if (Speed != 0.0f) {
		ChangeSpeed = Speed;
	}
}

// 상태가 바뀌는 중 Tick에서 작동하는 함수
void UCrouch_PlayerDown::StateChangeUpdate(AMultiPlayerBase* Player)
{
	FRotator InterpToAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();
	Player->CameraPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, -5.0f) / 2.0f;
	float PelvisZ = 20.0f;

	if (Player->GetUpperState()->GetEState() != EPlayerUpperState::ADS)
	{
		//  카메라 회전
		if (Player->FollowCamera->GetRelativeRotation().Yaw != 0.0f)
		{
			float nowRoll = 0.0f;
			FRotator NowFollowCameraRot = Player->FollowCamera->GetRelativeRotation();
			Player->FollowCamera->SetRelativeRotation(FRotator(NowFollowCameraRot.Pitch, NowFollowCameraRot.Yaw
				, FMath::FInterpTo(NowFollowCameraRot.Roll, nowRoll, GetWorld()->DeltaTimeSeconds, 5.0f)));
		}
	}
}

// 상태의 UClass 반환
UClass* UCrouch_PlayerDown::GetState()
{
	return UCrouch_PlayerDown::StaticClass();
}

// 앉기 상태에서 점프키를 누르면 일어서기 상태 객체를 반환합니다.
UPlayerDownStateBase* UCrouch_PlayerDown::PlayerJump(AMultiPlayerBase* Player)
{
	return NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass());
}
