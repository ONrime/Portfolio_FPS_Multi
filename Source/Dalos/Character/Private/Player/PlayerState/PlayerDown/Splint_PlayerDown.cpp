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
	PelvisLocZ = 60.0f; // 골반 위치 (FootIK계산을 위해 있는 변수입니다.)
}

// 입력에 따른 상태 변환값을 반환하는 함수
UPlayerDownStateBase* USplint_PlayerDown::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // 플레이어 컨트롤러 입니다.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // 컨트롤러의 입력을 관리하는 객체입니다.

	// 각 지정된 키를 가져옵니다.
	TArray<FInputActionKeyMapping> ActionCrouch = PlayerInput->GetKeysForAction(TEXT("Crouch")); // 앉기 키를 가져옵니다.
	TArray<FInputActionKeyMapping> ActionSplint = PlayerInput->GetKeysForAction(TEXT("Splint")); // 달리기 키를 가져옵니다.

	if (PlayerInput->IsPressed(ActionCrouch[0].Key)) 
	{ // 앉기 키를 눌렀다면
		temp = NewObject<USliding_PlayerDown>(this, USliding_PlayerDown::StaticClass()); // 슬라이딩 상태 객체를 만듭니다.
	}
	else if (PlayerInput->IsPressed(ActionSplint[0].Key)) 
	{ // 달리기 키를 눌렀다면
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass()); // 서있기 상태 객체를 만듭니다.
	}
	return temp;
}

// 입력에 따른 상태 변환값을 반환하는 함수 (서버 및 다른 클라이언트에 전달하는 용도입니다.)
UPlayerDownStateBase* USplint_PlayerDown::SendHandleInput(EPlayerPress Press)
{
	if (Press == EPlayerPress::CROUCH) 
	{
		temp = NewObject<USliding_PlayerDown>(this, USliding_PlayerDown::StaticClass()); // 슬라이딩 상태 객체를 만듭니다.
	}
	else if (Press == EPlayerPress::SPLINT) 
	{
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass()); // 서있기 상태 객체를 만듭니다.
	}

	return temp;
}

// 강제 상태 변환(조건에 따른 변환을 할 때 사용합니다.)
UPlayerDownStateBase* USplint_PlayerDown::ChangeState(EPlayerDownState State)
{
	// 달리고 있다면 속도가 줄어들면 달리기 상태에서 서있기 상태로 변경할 때 사용됩니다.
	if (State == EPlayerDownState::STANDING) 
	{
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass());
	}
	return temp;
}

// 상태가 시작 할 때 작동하는 함수
void USplint_PlayerDown::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	//UE_LOG(LogTemp, Warning, TEXT("Splint: StateStart"));
	Player->PlayerSpeed = 120.0f; // 플레이어 속도
	Player->CameraPitch = 0.0f;   // 달리고 있을 때는 마우스의 위, 아래에 따른 카메라 위치 변환 없게 합니다.
	if (Player->IsLocallyControlled()) 
	{ // 달리고 있을 때는 십자선을 가립니다,
		if(Player->GetHUD()->CrossHairHideCheck.IsBound()) Player->GetHUD()->CrossHairHideCheck.Execute(true);
	}
}

// Tick에서 작동하는 함수
void USplint_PlayerDown::StateUpdate(AMultiPlayerBase* Player)
{
	// 카메라
	 //  카메라 회전
	if (Player->IsLocallyControlled()) 
	{ // 카메라 쉐이크 작동
		//SplintShake = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(UPlayer_Splint_CameraShake::StaticClass(), 1.0f);
		SplintShake = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(UPlayer_Splint_CameraShake::StaticClass(), 1.0f);
	}
	FRotator CurrentFollowCameraRot = Player->FollowCamera->GetRelativeRotation();
	Player->FollowCamera->SetRelativeRotation(FRotator(CurrentFollowCameraRot.Pitch, CurrentFollowCameraRot.Yaw
		, FMath::FInterpTo(CurrentFollowCameraRot.Roll, CameraTurnChagneRoll, GetWorld()->DeltaTimeSeconds, 5.0f)));

}

// 상태가 끝날 때 작동하는 함수
void USplint_PlayerDown::StateEnd(AMultiPlayerBase* Player)
{
	//UE_LOG(LogTemp, Warning, TEXT("Splint: StateEnd"));
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StopCameraShake(SplintShake);
	if (Player->IsLocallyControlled()) 
	{ // 십자선 다시 켜기
		if (Player->GetHUD()->CrossHairHideCheck.IsBound()) Player->GetHUD()->CrossHairHideCheck.Execute(false);
	}
}

// 상태의 UClass 반환
UClass* USplint_PlayerDown::GetState()
{
	return USplint_PlayerDown::StaticClass();
}

// 마우스 축의 움직임에 따라 카메라의 회전(X축)을 구합니다.
void USplint_PlayerDown::TurnAtRate(AMultiPlayerBase* Player, float Rate)
{
	Super::TurnAtRate(Player, Rate);
	CameraTurnChagneRoll = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(-2.2f, 2.2f), Rate);
}
