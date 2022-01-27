// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Public/Player/PlayerState/PlayerDown/Prone_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Crouch_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Standing_PlayerDown.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"

UProne_PlayerDown::UProne_PlayerDown()
{
	temp = nullptr;
	DownState = EPlayerDownState::PRONE;
	PelvisLocZ = -40.0f;
	IsChangeState = true;
	ChangeSpeed = 5.0f;

	CapsuleRadius = 15.0f;
	CapsuleHalfHeight = 20.0f;
	MeshZLoc = -15.0f;
}

UPlayerDownStateBase* UProne_PlayerDown::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput);
	TArray<FInputActionKeyMapping> ActionCrouch = PlayerInput->GetKeysForAction(TEXT("Crouch"));
	TArray<FInputActionKeyMapping> ActionProne = PlayerInput->GetKeysForAction(TEXT("Prone"));

	if (PlayerInput->IsPressed(ActionCrouch[0].Key)) 
	{
		temp = NewObject<UCrouch_PlayerDown>(this, UCrouch_PlayerDown::StaticClass());
		temp->SetIsChangeState(true);
	}
	else if (PlayerInput->IsPressed(ActionProne[0].Key))
	{
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass());
		temp->SetIsChangeState(true);
	}

	return temp;
}

UPlayerDownStateBase* UProne_PlayerDown::SendHandleInput(EPlayerPress Press)
{
	if (Press == EPlayerPress::CROUCH) 
	{
		temp = NewObject<UCrouch_PlayerDown>(this, UCrouch_PlayerDown::StaticClass());
		temp->SetIsChangeState(true);
	}
	else if (Press == EPlayerPress::PRONE)
	{
		temp = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass());
		temp->SetIsChangeState(true);
	}

	return temp;
}


void UProne_PlayerDown::StateChangeStart(AMultiPlayerBase* Player, float Speed)
{
	UE_LOG(LogTemp, Warning, TEXT("Prone: StateChangeStart"));
	Player->IsMove = false;
	if (Speed != 0.0f)
	{
		ChangeSpeed = Speed;

	}

	// 몸에 카메라를 장착합니다.
	Player->SpringArm->AttachToComponent(Player->BodyMesh, FAttachmentTransformRules::KeepRelativeTransform, "CameraLoc");
	Player->SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Player->SetIsCameraLock(true); // 카메라 이동 잠그기

	Player->GetCapsuleComponent()->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
	Player->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, MeshZLoc));
	Player->BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, MeshZLoc));
	Player->SetActorLocation(FVector(Player->GetActorLocation().X, Player->GetActorLocation().Y, Player->GetActorLocation().Z - 76.0f));

}

void UProne_PlayerDown::StateChangeUpdate(AMultiPlayerBase* Player)
{
	// 카메라 위치
	int ProneDir = ProneYaw / FMath::Abs(ProneYaw);
	Player->CameraPitch = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 180.0f * ProneDir), FVector2D(55.0f, 70.0f), ProneYaw);
	//Player->CameraPitch = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 180.0f * ProneDir), FVector2D(-25.0f, 5.0f), ProneYaw);

}

void UProne_PlayerDown::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	UE_LOG(LogTemp, Warning, TEXT("Prone: StateStart1"));
	IsChangeState = false;
	Player->IsMove = true;
	Player->PlayerSpeed = 20.0f;
	IsTurn = true;
	ProneActorRot = Player->GetActorRotation();
	Player->SetIsProne(true);

	//Player->GetCapsuleComponent()->SetCollisionProfileName("ProneColl");
	Box = NewObject<UBoxComponent>(Player, UBoxComponent::StaticClass());
	if (Box)
	{
		Box->RegisterComponent();
		Box->SetBoxExtent(FVector(85.0f, 30.0f, 15.0f));
		Box->SetCollisionProfileName("Pawn");
		Box->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
		Box->AttachToComponent(Player->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		BoxRot = Player->GetActorRotation();
	}
	
}

