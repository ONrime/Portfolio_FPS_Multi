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
	PelvisLocZ = 60.0f; // ��� ��ġ (FootIK����� ���� �ִ� �����Դϴ�.)

	CapsuleRadius = 50.0f;
	CapsuleHalfHeight = 96.0f;
	MeshZLoc = -97.0f;
}

// �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
UPlayerDownStateBase* UStanding_PlayerDown::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // �÷��̾� ��Ʈ�ѷ� �Դϴ�.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // ��Ʈ�ѷ��� �Է��� �����ϴ� ��ü�Դϴ�.

	// �� ������ Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionCrouch= PlayerInput->GetKeysForAction(TEXT("Crouch")); // �ɱ� Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionSplint = PlayerInput->GetKeysForAction(TEXT("Splint")); // �޸��� Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionProne = PlayerInput->GetKeysForAction(TEXT("Prone")); // ���帮�� Ű�� �����ɴϴ�.

	if (PlayerInput->IsPressed(ActionCrouch[0].Key)) 
	{// �ɱ� Ű�� ���ȴٸ� �۵��մϴ�.
		temp = NewObject<UCrouch_PlayerDown>(this, UCrouch_PlayerDown::StaticClass()); // �ɱ� ���� ��ü�� ����ϴ�.
	}
	else if (PlayerInput->IsPressed(ActionSplint[0].Key) && Player->GetInputDirForward() > 0.0f) 
	{// �޸��� Ű�� ���ȴٸ� �۵��մϴ�.
		temp = NewObject<USplint_PlayerDown>(this, USplint_PlayerDown::StaticClass()); // �޸��� ���� ��ü�� ����ϴ�.
	}
	else if (PlayerInput->IsPressed(ActionProne[0].Key)) 
	{// ���帮�� Ű�� ���ȴٸ� �۵��մϴ�.
		temp = NewObject<UProne_PlayerDown>(this, UProne_PlayerDown::StaticClass()); // ���帮�� ���� ��ü�� ����ϴ�.
	}

	return temp; // ������� ���� ��ü�� ��ȯ�մϴ�.
}

// �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ� (���� �� �ٸ� Ŭ���̾�Ʈ�� �����ϴ� �뵵�Դϴ�.)
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

// ���°� ���� �� �� �۵��ϴ� �Լ�
void UStanding_PlayerDown::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	UE_LOG(LogTemp, Warning, TEXT("Standing: StateStart"));
	IsChangeState = false;        // ���¸��� �ٲ� �ٲ�� �������� ����� �̷������ ���������� ǥ���ϴ� �����Դϴ�.
	Player->IsMove = true;        // �÷��̾ ������ �� �ְ� �մϴ�.
	// �� ���� ���� �ӵ��� �����մϴ�.
	Player->PlayerSpeed = 70.0f;  // ���ִ� ���¿����� �ӵ��� 70���� ����
	//Player->PelvisLength = 96.0f;
}

// Tick���� �۵��ϴ� �Լ�
void UStanding_PlayerDown::StateUpdate(AMultiPlayerBase* Player)
{
	if (IsChangeState) 
	{
		StateChangeUpdate(Player); // ���°� �ٲ� �� �ٲ�� ������ �����Ѵٸ� �۵��Ǵ� �Լ��Դϴ�.
	}
	else 
	{ // �⺻ update�Դϴ�.
		// ī�޶� ��ġ
		FRotator InterpToAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();
		Player->CameraPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, -5.0f) / 2.0f; // ī�޶��� ��, �� �������� ��Ÿ���� ���Դϴ�.

		// ī�޶� ȸ��
		// �����Ӱ� �þ��� ȸ�� ���߿� ���� ū ���� ������ ȸ�� ������ �����մϴ�.
		float CurrentRoll = 0.0f; // ������ ȸ�� ��
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

// ���°� ���� �� �۵��ϴ� �Լ�
void UStanding_PlayerDown::StateEnd(AMultiPlayerBase* Player)
{
	//UE_LOG(LogTemp, Warning, TEXT("Standing: StateEnd"));
}

// ���°� �ٲ�� ������ �� �۵��ϴ� �Լ�
void UStanding_PlayerDown::StateChangeStart(AMultiPlayerBase* Player, float Speed)
{
	//UE_LOG(LogTemp, Warning, TEXT("Standing: StateChangeStart"));
	if (Speed != 0.0f) 
	{
		ChangeSpeed = Speed; // �ٲ�� �ӵ� ����(Tick���� �۵��Ǵ� ���� �ӵ��� ���� �� �Դϴ�.)

		Player->GetCapsuleComponent()->GetScaledCapsuleSize(CurrentCapsuleRadius, CurrentCapsuleHalfHeight);
		CurrentMeshZLoc = Player->BodyMesh->GetRelativeLocation().Z;

	}
}

// ���°� �ٲ�� �� Tick���� �۵��ϴ� �Լ�
void UStanding_PlayerDown::StateChangeUpdate(AMultiPlayerBase* Player)
{
	// ī�޶� ��ġ
	FRotator InterpToAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();
	Player->CameraPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, -5.0f) / 2.0f; // ī�޶��� ��, �� �������� ��Ÿ���� ���Դϴ�.

	CurrentCapsuleRadius = FMath::FInterpTo(CurrentCapsuleRadius, CapsuleRadius, GetWorld()->DeltaTimeSeconds, ChangeSpeed);
	CurrentCapsuleHalfHeight = FMath::FInterpTo(CurrentCapsuleHalfHeight, CapsuleHalfHeight, GetWorld()->DeltaTimeSeconds, ChangeSpeed);
	CurrentMeshZLoc = FMath::FInterpTo(CurrentMeshZLoc, MeshZLoc, GetWorld()->DeltaTimeSeconds, ChangeSpeed * 2.0f);

	Player->GetCapsuleComponent()->SetCapsuleSize(CurrentCapsuleRadius, CurrentCapsuleHalfHeight);
	Player->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentMeshZLoc));
	Player->BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentMeshZLoc));
}

// ������ UClass ��ȯ
UClass* UStanding_PlayerDown::GetState()
{
	return UStanding_PlayerDown::StaticClass();
}

// ���콺 ���� �����ӿ� ���� ī�޶��� ȸ��(X��)�� ���մϴ�.
void UStanding_PlayerDown::TurnAtRate(AMultiPlayerBase* Player, float Rate)
{
	Super::TurnAtRate(Player, Rate);
	CameraTurnChagneRoll = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(-1.2f, 1.2f), Rate);
}

// �÷��̾��� �����ӿ� ���� ī�޶��� ȸ��(X��)�� ���մϴ�.
void UStanding_PlayerDown::PlayerMove(AMultiPlayerBase* Player, float InputValue, float InputDirRight)
{
	if (Player->IsLocallyControlled()) {
		if (InputValue > 0.0f) {
			WalkShake = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(UPlayer_Walk_CameraShake::StaticClass(), 1.0f);
		}else { GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StopCameraShake(WalkShake); }
		CameraMoveChagneRoll = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(-1.5f, 1.5f), InputDirRight);
	}
}
