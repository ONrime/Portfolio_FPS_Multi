#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Standing_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Splint_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Crouch_PlayerDown.h"
#include "Character/Public/Player/PlayerState/PlayerDown/Prone_PlayerDown.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraShake.h"
#include "Dalos/Player_Walk_CameraShake.h"

UStanding_PlayerDown::UStanding_PlayerDown()
{
	temp = nullptr;
	DownState = EPlayerDownState::STANDING;
	PelvisLocZ = 60.0f; // 골반 위치 (FootIK계산을 위해 있는 변수입니다.)

	CapsuleRadius = 50.0f;
	CapsuleHalfHeight = 96.0f;
	MeshZLoc = -97.0f;
}

// 입력에 따른 상태 변환값을 반환하는 함수
UPlayerDownStateBase* UStanding_PlayerDown::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // 플레이어 컨트롤러 입니다.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // 컨트롤러의 입력을 관리하는 객체입니다.

	// 각 지정된 키를 가져옵니다.
	TArray<FInputActionKeyMapping> ActionCrouch= PlayerInput->GetKeysForAction(TEXT("Crouch")); // 앉기 키를 가져옵니다.
	TArray<FInputActionKeyMapping> ActionSplint = PlayerInput->GetKeysForAction(TEXT("Splint")); // 달리기 키를 가져옵니다.
	TArray<FInputActionKeyMapping> ActionProne = PlayerInput->GetKeysForAction(TEXT("Prone")); // 엎드리기 키를 가져옵니다.

	if (PlayerInput->IsPressed(ActionCrouch[0].Key)) 
	{// 앉기 키를 눌렸다면 작동합니다.
		temp = NewObject<UCrouch_PlayerDown>(this, UCrouch_PlayerDown::StaticClass()); // 앉기 상태 객체를 만듭니다.
	}
	else if (PlayerInput->IsPressed(ActionSplint[0].Key) && Player->GetInputDirForward() > 0.0f) 
	{// 달리는 키를 눌렸다면 작동합니다.
		temp = NewObject<USplint_PlayerDown>(this, USplint_PlayerDown::StaticClass()); // 달리기 상태 객체를 만듭니다.
	}
	else if (PlayerInput->IsPressed(ActionProne[0].Key)) 
	{// 엎드리기 키를 눌렸다면 작동합니다.
		temp = NewObject<UProne_PlayerDown>(this, UProne_PlayerDown::StaticClass()); // 엎드리기 상태 객체를 만듭니다.
	}

	return temp; // 만들어진 상태 객체를 반환합니다.
}

// 입력에 따른 상태 변환값을 반환하는 함수 (서버 및 다른 클라이언트에 전달하는 용도입니다.)
UPlayerDownStateBase* UStanding_PlayerDown::SendHandleInput(EPlayerPress Press)
{
	if (Press == EPlayerPress::CROUCH) {
		temp = NewObject<UCrouch_PlayerDown>(this, UCrouch_PlayerDown::StaticClass());
	}
	else if (Press == EPlayerPress::SPLINT) {
		temp = NewObject<USplint_PlayerDown>(this, USplint_PlayerDown::StaticClass());
	}
	else if (Press == EPlayerPress::PRONE) {
		temp = NewObject<UProne_PlayerDown>(this, UProne_PlayerDown::StaticClass());
	}
	//UE_LOG(LogTemp, Warning, TEXT("SendHandleInput"));
	return temp;
}

// 상태가 시작 할 때 작동하는 함수
void UStanding_PlayerDown::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	UE_LOG(LogTemp, Warning, TEXT("Standing: StateStart"));
	IsChangeState = false;        // 상태마다 바뀔때 바뀌는 과정에서 계산이 이루어지는 상태인지를 표시하는 변수입니다.
	Player->IsMove = true;        // 플레이어가 움직일 수 있게 합니다.
	// 각 상태 별로 속도를 설정합니다.
	Player->PlayerSpeed = 70.0f;  // 서있는 상태에서의 속도를 70으로 설정
	//Player->PelvisLength = 96.0f;
}