void UProne_PlayerDown::StateUpdate(AMultiPlayerBase* Player)
{
	// 상태 변환시 작동
	if (IsChangeState)
	{
		StateChangeUpdate(Player);
	}
	else{
		Player->IsMove = true;
		// 카메라 위치
		int ProneDir = ProneYaw / FMath::Abs(ProneYaw);
		//Player->CameraPitch = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 180.0f * ProneDir), FVector2D(-25.0f, 5.0f), ProneYaw);
		Player->CameraPitch = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 180.0f * ProneDir), FVector2D(55.0f, 70.0f), ProneYaw);

		float NowRoll = 0.0f;
		FRotator NowFollowCameraRot = Player->FollowCamera->GetRelativeRotation();

		if (ProneYaw * ProneDir <= 90.0f)
		{
			NowRoll = FMath::GetMappedRangeValueClamped(FVector2D(20.0f * ProneDir, 90.0f * ProneDir), FVector2D(0.0f, -25.0f * ProneDir), ProneYaw);
		}
		else { NowRoll = FMath::GetMappedRangeValueClamped(FVector2D(90.0f * ProneDir, 160.0f * ProneDir), FVector2D(-25.0f * ProneDir, 0.0f), ProneYaw); }


		Player->FollowCamera->SetRelativeRotation(FRotator(NowFollowCameraRot.Pitch, NowFollowCameraRot.Yaw
			, FMath::FInterpTo(NowFollowCameraRot.Roll, NowRoll, GetWorld()->DeltaTimeSeconds, 5.0f)));

		//UE_LOG(LogTemp, Warning, TEXT("SpringArm: %f, %f, %f"), Player->FollowCamera->GetRelativeLocation().X
		//	, Player->FollowCamera->GetRelativeLocation().Y, Player->FollowCamera->GetRelativeLocation().Z);

	}
	
	//UE_LOG(LogTemp, Warning, TEXT("CameraPitch: %f"), Player->CameraPitch);
	UE_LOG(LogTemp, Warning, TEXT("ProneYaw: %f"), ProneYaw);
	
	//UE_LOG(LogTemp, Warning, TEXT("ProneDir: %d"), ProneDir);
	//UE_LOG(LogTemp, Warning, TEXT("NowRoll: %f"), NowRoll);
}

void UProne_PlayerDown::StateEnd(AMultiPlayerBase* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Prone: StateEnd"));

	Player->SpringArm->AttachToComponent(Player->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	Player->SpringArm->SetRelativeLocation(FVector(25.0f, 0.0f, 65.0f));
	Player->SetIsCameraLock(false);

	Player->IsMove = false;
	IsChangeState = true;
	Player->SetIsProne(false);

	if (Box)
	{
		Box->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		Box->DestroyComponent();
	}
}

UClass* UProne_PlayerDown::GetState()
{
	return UProne_PlayerDown::StaticClass();
}

// 마우스 축의 움직임에 따라 엎드렸을 때 상체 회전(Z축) 값을 구합니다.
void UProne_PlayerDown::TurnAtRate(AMultiPlayerBase* Player, float Rate)
{
	Super::TurnAtRate(Player, Rate);
	//CameraTurnChagneRoll = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(-1.2f, 1.2f), Rate);
	
	if (Player->GetIsProne())
	{
		FRotator ProneRot = FRotator::ZeroRotator;

		if (Player->GetVelocity().Size() > 3.0f)
		{
			// 움직일 때 고정된 방향(TurnDirEnd)을 움직이는 방향으로 바꾼다.
			ProneActorRot = Player->GetActorRotation();

			if (!(ProneYaw <= 110.0f && ProneYaw >= -110.0f))
			{
				// 만약 등이 바닥을 향하게 누워있다면 향해야 되는 방향을 반대로 하기
				ProneActorRot = FRotator(ProneActorRot.Pitch, ProneActorRot.Yaw + 180.0f, ProneActorRot.Roll);
				ProneBack = 1; // 누워있는 상태
			}
			else {
				ProneBack = -1; // 엎드려있는 상태
			}
			BoxRot = Player->GetActorRotation();
		}
		else
		{
			if (!(ProneYaw <= 110.0f && ProneYaw >= -110.0f))
			{
				ProneBack = 1; // 누워있는 상태
			}
			else {
				ProneBack = -1; // 엎드려있는 상태
			}
			
		}
		if (Box) Box->SetWorldRotation(BoxRot);
		FRotator ProneInterpToAngle = (Player->GetActorRotation() - ProneActorRot).GetNormalized();
		ProneYaw = ProneInterpToAngle.Yaw;
	}
}

// 엎드렸을 때 마우스 축의 움직임(상, 하)에 따라 지정한 한계지점까지만 카메라가 움직이게 합니다.
void UProne_PlayerDown::LookUpAtRate(AMultiPlayerBase* Player, float Rate)
{
	FRotator InterpToAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();
	float PronePitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, 90.0f);
	//Player->CameraPitch = FMath::ClampAngle(ProneYaw, -25.0f, 25.0f);

	if (Player->IsPlayerCameraTurn) {
		Player->SpringArm->bUsePawnControlRotation = true;
		float PitchRate = Rate * Player->BaseLookUpRate * 0.008f;
		float PitchDownLimite = 0.0f;
		float PitchUpLimite = 0.0f;

		PitchDownLimite = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 180.0f), FVector2D(-5.0f, -40.0f), FMath::Abs(ProneYaw));
		PitchUpLimite = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 180.0f), FVector2D(20.0f, 40.0f), FMath::Abs(ProneYaw));
		if ((PronePitch <= PitchDownLimite && PitchRate > 0.0f) || (PronePitch >= PitchUpLimite && PitchRate <= 0.0f)) PitchRate = 0.0f;
		Player->AddControllerPitchInput(PitchRate);
	}
	else { // 카메라 움직임 끄기
		//player->SpringArm->bUsePawnControlRotation = false;
	}
}
