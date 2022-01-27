// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerBody_AnimInstance.h"
#include "Dalos/Character/Public/Player/MultiPlayerBase.h"
#include "GameFramework/SpringArmComponent.h"
//#include "Net/UnrealNetwork.h"


UPlayerBody_AnimInstance::UPlayerBody_AnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage>VAULT(TEXT("AnimMontage'/Game/Player/Anim/Body/Climb/Player_Vault_Montage.Player_Vault_Montage'"));
	if (VAULT.Succeeded()) Vault_Montage = VAULT.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>CLIMB(TEXT("AnimMontage'/Game/Player/Anim/Body/Climb/ALS_N_Mantle_1m_RH_Montage.ALS_N_Mantle_1m_RH_Montage'"));
	if (CLIMB.Succeeded()) Climb_Montage = CLIMB.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>SLIDING(TEXT("AnimMontage'/Game/Player/Anim/Body/Sliding_Body_Montage.Sliding_Body_Montage'"));
	if (SLIDING.Succeeded()) Sliding_Montage = SLIDING.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>MELEE_BODY(TEXT("AnimMontage'/Game/Player/Anim/Body/UnArmed/UnArmed_Melee_Body_Montage.UnArmed_Melee_Body_Montage'"));
	if (MELEE_BODY.Succeeded()) Melee_Montage = MELEE_BODY.Object;
}

