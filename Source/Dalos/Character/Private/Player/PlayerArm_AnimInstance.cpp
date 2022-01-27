// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerArm_AnimInstance.h"
#include "Dalos/Character/Public/Player/PlayerBody_AnimInstance.h"
#include "Dalos/Character/Public/Player/MultiPlayerBase.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/Armed_PlayerUpper.h"
#include "Net/UnrealNetwork.h"

UPlayerArm_AnimInstance::UPlayerArm_AnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage>AK_FIRE1(TEXT("AnimMontage'/Game/Player/Anim/Arm/Armed/AK/Fire/Armed_AK_Fire_Center_Arm_Montage.Armed_AK_Fire_Center_Arm_Montage'"));
	if (AK_FIRE1.Succeeded()) AK_Fire1_Montage = AK_FIRE1.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>AK_FIRE2(TEXT("AnimMontage'/Game/Player/Anim/Arm/Armed/AK/Fire/Armed_AK_Fire_Complex_Arm_Montage.Armed_AK_Fire_Complex_Arm_Montage'"));
	if (AK_FIRE2.Succeeded()) AK_Fire2_Montage = AK_FIRE2.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>AK_FIRE(TEXT("AnimMontage'/Game/Player/Anim/Arm/Armed/AK/Fire/Armed_AK_Fire_C_Arm_Montage.Armed_AK_Fire_C_Arm_Montage'"));
	if (AK_FIRE.Succeeded()) AK_Fire_Montage = AK_FIRE.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>AK_ADS_FIRE(TEXT("AnimMontage'/Game/Player/Anim/Arm/Armed/AK/Fire/Armed_AK_Fire_C_ADS_Arm_Montage.Armed_AK_Fire_C_ADS_Arm_Montage'"));
	if (AK_ADS_FIRE.Succeeded()) AK_ADS_Fire_Montage = AK_ADS_FIRE.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>AK_RELOAD(TEXT("AnimMontage'/Game/Player/Anim/Arm/Armed/AK/Armed_AK_Reload_Arm_Montage.Armed_AK_Reload_Arm_Montage'"));
	if (AK_RELOAD.Succeeded()) AK_Reload_Montage = AK_RELOAD.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>MELEE(TEXT("AnimMontage'/Game/Player/Anim/Arm/UnArmed/UnArmed_Punch_Arm_Montage.UnArmed_Punch_Arm_Montage'"));
	if (MELEE.Succeeded()) Melee_Montage = MELEE.Object;
}

void UPlayerArm_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto pawn = TryGetPawnOwner();
	if (::IsValid(pawn)) {
		auto player = Cast<AMultiPlayerBase>(pawn);
		playerSpeed = player->GetVelocity().Size();
		FRotator InterpToAngle = (player->GetControllerRot() - player->GetActorRotation()).GetNormalized();
		upperPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, 90.0f);

		IsJumped = player->IsJumped;
		IsFalling = player->GetMovementComponent()->IsFalling();
		IsHandUp = player->IsHandUp;

		UpperStateNClass = player->UpperStateNClass;
		DownStateNClass= player->DownStateNClass;

		FVector playerVelocity = player->GetVelocity();
		playerVelocity.Normalize();
		FMatrix RotMatrix = FRotationMatrix(player->GetActorRotation());
		FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
		FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
		FVector NormalizedVel = playerVelocity.GetSafeNormal2D();

		dirForward = FVector::DotProduct(ForwardVector, NormalizedVel);
		dirRight = FVector::DotProduct(RightVector, NormalizedVel);
		if (FMath::Abs(dirForward) > FMath::Abs(dirRight)) {
			InputDir = FMath::Abs(dirForward);
		}
		else { InputDir = FMath::Abs(dirRight); }

		AimDirRight = player->GetAimDirRight();
		AimDirUp = player->GetAimDirUp();
		//UE_LOG(LogTemp, Warning, TEXT("AimDirRight: %f"), AimDirRight);
	}
}

void UPlayerArm_AnimInstance::PlayFireMontage()
{
	fireRandNum = FMath::RandRange(1, 3);
	FString hitString = "Fire";
	hitString.AppendInt(fireRandNum);
	FName hitName = (*hitString);

	if (UpperStateNClass == UArmed_PlayerUpper::StaticClass())
	{
		Montage_Play(AK_Fire_Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
		Montage_JumpToSection(hitName, AK_Fire_Montage);
	}
	else {
		Montage_Play(AK_ADS_Fire_Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
		Montage_JumpToSection(hitName, AK_ADS_Fire_Montage);
	}
	
}

void UPlayerArm_AnimInstance::StopFireMontage()
{
	/*switch (fireRandNum) {
	case 1:
		Montage_JumpToSection("End", AK_Fire1_Montage);
		break;
	case 2:
		Montage_JumpToSection("End", AK_Fire2_Montage);
		break;
	}*/
}

void UPlayerArm_AnimInstance::PlayReloadMontage()
{
	Montage_Play(AK_Reload_Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
}

void UPlayerArm_AnimInstance::StopReloadMontage()
{
	Montage_Stop(0.1f);
}

void UPlayerArm_AnimInstance::PlayMeleeMontage()
{
	int RandNum = FMath::RandRange(1, 2);
	FString hitString = "Melee";
	hitString.AppendInt(RandNum);
	FName hitName = (*hitString);

	Montage_Play(Melee_Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
	Montage_JumpToSection(hitName, Melee_Montage);
}

void UPlayerArm_AnimInstance::StopMeleeMontage()
{
	Montage_Stop(0.1f);
}

void UPlayerArm_AnimInstance::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UPlayerArm_AnimInstance, upperPitch, COND_SkipOwner);
}
