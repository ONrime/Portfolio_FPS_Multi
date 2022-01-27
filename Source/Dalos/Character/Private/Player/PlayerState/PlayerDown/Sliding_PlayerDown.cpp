// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Sliding_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Standing_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Crouch_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerBody_AnimInstance.h"
#include "Dalos/Character/Public/Player/PlayerArm_AnimInstance.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraShake.h"
#include "Kismet/KismetMathLibrary.h"
#include "Dalos/Player_Walk_CameraShake.h"

USliding_PlayerDown::USliding_PlayerDown()
{
	temp = nullptr;
	DownState = EPlayerDownState::SLIDING;
	PelvisLocZ = 20.0f; // ��� ��ġ (FootIK����� ���� �ִ� �����Դϴ�.)
}

// �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
UPlayerDownStateBase* USliding_PlayerDown::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // �÷��̾� ��Ʈ�ѷ� �Դϴ�.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // ��Ʈ�ѷ��� �Է��� �����ϴ� ��ü�Դϴ�.

	// �� ������ Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionJump = PlayerInput->GetKeysForAction(TEXT("Jump"));     // ���� Ű�� �����ɴϴ�.
	TArray<FInputActionKeyMapping> ActionCrouch = PlayerInput->GetKeysForAction(TEXT("Crouch")); // �ɱ� Ű�� �����ɴϴ�.

	if (PlayerInput->IsPressed(ActionCrouch[0].Key) || PlayerInput->IsPressed(ActionJump[0].Key)) 
	{
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass()); // ���ֱ� ���� ��ü�� ����ϴ�.
	}

	return temp;
}

// ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
UPlayerDownStateBase* USliding_PlayerDown::SendHandleInput(EPlayerPress Press)
{
	if (Press == EPlayerPress::CROUCH || Press == EPlayerPress::JUMP) {
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass());
	}
	UE_LOG(LogTemp, Warning, TEXT("SendHandleInput"));
	return temp;
}

// ���� ���� ��ȯ(���ǿ� ���� ��ȯ�� �� �� ����մϴ�.)
UPlayerDownStateBase* USliding_PlayerDown::ChangeState(EPlayerDownState State)
{
	if (State == EPlayerDownState::CROUCH) {
		temp = NewObject<UCrouch_PlayerDown>(this, UCrouch_PlayerDown::StaticClass());
	}
	return temp;
}

// ���°� ���� �� �� �۵��ϴ� �Լ�
void USliding_PlayerDown::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	UE_LOG(LogTemp, Warning, TEXT("Sliding: StateStart"));
	Player->PlayerSpeed = 0.0f;
	Player->IsMove = false;
	Player->AddMovementInput(Player->GetMesh()->GetRightVector(), 30.0f);
	Player->GetBodyAnim()->PlaySlidingMontage();
	Player->GetLegAnim()->PlaySlidingMontage();
	Player->CameraPitch = 0.0f;
}

// Tick���� �۵��ϴ� �Լ�
void USliding_PlayerDown::StateUpdate(AMultiPlayerBase* Player)
{
	FRotator InterpToAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();
	Player->CameraPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, -5.0f) / 2.0f;

	SlidingAngle = GetSlidingAngle(Player->GetMesh()->GetComponentLocation(), Player->GetMesh()->GetUpVector(), ForceDir);

	float Dot = FVector::DotProduct(ForceDir, Player->GetMesh()->GetRightVector());
	float BodyAngle = FMath::RadiansToDegrees(FMath::Acos(Dot));
	if (BodyAngle < 50.0f) SlidingAngle = 90.0f;

	float AddForce = FMath::GetMappedRangeValueClamped(FVector2D(90.0f, 0.0f), FVector2D(0.0f, 30.0f), SlidingAngle);
	BaseForce = FMath::FInterpTo(BaseForce, 0.0f + AddForce, GetWorld()->DeltaTimeSeconds, 3.0f);

	Player->AddMovementInput(Player->GetMesh()->GetRightVector(), BaseForce);

	//UE_LOG(LogTemp, Warning, TEXT("GetVelocity().Size(): %f"), player->GetVelocity().Size());
	//UE_LOG(LogTemp, Warning, TEXT("BodyAngle: %f"), BodyAngle);
	//UE_LOG(LogTemp, Warning, TEXT("BaseForce: %f"), BaseForce);
	//UE_LOG(LogTemp, Warning, TEXT("AddForce: %f"), AddForce);
}

// ���°� ���� �� �۵��ϴ� �Լ�
void USliding_PlayerDown::StateEnd(AMultiPlayerBase* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Sliding: StateEnd"));
	Player->IsMove = true;
	Player->GetBodyAnim()->StopMontage();
	Player->GetLegAnim()->StopMontage();
}

UClass* USliding_PlayerDown::GetState()
{
	return USliding_PlayerDown::StaticClass();
}

// �ٴڿ� Trace�� ���� �ٴ��� ���� �����̵� ������ ���մϴ�.
float USliding_PlayerDown::GetSlidingAngle(FVector Loc, FVector Dir, FVector& SlidingDir)
{
	float Angle = 0.0f;
	FVector StartTracer = Loc + Dir * 50.0f;
	FVector EndTracer = Loc + Dir * -30.0f;
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;
	bool IsHit = UKismetSystemLibrary::LineTraceSingle(this, StartTracer, EndTracer, ETraceTypeQuery::TraceTypeQuery4
		, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	if (IsHit) {
		// �ٴڿ��� ���� ������ X���� ��ǥ�迡�� Z���� �����̵��� �������� �����մϴ�.
		SlidingDir = FRotationMatrix::MakeFromX(OutHit.Normal).GetUnitAxis(EAxis::Z);
		float Dot = FVector::DotProduct(Dir, SlidingDir);
		Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
		UE_LOG(LogTemp, Warning, TEXT("SlidingEngle: %f"), Angle);
	}

	return Angle;
}