void UPlayerBody_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn)) 
	{
		AMultiPlayerBase* Player = Cast<AMultiPlayerBase>(Pawn);

		// �Ϲ����� ���¿����� Ȥ�� ������� ���� ��ü ��(UpperYaw, UpperProneYaw)�� ���ϴ� �Լ��Դϴ�.
		TurnBodyYaw(Player, UpperYaw, UpperProneYaw);
		//UpperYaw = player->GetUpperYaw();
		//UpperProneYaw = player->GetProneYaw();
		//ProneRotBlend = player->GetProneRotBlend();
		FRotator InterpToAngle = (Player->GetControllerRot() - Player->GetActorRotation()).GetNormalized();
		UpperPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, 90.0f); // ��, �Ʒ��� ��
		//UpperPitch = player->GetUpperPitch();
		
		PlayerSpeed = Player->GetVelocity().Size(); // �÷��̾��� �ӵ�
		// ������ �������� �Էµ� ������ ã�� �Լ��Դϴ�. 
		GetVeloctyBlend(Player->GetVelocity(), Player->GetActorRotation(), VelocityBlend);

		DownStateNClass = Player->DownStateNClass;   // ��ü�� ���¸� ��Ÿ���� �����Դϴ�.
		DownStateBClass = Player->DownStateBClass;   // ��ü�� �� ���¸� ��Ÿ���� �����Դϴ�.
		UpperStateNClass = Player->UpperStateNClass; // ��ü�� ���¸� ��Ÿ���� �����Դϴ�.

		IsJumped = Player->IsJumped;                             // ������ �������� ����
		IsFalling = Player->GetMovementComponent()->IsFalling(); // �߶��� ����
		IsHandUp = Player->IsHandUp;                             // ���� ��Ʈ�Ǿ� ���� �÷��� �Ǵ� ����
		IsProne = Player->GetIsProne();                          // ������� ��(�ٲ�� �������� �ƴ� ���帮�⸦ �������� ��)

		/* ���� ���
		* ��, ���� ������ ���� ô���� �־����� ������ ���ϴ� �����Դϴ�.
		*/
		if (Player->IsCoverLeft) // ���� ������ ��
		{
			// UpperYaw(��, ��)�� ���� ���� �����ϴ� ô�߰��� ���մϴ�.
			float ClampAngle = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, -50.0f), FVector2D(-Player->CoverAngleLeft, 5.0f), UpperYaw);
			CoverAngle = FMath::FInterpTo(CoverAngle, ClampAngle, GetWorld()->DeltaTimeSeconds, 12.0f);
		}
		if (Player->IsCoverRight) // ������ ������ ��
		{
			float ClampAngle = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 50.0f), FVector2D(Player->CoverAngleRight, -5.0f), UpperYaw);
			CoverAngle = FMath::FInterpTo(CoverAngle, ClampAngle, GetWorld()->DeltaTimeSeconds, 12.0f);
		}
		if (!Player->IsCoverRight && !Player->IsCoverLeft && CoverAngle != 0.0f)
		{
			// ���� ���Ҷ� 0���� �����մϴ�.
			CoverAngle = FMath::FInterpTo(CoverAngle, 0.0f, GetWorld()->DeltaTimeSeconds, 10.0f);
		}

		/* FootIK ���
		* ���� ���� ���� ������ ����ġ ���� ���Ҽ� �ֱ� ������ �̵��߿��� �������� ��ġ���� ���Ҽ� �ֽ��ϴ�.
		*/
		float LeftDistance = 0.0f, RightDistance = 0.0f, RootDistance = 0.0f;

		/* LeftFootAlpha, RightFootAlpha�� ���鿡 ��Ҵ����� ��Ÿ���� float�� �� ������
		* AnimBP���� Transform Bone�� Two Bone IK���� Alpha�� ����ϱ� ���� float�� ��������ϴ�.
		* FootIKTracer()�Լ����� ���� �ٴڿ� ��Ҵ���(LeftFootAlpha, RightFootAlpha)
		* , ��Ҵٸ� �ٴ��� ������ ���� ���� ����(LeftFootRot, RightFootRot)�� ��� �Ǵ���
		* , ���� �ٴ��� ��ġ�� ���� ���� ��ǥ(LeftFootLoc, RightFootLoc)�� ��� �־�� �ϴ���
		* , ���� ���� ��ġ�� �ٴڰ��� �Ÿ�(LeftDistance, RightDistance)�� ��� �Ǵ����� ���ϴ� �Լ��Դϴ�.
		* FootIK�� ���� ��ġ(Location)���� ����� ���� ����ؼ� 
		* �����(VB pelvis_foot_l, VB pelvis_foot_r)�� ����� ������� ���� �� ��ġ���� ���Ͽ�
		* �̵��� ���� �� ��ġ�� ����մϴ�.
		*/
		LeftFootAlpha = FootIKTracer(Player, "foot_l", "VB pelvis_foot_l", LeftFootLoc, LeftFootRot, LeftDistance);
		RightFootAlpha = FootIKTracer(Player, "foot_r", "VB pelvis_foot_r", RightFootLoc, RightFootRot, RightDistance);

		/*
		* 
		*/
		if (IsFalling) // �߶����϶� IK ���� �ϱ�
		{
			RootLock = true;
			RootDistance = 0.0f;
		}
		if(!RootLock){
			// ���̰� ū �Ÿ��� RootDistance�� ����Ѵ�.
			LeftDistance >= RightDistance ? RootDistance = LeftDistance : RootDistance = RightDistance;
			RootDistance *= -1;
		}
		RootZLoc = FMath::FInterpTo(RootZLoc, RootDistance, GetWorld()->DeltaTimeSeconds, 12.0f);
		Player->SetRootDistance(RootZLoc); // ī�޶� ��ġ�� �����ϱ����� �÷��̾�� ������ Root�� ��ġ�� �����ϴ�.

		if (RootZLoc >= -0.5f) RootLock = false; // ��� ����
		IsFalling ? RootLocAlpha = 0.0f : RootLocAlpha = 1.0f; // �߶����ο� ���� ��Ʈ�� ���İ��� ���մϴ�.

		// �����(���� �� ��ġ��)�� ��Ʈ Z�� ��ġ�� ���̰����� �̵��Ҷ��� ��ġ���� ���մϴ�.
		// LeftFootLoc.Z(�ٴ��� ��ġ) 
		// + (����� ��ġ(IK���� ���� �� ��ġ) - ��Ʈ ��ġ - 7.0f(�ణ�� ����)) --> IK���� ���� �ٴڿ��� ���� �߰��� �Ÿ��� (������ ���� ���� 0~1������ �̵� �Ҷ��� ���� Ŀ���ϴ�.)
		// + RootZLoc(��Ʈ�� ���� ���� �ٴ��� �Ÿ�) - 3.0f(�ణ�� ����)
		LeftFootLoc = FVector(LeftFootLoc.X, LeftFootLoc.Y
			, LeftFootLoc.Z + (Player->BodyMesh->GetSocketLocation("VB pelvis_foot_l").Z
				- (Player->BodyMesh->GetSocketLocation("root").Z - 7.0f)) + (RootZLoc * -1) - 3.0f);

		RightFootLoc = FVector(RightFootLoc.X, RightFootLoc.Y
			, RightFootLoc.Z + (Player->BodyMesh->GetSocketLocation("VB pelvis_foot_r").Z
				- (Player->BodyMesh->GetSocketLocation("root").Z - 7.0f)) + (RootZLoc * -1) - 3.0f);
		// �ȱ⸦ �� �� ���� ���� ��Ʈ ������ ����� �̷�������Ѵ�.

		/* Prone IK ���
		* 
		*/
		if (Player->GetIsProne())
		{
			// Root
			FVector RootDir = ProneRootIK(Player, "pelvis", ProneIK_Pelvis_Rot);
			// ô��
			FVector SpineDir = ProneMiddleIK(Player, "ProneSpineLoc", ProneIK_Upper_Rot, RootDir);
			// ����
			FVector Calf_Rgiht_Dir = ProneMiddleIK(Player, "ProneCalfRightLoc", ProneIK_Right_Knee_Rot, RootDir);
			FVector Calf_Left_Dir = ProneMiddleIK(Player, "ProneCalfLeftLoc", ProneIK_Left_Knee_Rot, RootDir);

			// ��
			ProneEndIK(Player, "ProneFootRightLoc", ProneIK_Right_Foot_Rot, Calf_Rgiht_Dir);
			ProneEndIK(Player, "ProneFootLeftLoc", ProneIK_Left_Foot_Rot, Calf_Left_Dir);
			// �Ȳ�ġ
			ProneEndIK(Player, "ProneArmRightLoc", ProneIK_Right_Hand_Rot, SpineDir);
			ProneEndIK(Player, "ProneArmLeftLoc", ProneIK_Left_Hand_Rot, SpineDir);
		}
		
		//UE_LOG(LogTemp, Warning, TEXT("upperPitch: %f"), player->GetControllerRot().Pitch - player->GetActorRotation().Pitch);
		//UE_LOG(LogTemp, Warning, TEXT("CoverAngle: %f"), CoverAngle);
		//UE_LOG(LogTemp, Warning, TEXT("RootZLoc: %f"), RootZLoc);
		//UE_LOG(LogTemp, Warning, TEXT("RightFootLoc.Z: %f"), RightFootLoc.Z);
		//UE_LOG(LogTemp, Warning, TEXT("VB pelvis_foot_r: %f"), Player->BodyMesh->GetSocketLocation("VB pelvis_foot_r").Z);
		//UE_LOG(LogTemp, Warning, TEXT("root: %f"), Player->BodyMesh->GetSocketLocation("root").Z);
	}
}