// Tick에서 작동하는 함수
void UStanding_PlayerDown::StateUpdate(AMultiPlayerBase* Player)
{
	if (IsChangeState) 
	{
		StateChangeUpdate(Player); // 상태가 바뀔 때 바뀌는 과정이 존재한다면 작동되는 함수입니다.
	}
	else 
	{ // 기본 update입니다.
		// 카메라 위치
		FRotator InterpToAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();
		Player->CameraPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, -5.0f) / 2.0f; // 카메라의 상, 하 움직임을 나타내는 값입니다.

		// 카메라 회전
		// 움직임과 시야의 회전 값중에 가장 큰 값을 현재의 회전 값으로 설정합니다.
		float CurrentRoll = 0.0f; // 현재의 회전 값
		if (FMath::Abs(CameraMoveChagneRoll) > FMath::Abs(CameraTurnChagneRoll))
		{
			CurrentRoll = CameraMoveChagneRoll;
		}
		else { CurrentRoll = CameraTurnChagneRoll; }

		FRotator CurrentFollowCameraRot = Player->FollowCamera->GetRelativeRotation();
		Player->FollowCamera->SetRelativeRotation(FRotator(CurrentFollowCameraRot.Pitch, CurrentFollowCameraRot.Yaw
			, FMath::FInterpTo(CurrentFollowCameraRot.Roll, CurrentRoll, GetWorld()->DeltaTimeSeconds, 5.0f)));

	}
}

// 상태가 끝날 때 작동하는 함수
void UStanding_PlayerDown::StateEnd(AMultiPlayerBase* Player)
{
	//UE_LOG(LogTemp, Warning, TEXT("Standing: StateEnd"));
}

// 상태가 바뀌기 시작할 때 작동하는 함수
void UStanding_PlayerDown::StateChangeStart(AMultiPlayerBase* Player, float Speed)
{
	//UE_LOG(LogTemp, Warning, TEXT("Standing: StateChangeStart"));
	if (Speed != 0.0f) 
	{
		ChangeSpeed = Speed; // 바뀌는 속도 설정(Tick에서 작동되는 보간 속도를 정한 것 입니다.)

		Player->GetCapsuleComponent()->GetScaledCapsuleSize(CurrentCapsuleRadius, CurrentCapsuleHalfHeight);
		CurrentMeshZLoc = Player->BodyMesh->GetRelativeLocation().Z;

	}
}

// 상태가 바뀌는 중 Tick에서 작동하는 함수
void UStanding_PlayerDown::StateChangeUpdate(AMultiPlayerBase* Player)
{
	// 카메라 위치
	FRotator InterpToAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();
	Player->CameraPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, -5.0f) / 2.0f; // 카메라의 상, 하 움직임을 나타내는 값입니다.

	CurrentCapsuleRadius = FMath::FInterpTo(CurrentCapsuleRadius, CapsuleRadius, GetWorld()->DeltaTimeSeconds, ChangeSpeed);
	CurrentCapsuleHalfHeight = FMath::FInterpTo(CurrentCapsuleHalfHeight, CapsuleHalfHeight, GetWorld()->DeltaTimeSeconds, ChangeSpeed);
	CurrentMeshZLoc = FMath::FInterpTo(CurrentMeshZLoc, MeshZLoc, GetWorld()->DeltaTimeSeconds, ChangeSpeed * 2.0f);

	Player->GetCapsuleComponent()->SetCapsuleSize(CurrentCapsuleRadius, CurrentCapsuleHalfHeight);
	Player->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentMeshZLoc));
	Player->BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentMeshZLoc));
}

// 상태의 UClass 반환
UClass* UStanding_PlayerDown::GetState()
{
	return UStanding_PlayerDown::StaticClass();
}

// 마우스 축의 움직임에 따라 카메라의 회전(X축)을 구합니다.
void UStanding_PlayerDown::TurnAtRate(AMultiPlayerBase* Player, float Rate)
{
	Super::TurnAtRate(Player, Rate);
	CameraTurnChagneRoll = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(-1.2f, 1.2f), Rate);
}

// 플레이어의 움직임에 따라 카메라의 회전(X축)을 구합니다.
void UStanding_PlayerDown::PlayerMove(AMultiPlayerBase* Player, float InputValue, float InputDirRight)
{
	if (Player->IsLocallyControlled()) {
		if (InputValue > 0.0f) {
			WalkShake = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(UPlayer_Walk_CameraShake::StaticClass(), 1.0f);
		}else { GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StopCameraShake(WalkShake); }
		CameraMoveChagneRoll = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(-1.5f, 1.5f), InputDirRight);
	}
}