/* �÷��̾��� ��ü�� �����ǰ� ��ü�� �þ߿� ���� �����̰� �ϱ� ���� �ʿ��� ���� ��ȯ�ϴ� �Լ��Դϴ�.
* �⺻������ �÷��̾�� �þ߿� ���� ���� �����Դϴ�.
* ���� �÷��̾��� �þ߰� �����̴� ��ŭ ��ü�� �ݴ�� �����ٸ� ��ü�� ������ ���·� ��ü�� �����̴� ����� �� �� �ֽ��ϴ�.
* �̴�θ� �Ѵٸ� ��ü�� ������ü ��ü�� �ڸ� �ٶ󺸴� ������ȭ�� ���� ��Ȳ�� �������� ������
* ���� ������ ���ϰ� ������ �Ѿ�� ��ü�� �÷��̾ �ٶ󺸴� �������� ȸ���ϰ� �մϴ�.
* �ڵ�� �����ϸ� �÷��̾ �������� ������ �߽���(�߽��� = StandingDirEnd)�� ���ϰ�
* �� �߽����� �þ߿��� ���̸� ���� �� ��(Yaw)���� ��ü�� �ݴ�� �����̰� �մϴ�.
* 
*  ���� ���⼭ ���帮�� ��ɵ� ���� �ٷ�ϴ�.
* �ٸ� ���� �ִٸ� ���� ������ �Ѿ�� �߽���(�߽��� = StandingDirEnd)�� 180�� ȸ���ϰ� �ϴ� �Ͱ� �����ִ��� ������ִ����� �Ǵܸ� �մϴ�.
*/
void UPlayerBody_AnimInstance::TurnBodyYaw(AMultiPlayerBase* Player, float& Yaw, float& ProneYaw)
{
	if (Player->GetIsProne())
	{ // �÷��̾ ���帮�⸦ ���� �Ҷ�(�ٲ�� ���� �ƴ� ����)

		if (Player->GetVelocity().Size() > 3.0f) // ������ ��
		{
			// ������ �� ������ ����(TurnDirEnd)�� �����̴� �������� �ٲߴϴ�.
			StandingDirEnd = Player->GetActorRotation();

			if (!(ProneYaw <= 110.0f && ProneYaw >= -110.0f))
			{ // ���� ��ġ�� �ѱ�� ���帮�Ⱑ �ƴ� �����ִ� �� �Դϴ�.

				// ���� ���� �ٴ��� ���ϰ� �����ִٸ� �߽� ���� �ݴ�� �ϱ�
				StandingDirEnd = FRotator(StandingDirEnd.Pitch, StandingDirEnd.Yaw + 180.0f, StandingDirEnd.Roll);
				IsProneBack = true; // �����ִ� ���� �˸���
			}
			else {
				IsProneBack = false; // ������ִ� ���� �˸���
			}
		}
		else
		{
			// �������� ���� �� StandingDirEnd �� ActorRotation���� ���� ��ŵ�ϴ�.
			// IsTurn���� �ѹ��� StandingDirEnd �� ActorRotation ���� �����ǰ� �߽��ϴ�.
			if (!IsTurn) 
			{ // ���帮�Ⱑ ���� �� ���� �߽� ���� ���մϴ�.

				IsTurn = true;
				StandingDirEnd = Player->GetActorRotation(); // �߽� �� �ֱ�
			}

			if (!(ProneYaw <= 110.0f && ProneYaw >= -110.0f))
			{ // ���� ��ġ�� �ѱ�� ���帮�Ⱑ �ƴ� �����ִ� �� �Դϴ�.
				IsProneBack = true; // �����ִ� ���� �˸���
			}
			else
			{
				IsProneBack = false; // ������ִ� ���� �˸���
			}
		}
	}
	else 
	{ // ���帮�� ���°� �ƴҶ�
		if (Player->GetVelocity().X > 0.0f || Player->GetVelocity().Y > 0.0f)
		{ // ������ ��
			// ��ü�� ���󰡰� �մϴ�.
			StandingDir = Player->GetActorRotation();
			StandingDirEnd = Player->GetActorRotation();
			IsTurn = false;
		}
		else { // �������� ���� ��
			// ��ü ���� �����̰� �մϴ�.
			// IsTurn���� �ѹ��� TurnDirEnd�� ActorRotation ���� �����ǰ� �߽��ϴ�.
			if (!IsTurn) 
			{
				IsTurn = true;
				// �߽� ���� �����մϴ�.
				StandingDirEnd = Player->GetActorRotation();
			}
			else
			{
				if (Yaw >= 70.0f || Yaw <= -70.0f)
				{ // ���� ������ �Ѿ�� ��ü�� �÷��̾��� �þ��������� ���ư��� �մϴ�.
					StandingDirEnd = Player->GetActorRotation();
				}
			}
		}
	}
	// ��ü�� ��ü�� ���� ���� �϶� õõ�� �����̰� �ϱ����� RInterpTo�Լ��� �̿��մϴ�.
	StandingDir = FMath::RInterpTo(StandingDir, StandingDirEnd, GetWorld()->GetDeltaSeconds(), 5.0f);
	FRotator InterpToAngle = (Player->GetActorRotation() - StandingDir).GetNormalized(); // �þ߿� �߽� ���� ����
	ProneYaw = InterpToAngle.Yaw; // ���帮�� ������ �� �� ��ü�� ȸ�� ������ 180�� ���� �޾ƾ� �Ѵ�.
	Yaw = FMath::ClampAngle(InterpToAngle.Yaw, -90.0f, 90.0f);

	// ������� �� Pitch�� ���ϴ� �Լ��Դϴ�.
	ProneRotBlend = GetProneRotBlend(StandingDirEnd.Vector(), Player->GetActorRotation());
}


/*�÷��̾ ���� ������ ������ �������� ��ü�� �����̴� �ִϸ��̼��� �������� ����� ���ؼ���
* �÷��̾ ��� �������� �����̴����� ��Ÿ���� ���� ���� �ʿ��մϴ�. �� ������ ���ϴ� �Լ��Դϴ�.
*/
void UPlayerBody_AnimInstance::GetVeloctyBlend(FVector PlayerVelocity, FRotator PlayerRotator, FVelocityBlend& Current)
{
	PlayerVelocity.Normalize();
	FMatrix RotMatrix = FRotationMatrix(PlayerRotator);
	FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X); // ������ �÷��̾� Rotation X�� ���� (��, ��)
	FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);   // ������ �÷��̾� Rotation Y�� ���� (��, ��)
	FVector NormalizedVel = PlayerVelocity.GetSafeNormal2D();  // �����̰� �ִ� ������ X, Y���� �� Z�� 0.0

	// ���� �÷��̾��� Rotation���� ����� �����̰� �ִ� ������ �������� ������ �÷��̾��� ������ ���� �����̴� ������ ���մϴ�.
	DirForward = FVector::DotProduct(ForwardVector, NormalizedVel);
	DirRight = FVector::DotProduct(RightVector, NormalizedVel);

	/*Current.front = FMath::Clamp(DirForward, 0.0f, 1.0f);
	Current.back = FMath::Abs(FMath::Clamp(DirForward, -1.0f, 0.0f));
	Current.left = FMath::Abs(FMath::Clamp(DirRight, -1.0f, 0.0f));
	Current.right = FMath::Clamp(DirRight, 0.0f, 1.0f);*/

	// ���� ���� ��
	FVelocityBlend SetVel;
	SetVel.front = FMath::Clamp(DirForward, 0.0f, 1.0f);
	SetVel.back = FMath::Abs(FMath::Clamp(DirForward, -1.0f, 0.0f));
	SetVel.left = FMath::Abs(FMath::Clamp(DirRight, -1.0f, 0.0f));
	SetVel.right = FMath::Clamp(DirRight, 0.0f, 1.0f);
	Current.front = FMath::FInterpTo(Current.front, SetVel.front, GetWorld()->GetDeltaSeconds(), 8.0f);
	Current.back = FMath::FInterpTo(Current.back, SetVel.back, GetWorld()->GetDeltaSeconds(), 8.0f);
	Current.left = FMath::FInterpTo(Current.left, SetVel.left, GetWorld()->GetDeltaSeconds(), 8.0f);
	Current.right = FMath::FInterpTo(Current.right, SetVel.right, GetWorld()->GetDeltaSeconds(), 8.0f);

	// ���
	/*UE_LOG(LogTemp, Warning, TEXT("current.front: %f"), current.front);
	UE_LOG(LogTemp, Warning, TEXT("current.back: %f"), current.back);
	UE_LOG(LogTemp, Warning, TEXT("current.left: %f"), current.left);
	UE_LOG(LogTemp, Warning, TEXT("current.right: %f"), current.right);*/
}


/*���帮���� �� ���¸� ��Ÿ���� �Լ�
* ������� �� �� ����(���帮�� or �����ֱ� or �·� ������ֱ� or ��� ������ֱ�)�� ���� ��ü�� ��, ���� �ִϸ��̼��� �޶����ϴ�.
* �ڿ������� ������ ���ؼ��� �� ���¸� ��Ÿ���� ���� �ƴ� ��ġ(��)���� ���¸� ��Ÿ���� ����(����ü)�� �ʿ��մϴ�.
* �� ����ü�� ���ϴ� �Լ��Դϴ�.
* ProneDir�� ���帮���� �߽�(���帮�⸦ ������ ���� Rotation��)�� ��Ÿ���ϴ�.
* PlayerRot�� ������ ActorRotation�� ��Ÿ����
* ProneDir�� �������� PlayerRot�� ���� ������ ���մϴ�
*/
FVelocityBlend UPlayerBody_AnimInstance::GetProneRotBlend(FVector ProneDir, FRotator PlayerRot)
{
	ProneDir.Normalize();
	FMatrix RotMatrix = FRotationMatrix(PlayerRot);
	FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X); // ������ �÷��̾� Rotation X�� ���� (��, ��)
	FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);   // ������ �÷��̾� Rotation Y�� ���� (��, ��)
	FVector NormalizedVel = ProneDir.GetSafeNormal2D();        // ���帮�� �߽��� X, Y���� �� Z�� 0.0

	// ���� �÷��̾ �ٶ󺸰� �ִ� ����� ���帮�� �߽��� ������ �������� ������ ���帮�Ⱑ ��� ������ �ٶ󺸰� �ִ��� ���Ѵ�.
	float ProneDirForward = FVector::DotProduct(ForwardVector, NormalizedVel);
	float ProneDirRight = FVector::DotProduct(RightVector, NormalizedVel);

	//UE_LOG(LogTemp, Warning, TEXT("DirForward: %f"), DirForward);
	//UE_LOG(LogTemp, Warning, TEXT("DirRight: %f"), DirRight);

	// ���� ���� ���� FVelocityBlend ����ü�� ���� ��´�.
	FVelocityBlend SetRot;
	SetRot.front = FMath::Clamp(ProneDirForward, 0.0f, 1.0f);
	SetRot.back = FMath::Abs(FMath::Clamp(ProneDirForward, -1.0f, 0.0f));
	SetRot.left = FMath::Abs(FMath::Clamp(ProneDirRight, -1.0f, 0.0f));
	SetRot.right = FMath::Clamp(ProneDirRight, 0.0f, 1.0f);

	return SetRot;
}

/*�߿� ��� ���� ���� ���� ������ ��ġ�� ���ϴ� �Լ��Դϴ�.
* Trace�� ���� �ٴ��� ��Ʈ�� ��ġ, ������ ���մϴ�.
*/
bool UPlayerBody_AnimInstance::FootIKTracer(AMultiPlayerBase* Player, FName Name, FName Name2, FVector& Loc, FRotator& Rot, float& Distance)
{
	FVector SocketLoc = FVector(Player->BodyMesh->GetSocketLocation(Name).X
		, Player->BodyMesh->GetSocketLocation(Name).Y, Player->BodyMesh->GetSocketLocation(Name2).Z);
	FVector StartTracer = SocketLoc + FVector(0.0f, 0.0f, 40.0f);
	FVector EndTracer = SocketLoc - FVector(0.0f, 0.0f, 40.0f);
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
		EDrawDebugTrace::None, OutHit, true);

	if (hitis) 
	{
		Loc = FVector(Player->BodyMesh->GetSocketLocation(Name2).X
			, Player->BodyMesh->GetSocketLocation(Name2).Y
			, OutHit.ImpactPoint.Z);
		FVector ImpactNomal = OutHit.ImpactNormal;
		float FootRotationRoll = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomal.Y, ImpactNomal.Z));
		float FootRotationPitch = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomal.X, ImpactNomal.Z) * -1.0f);
		Rot = FMath::RInterpTo(Rot, FRotator(FootRotationPitch, 0.0f, FootRotationRoll), GetWorld()->GetDeltaSeconds(), 15.0f);
		Distance = (Player->BodyMesh->GetSocketLocation("root").Z) - OutHit.Location.Z;
	}
	
	return hitis;
}

/* Prone IK�� ���õ� ������ �Լ���
* ������� �� �ٴ��� ������ �°� ���� ������ �����ϴ� ����Դϴ�.
*/
FVector UPlayerBody_AnimInstance::ProneRootIK(AMultiPlayerBase* Player, FName BoneName, FRotator& Rot)
{
	// Trace�� �� ��ġ�� �����մϴ�.
	FVector SocketLoc = Player->BodyMesh->GetSocketLocation(BoneName);
	FVector StartTracer = SocketLoc + FVector(0.0f, 0.0f, 20.0f);
	FVector EndTracer = SocketLoc - FVector(0.0f, 0.0f, 60.0f);
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool hitis = false;
	hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
		EDrawDebugTrace::None, OutHit, true);

	// ���� �ٴ��� ��´ٸ�
	if (hitis)
	{
		// �ٴ��� �ٶ󺸰� �ִ� �����Դϴ�.
		FVector ImpactNomal = OutHit.ImpactNormal;
		//UE_LOG(LogTemp, Warning, TEXT("ImpactNomal: %f, %f, %f"), ImpactNomal.X, ImpactNomal.Y, ImpactNomal.Z);
		
		// ������ ȸ���ؾߵ� ������ ���մϴ�.
		float ProneRotationRoll = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomal.Y, ImpactNomal.Z));
		float ProneRotationPitch = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomal.X, ImpactNomal.Z) * -1.0f);
		Rot = FMath::RInterpTo(Rot, FRotator(ProneRotationPitch, 0.0f, ProneRotationRoll), GetWorld()->GetDeltaSeconds(), 5.0f);

		// �ٴ��� ������ ��ȯ �մϴ�.
		return ImpactNomal;
	}

	// ���� �ٴ��� ���� �ʴ´ٸ� ���θ� ��ȯ�մϴ�.
	return FVector::ZeroVector;
}

FVector UPlayerBody_AnimInstance::ProneMiddleIK(AMultiPlayerBase* Player, FName BoneName, FRotator& Rot, FVector Dir)
{
	// Trace�� �� ��ġ�� �����մϴ�.
	FVector SocketLoc = Player->BodyMesh->GetSocketLocation(BoneName);
	FVector StartTracer = SocketLoc + Dir * 20.0f;
	FVector EndTracer = SocketLoc - Dir * 60.0f;
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool hitis = false;
	hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
		EDrawDebugTrace::None, OutHit, true);

	// ���� �ٴ��� ��´ٸ�
	if (hitis)
	{
		// �ٴ��� �ٶ󺸰� �ִ� �����Դϴ�.
		FVector ImpactNomal = OutHit.ImpactNormal;

		// ������ ȸ���ؾߵ� ������ ���մϴ�.
		// ProneDegrees �Լ��� ����, ���� ���� ���ؿ� ���� ������ ���մϴ�.
		float ProneRotationRoll = ProneDegrees(ImpactNomal.Y, ImpactNomal.Z, Dir.Y, Dir.Z, 1.0f);
		float ProneRotationPitch = ProneDegrees(ImpactNomal.X, ImpactNomal.Z, Dir.X, Dir.Z, -1.0f);
		Rot = FMath::RInterpTo(Rot, FRotator(ProneRotationPitch, 0.0f, ProneRotationRoll), GetWorld()->GetDeltaSeconds(), 5.0f);

		// �ٴ��� ������ ��ȯ �մϴ�.
		return ImpactNomal;
	}

	// ���� �ٴ��� ���� �ʴ´ٸ� ���θ� ��ȯ�մϴ�.
	return FVector::ZeroVector;
}

void UPlayerBody_AnimInstance::ProneEndIK(AMultiPlayerBase* Player, FName BoneName, FRotator& Rot, FVector Dir)
{
	// Trace�� �� ��ġ�� �����մϴ�
	FVector SocketLoc = Player->BodyMesh->GetSocketLocation(BoneName);
	FVector StartTracer = SocketLoc + Dir * 20.0f;
	FVector EndTracer = SocketLoc - Dir * 60.0f;

	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool hitis = false;
	hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
		EDrawDebugTrace::None, OutHit, true);

	// ���� �ٴ��� ��´ٸ�
	if (hitis)
	{
		// �ٴ��� �ٶ󺸰� �ִ� �����Դϴ�.
		FVector ImpactNomal = OutHit.ImpactNormal;

		// ������ ȸ���ؾߵ� ������ ���մϴ�.
		// ProneDegrees �Լ��� ����, ���� ���� ���ؿ� ���� ������ ���մϴ�.
		float ProneRotationRoll = ProneDegrees(ImpactNomal.Y, ImpactNomal.Z, Dir.Y, Dir.Z, 1.0f);
		float ProneRotationPitch = ProneDegrees(ImpactNomal.X, ImpactNomal.Z, Dir.X, Dir.Z, -1.0f);
		Rot = FMath::RInterpTo(Rot, FRotator(ProneRotationPitch, 0.0f, ProneRotationRoll), GetWorld()->GetDeltaSeconds(), 5.0f);
	}
}

/* ����, ���� ������ ���� ���� ��� �Լ��Դϴ�.
* ImpactNomalXY, ImpactNomalZ�� ���� ���� �����Դϴ�.
* DirXY, DirZ ���� ���� �����Դϴ�.
* XY�� x�� ���� y������ �����ϴ� �Ű������Դϴ�. (1�̸� x�� -1�̸� y��)
*/
float UPlayerBody_AnimInstance::ProneDegrees(float ImpactNomalXY, float ImpactNomalZ, float DirXY, float DirZ, float XY)
{
	float Degrees = 0.0f;
	// ���� ���� �������� ���� ����ϱ�
	// ���� ������ z�� ���� ���ٸ� ���� ������ �������� ����մϴ�.
	// z���� ũ�ٴ� ���� �� ��ŭ ���� ����Ű�� �ִٴ� ���̱� ������ z���� �������� ���մϴ�.
	if (DirZ > ImpactNomalZ)
	{
		Degrees = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomalXY, DirZ) * XY);
	}
	else if (DirZ < ImpactNomalZ)
	{
		Degrees = FMath::RadiansToDegrees(FMath::Atan2(DirXY, ImpactNomalZ) * -1.0f * XY);
	}

	// ���࿡ �Ѵ� ���� ���̶�� 0���� ��ȯ�մϴ�.
	return Degrees;
}

/*�ִ� ��Ƽ��Ƽ
*/
void UPlayerBody_AnimInstance::AnimNotify_ClimbEnd()
{
	if (VaultDelegate.IsBound()) VaultDelegate.Execute();
}

void UPlayerBody_AnimInstance::AnimNotify_VaultEnd()
{
	if (ClimbDelegate.IsBound()) ClimbDelegate.Execute();
}

/*��Ÿ�� �Լ���
*/
void UPlayerBody_AnimInstance::PlayVaultMontage()
{
	Montage_Play(Vault_Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
}
void UPlayerBody_AnimInstance::PlayClimbMontage()
{
	Montage_Play(Climb_Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
}
void UPlayerBody_AnimInstance::PlaySlidingMontage()
{
	Montage_Play(Sliding_Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
}
void UPlayerBody_AnimInstance::PlayFireMontage()
{
}
void UPlayerBody_AnimInstance::PlayMeleeMontage()
{
	int RandNum = FMath::RandRange(1, 2);
	FString hitString = "Melee";
	hitString.AppendInt(RandNum);
	FName hitName = (*hitString);

	Montage_Play(Melee_Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
	Montage_JumpToSection(hitName, Melee_Montage);
}
void UPlayerBody_AnimInstance::StopMontage()
{
	Montage_Stop(0.2f);
}



/*void UPlayerBody_AnimInstance::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UPlayerBody_AnimInstance, upperPitch, COND_SkipOwner);
}*/
