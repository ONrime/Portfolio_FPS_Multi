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

		// 일반적인 상태에서의 혹은 엎드렸을 때의 상체 값(UpperYaw, UpperProneYaw)을 구하는 함수입니다.
		TurnBodyYaw(Player, UpperYaw, UpperProneYaw);
		//UpperYaw = player->GetUpperYaw();
		//UpperProneYaw = player->GetProneYaw();
		//ProneRotBlend = player->GetProneRotBlend();
		FRotator InterpToAngle = (Player->GetControllerRot() - Player->GetActorRotation()).GetNormalized();
		UpperPitch = FMath::ClampAngle(InterpToAngle.Pitch, -90.0f, 90.0f); // 위, 아래의 값
		//UpperPitch = player->GetUpperPitch();
		
		PlayerSpeed = Player->GetVelocity().Size(); // 플레이어의 속도
		// 시점을 기준으로 입력된 방향을 찾는 함수입니다. 
		GetVeloctyBlend(Player->GetVelocity(), Player->GetActorRotation(), VelocityBlend);

		DownStateNClass = Player->DownStateNClass;   // 하체의 상태를 나타내는 변수입니다.
		DownStateBClass = Player->DownStateBClass;   // 하체의 전 상태를 나타내는 변수입니다.
		UpperStateNClass = Player->UpperStateNClass; // 상체의 상태를 나타내는 변수입니다.

		IsJumped = Player->IsJumped;                             // 점프를 했을때의 상태
		IsFalling = Player->GetMovementComponent()->IsFalling(); // 추락의 상태
		IsHandUp = Player->IsHandUp;                             // 벽에 히트되어 손을 올려야 되는 상태
		IsProne = Player->GetIsProne();                          // 엎드렸을 때(바뀌고 있을때가 아닌 엎드리기를 시작했을 때)

		/* 엄폐 기능
		* 좌, 우의 시점에 따라 척추의 휘어지는 정도를 정하는 구간입니다.
		*/
		if (Player->IsCoverLeft) // 왼쪽 엄폐일 때
		{
			// UpperYaw(좌, 우)의 값에 따라 엄폐하는 척추값을 구합니다.
			float ClampAngle = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, -50.0f), FVector2D(-Player->CoverAngleLeft, 5.0f), UpperYaw);
			CoverAngle = FMath::FInterpTo(CoverAngle, ClampAngle, GetWorld()->DeltaTimeSeconds, 12.0f);
		}
		if (Player->IsCoverRight) // 오른쪽 엄폐일 때
		{
			float ClampAngle = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 50.0f), FVector2D(Player->CoverAngleRight, -5.0f), UpperYaw);
			CoverAngle = FMath::FInterpTo(CoverAngle, ClampAngle, GetWorld()->DeltaTimeSeconds, 12.0f);
		}
		if (!Player->IsCoverRight && !Player->IsCoverLeft && CoverAngle != 0.0f)
		{
			// 엄폐를 안할때 0으로 보간합니다.
			CoverAngle = FMath::FInterpTo(CoverAngle, 0.0f, GetWorld()->DeltaTimeSeconds, 10.0f);
		}

		/* FootIK 기능
		* 가상 뼈를 통해 고정된 발위치 값을 구할수 있기 때문에 이동중에도 안정적인 위치값을 구할수 있습니다.
		*/
		float LeftDistance = 0.0f, RightDistance = 0.0f, RootDistance = 0.0f;

		/* LeftFootAlpha, RightFootAlpha는 지면에 닿았는지를 나타내며 float를 한 이유는
		* AnimBP내의 Transform Bone과 Two Bone IK에서 Alpha로 사용하기 위해 float로 만들었습니다.
		* FootIKTracer()함수에서 발이 바닥에 닿았는지(LeftFootAlpha, RightFootAlpha)
		* , 닿았다면 바닥의 각도에 따른 발의 각도(LeftFootRot, RightFootRot)는 어떻게 되는지
		* , 닿은 바닥의 위치에 따라 발의 좌표(LeftFootLoc, RightFootLoc)가 어디에 있어야 하는지
		* , 현재 발의 위치와 바닥간의 거리(LeftDistance, RightDistance)가 어떻게 되는지를 구하는 함수입니다.
		* FootIK로 인해 위치(Location)값이 변경될 발을 대신해서 
		* 가상뼈(VB pelvis_foot_l, VB pelvis_foot_r)를 사용해 변경되지 않은 발 위치값을 구하여
		* 이동할 때의 발 위치를 계산합니다.
		*/
		LeftFootAlpha = FootIKTracer(Player, "foot_l", "VB pelvis_foot_l", LeftFootLoc, LeftFootRot, LeftDistance);
		RightFootAlpha = FootIKTracer(Player, "foot_r", "VB pelvis_foot_r", RightFootLoc, RightFootRot, RightDistance);

		/*
		* 
		*/
		if (IsFalling) // 추락중일때 IK 끄게 하기
		{
			RootLock = true;
			RootDistance = 0.0f;
		}
		if(!RootLock){
			// 차이가 큰 거리를 RootDistance로 등록한다.
			LeftDistance >= RightDistance ? RootDistance = LeftDistance : RootDistance = RightDistance;
			RootDistance *= -1;
		}
		RootZLoc = FMath::FInterpTo(RootZLoc, RootDistance, GetWorld()->DeltaTimeSeconds, 12.0f);
		Player->SetRootDistance(RootZLoc); // 카메라 위치를 조정하기위해 플레이어에게 조정된 Root의 위치를 보냅니다.

		if (RootZLoc >= -0.5f) RootLock = false; // 잠금 해제
		IsFalling ? RootLocAlpha = 0.0f : RootLocAlpha = 1.0f; // 추락여부에 따라 루트의 알파값을 정합니다.

		// 가상뼈(원래 발 위치값)와 루트 Z축 위치의 차이값으로 이동할때의 위치값을 구합니다.
		// LeftFootLoc.Z(바닥의 위치) 
		// + (가상뼈 위치(IK없을 때의 발 위치) - 루트 위치 - 7.0f(약간의 조정)) --> IK없을 때의 바닥에서 부터 발과의 거리값 (가만히 있을 때는 0~1이지만 이동 할때는 값이 커집니다.)
		// + RootZLoc(루트와 제일 낮은 바닥의 거리) - 3.0f(약간의 조정)
		LeftFootLoc = FVector(LeftFootLoc.X, LeftFootLoc.Y
			, LeftFootLoc.Z + (Player->BodyMesh->GetSocketLocation("VB pelvis_foot_l").Z
				- (Player->BodyMesh->GetSocketLocation("root").Z - 7.0f)) + (RootZLoc * -1) - 3.0f);

		RightFootLoc = FVector(RightFootLoc.X, RightFootLoc.Y
			, RightFootLoc.Z + (Player->BodyMesh->GetSocketLocation("VB pelvis_foot_r").Z
				- (Player->BodyMesh->GetSocketLocation("root").Z - 7.0f)) + (RootZLoc * -1) - 3.0f);
		// 걷기를 할 때 가상 뼈와 루트 사이의 계산이 이루어져야한다.

		/* Prone IK 계산
		* 
		*/
		if (Player->GetIsProne())
		{
			// Root
			FVector RootDir = ProneRootIK(Player, "pelvis", ProneIK_Pelvis_Rot);
			// 척추
			FVector SpineDir = ProneMiddleIK(Player, "ProneSpineLoc", ProneIK_Upper_Rot, RootDir);
			// 무릎
			FVector Calf_Rgiht_Dir = ProneMiddleIK(Player, "ProneCalfRightLoc", ProneIK_Right_Knee_Rot, RootDir);
			FVector Calf_Left_Dir = ProneMiddleIK(Player, "ProneCalfLeftLoc", ProneIK_Left_Knee_Rot, RootDir);

			// 발
			ProneEndIK(Player, "ProneFootRightLoc", ProneIK_Right_Foot_Rot, Calf_Rgiht_Dir);
			ProneEndIK(Player, "ProneFootLeftLoc", ProneIK_Left_Foot_Rot, Calf_Left_Dir);
			// 팔꿈치
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

/* 플레이어의 하체는 고정되고 상체만 시야에 따라 움직이게 하기 위해 필요한 값을 반환하는 함수입니다.
* 기본적으로 플레이어는 시야에 따라 같이 움직입니다.
* 만약 플레이어의 시야가 움직이는 만큼 하체를 반대로 돌린다면 하체가 고정된 상태로 상체만 움직이는 모습을 볼 수 있습니다.
* 이대로만 한다면 하체가 고정된체 상체가 뒤를 바라보는 공포영화와 같은 상황이 펼쳐지기 때문에
* 일정 범위를 정하고 범위를 넘어가면 하체가 플레이어가 바라보는 지점으로 회전하게 합니다.
* 코드로 설명하면 플레이어가 움직이지 않으면 중심점(중심점 = StandingDirEnd)을 정하고
* 그 중심점과 시야와의 차이를 구해 이 값(Yaw)으로 하체를 반대로 움직이게 합니다.
* 
*  이제 여기서 엎드리기 기능도 같이 다룹니다.
* 다른 점이 있다면 일정 범위를 넘어가면 중심점(중심점 = StandingDirEnd)을 180도 회전하게 하는 것과 누워있는지 엎드려있는지만 판단만 합니다.
*/
void UPlayerBody_AnimInstance::TurnBodyYaw(AMultiPlayerBase* Player, float& Yaw, float& ProneYaw)
{
	if (Player->GetIsProne())
	{ // 플레이어가 엎드리기를 시작 할때(바뀌는 중이 아닌 시작)

		if (Player->GetVelocity().Size() > 3.0f) // 움직일 때
		{
			// 움직일 때 고정된 방향(TurnDirEnd)을 움직이는 방향으로 바꿉니다.
			StandingDirEnd = Player->GetActorRotation();

			if (!(ProneYaw <= 110.0f && ProneYaw >= -110.0f))
			{ // 일정 수치를 넘기면 엎드리기가 아닌 누워있는 것 입니다.

				// 만약 등이 바닥을 향하게 누워있다면 중심 값을 반대로 하기
				StandingDirEnd = FRotator(StandingDirEnd.Pitch, StandingDirEnd.Yaw + 180.0f, StandingDirEnd.Roll);
				IsProneBack = true; // 누워있는 상태 알리기
			}
			else {
				IsProneBack = false; // 엎드려있는 상태 알리기
			}
		}
		else
		{
			// 움직이지 않을 때 StandingDirEnd 을 ActorRotation으로 고정 시킵니다.
			// IsTurn으로 한번만 StandingDirEnd 를 ActorRotation 으로 고정되게 했습니다.
			if (!IsTurn) 
			{ // 엎드리기가 시작 될 때만 중심 값을 구합니다.

				IsTurn = true;
				StandingDirEnd = Player->GetActorRotation(); // 중심 값 넣기
			}

			if (!(ProneYaw <= 110.0f && ProneYaw >= -110.0f))
			{ // 일정 수치를 넘기면 엎드리기가 아닌 누워있는 것 입니다.
				IsProneBack = true; // 누워있는 상태 알리기
			}
			else
			{
				IsProneBack = false; // 엎드려있는 상태 알리기
			}
		}
	}
	else 
	{ // 엎드리기 상태가 아닐때
		if (Player->GetVelocity().X > 0.0f || Player->GetVelocity().Y > 0.0f)
		{ // 움직일 때
			// 하체가 따라가게 합니다.
			StandingDir = Player->GetActorRotation();
			StandingDirEnd = Player->GetActorRotation();
			IsTurn = false;
		}
		else { // 움직이지 않을 때
			// 하체 따로 움직이게 합니다.
			// IsTurn으로 한번만 TurnDirEnd를 ActorRotation 으로 고정되게 했습니다.
			if (!IsTurn) 
			{
				IsTurn = true;
				// 중심 값을 설정합니다.
				StandingDirEnd = Player->GetActorRotation();
			}
			else
			{
				if (Yaw >= 70.0f || Yaw <= -70.0f)
				{ // 일정 범위를 넘어가면 하체가 플레이어의 시야지점으로 돌아가게 합니다.
					StandingDirEnd = Player->GetActorRotation();
				}
			}
		}
	}
	// 하체가 상체를 따라 움직 일때 천천히 움직이게 하기위해 RInterpTo함수를 이용합니다.
	StandingDir = FMath::RInterpTo(StandingDir, StandingDirEnd, GetWorld()->GetDeltaSeconds(), 5.0f);
	FRotator InterpToAngle = (Player->GetActorRotation() - StandingDir).GetNormalized(); // 시야와 중심 값의 차이
	ProneYaw = InterpToAngle.Yaw; // 엎드리기 상태일 때 는 전체를 회전 함으로 180도 값을 받아야 한다.
	Yaw = FMath::ClampAngle(InterpToAngle.Yaw, -90.0f, 90.0f);

	// 엎드렸을 때 Pitch값 구하는 함수입니다.
	ProneRotBlend = GetProneRotBlend(StandingDirEnd.Vector(), Player->GetActorRotation());
}


/*플레이어가 보는 시점의 방향을 기준으로 하체의 움직이는 애니메이션이 정해지는 기능을 위해서는
* 플레이어가 어느 방향으로 움직이는지를 나타내는 방향 값이 필요합니다. 그 방향을 구하는 함수입니다.
*/
void UPlayerBody_AnimInstance::GetVeloctyBlend(FVector PlayerVelocity, FRotator PlayerRotator, FVelocityBlend& Current)
{
	PlayerVelocity.Normalize();
	FMatrix RotMatrix = FRotationMatrix(PlayerRotator);
	FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X); // 현재의 플레이어 Rotation X축 방향 (전, 후)
	FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);   // 현재의 플레이어 Rotation Y축 방향 (좌, 우)
	FVector NormalizedVel = PlayerVelocity.GetSafeNormal2D();  // 움직이고 있는 방향의 X, Y축의 값 Z는 0.0

	// 현재 플레이어의 Rotation축의 방향과 움직이고 있는 방향을 내적으로 현재의 플레이어의 시점에 따른 움직이는 방향을 구합니다.
	DirForward = FVector::DotProduct(ForwardVector, NormalizedVel);
	DirRight = FVector::DotProduct(RightVector, NormalizedVel);

	/*Current.front = FMath::Clamp(DirForward, 0.0f, 1.0f);
	Current.back = FMath::Abs(FMath::Clamp(DirForward, -1.0f, 0.0f));
	Current.left = FMath::Abs(FMath::Clamp(DirRight, -1.0f, 0.0f));
	Current.right = FMath::Clamp(DirRight, 0.0f, 1.0f);*/

	// 보간 했을 때
	FVelocityBlend SetVel;
	SetVel.front = FMath::Clamp(DirForward, 0.0f, 1.0f);
	SetVel.back = FMath::Abs(FMath::Clamp(DirForward, -1.0f, 0.0f));
	SetVel.left = FMath::Abs(FMath::Clamp(DirRight, -1.0f, 0.0f));
	SetVel.right = FMath::Clamp(DirRight, 0.0f, 1.0f);
	Current.front = FMath::FInterpTo(Current.front, SetVel.front, GetWorld()->GetDeltaSeconds(), 8.0f);
	Current.back = FMath::FInterpTo(Current.back, SetVel.back, GetWorld()->GetDeltaSeconds(), 8.0f);
	Current.left = FMath::FInterpTo(Current.left, SetVel.left, GetWorld()->GetDeltaSeconds(), 8.0f);
	Current.right = FMath::FInterpTo(Current.right, SetVel.right, GetWorld()->GetDeltaSeconds(), 8.0f);

	// 출력
	/*UE_LOG(LogTemp, Warning, TEXT("current.front: %f"), current.front);
	UE_LOG(LogTemp, Warning, TEXT("current.back: %f"), current.back);
	UE_LOG(LogTemp, Warning, TEXT("current.left: %f"), current.left);
	UE_LOG(LogTemp, Warning, TEXT("current.right: %f"), current.right);*/
}


/*엎드리기의 각 상태를 나타내는 함수
* 엎드렸을 때 각 상태(엎드리기 or 누워있기 or 좌로 엎드려있기 or 우로 엎드려있기)에 따라 상체의 상, 하의 애니메이션이 달라집니다.
* 자연스러운 블랜딩을 위해서는 각 상태를 나타내는 값이 아닌 수치(값)으로 상태를 나타내는 변수(구조체)가 필요합니다.
* 그 구조체를 구하는 함수입니다.
* ProneDir는 엎드리기의 중심(엎드리기를 시작할 때의 Rotation값)을 나타냅니다.
* PlayerRot는 현재의 ActorRotation을 나타내고
* ProneDir를 기준으로 PlayerRot에 따른 방향을 구합니다
*/
FVelocityBlend UPlayerBody_AnimInstance::GetProneRotBlend(FVector ProneDir, FRotator PlayerRot)
{
	ProneDir.Normalize();
	FMatrix RotMatrix = FRotationMatrix(PlayerRot);
	FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X); // 현재의 플레이어 Rotation X축 방향 (전, 후)
	FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);   // 현재의 플레이어 Rotation Y축 방향 (좌, 우)
	FVector NormalizedVel = ProneDir.GetSafeNormal2D();        // 엎드리기 중심의 X, Y축의 값 Z는 0.0

	// 현재 플레이어가 바라보고 있는 방향과 엎드리기 중심의 방향을 내적으로 현재의 엎드리기가 어느 방향을 바라보고 있는지 구한다.
	float ProneDirForward = FVector::DotProduct(ForwardVector, NormalizedVel);
	float ProneDirRight = FVector::DotProduct(RightVector, NormalizedVel);

	//UE_LOG(LogTemp, Warning, TEXT("DirForward: %f"), DirForward);
	//UE_LOG(LogTemp, Warning, TEXT("DirRight: %f"), DirRight);

	// 구한 값에 따라 FVelocityBlend 구조체에 값을 담는다.
	FVelocityBlend SetRot;
	SetRot.front = FMath::Clamp(ProneDirForward, 0.0f, 1.0f);
	SetRot.back = FMath::Abs(FMath::Clamp(ProneDirForward, -1.0f, 0.0f));
	SetRot.left = FMath::Abs(FMath::Clamp(ProneDirRight, -1.0f, 0.0f));
	SetRot.right = FMath::Clamp(ProneDirRight, 0.0f, 1.0f);

	return SetRot;
}

/*발에 닿는 땅에 따라 발의 각도와 위치를 구하는 함수입니다.
* Trace를 통해 바닥의 히트및 위치, 각도를 구합니다.
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

/* Prone IK에 관련된 변수와 함수들
* 엎드렸을 때 바닥의 각도에 맞게 몸의 각도를 변경하는 기능입니다.
*/
FVector UPlayerBody_AnimInstance::ProneRootIK(AMultiPlayerBase* Player, FName BoneName, FRotator& Rot)
{
	// Trace를 쏠 위치를 지정합니다.
	FVector SocketLoc = Player->BodyMesh->GetSocketLocation(BoneName);
	FVector StartTracer = SocketLoc + FVector(0.0f, 0.0f, 20.0f);
	FVector EndTracer = SocketLoc - FVector(0.0f, 0.0f, 60.0f);
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool hitis = false;
	hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
		EDrawDebugTrace::None, OutHit, true);

	// 만약 바닥이 닿는다면
	if (hitis)
	{
		// 바닥이 바라보고 있는 방향입니다.
		FVector ImpactNomal = OutHit.ImpactNormal;
		//UE_LOG(LogTemp, Warning, TEXT("ImpactNomal: %f, %f, %f"), ImpactNomal.X, ImpactNomal.Y, ImpactNomal.Z);
		
		// 관절이 회전해야될 각도를 구합니다.
		float ProneRotationRoll = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomal.Y, ImpactNomal.Z));
		float ProneRotationPitch = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomal.X, ImpactNomal.Z) * -1.0f);
		Rot = FMath::RInterpTo(Rot, FRotator(ProneRotationPitch, 0.0f, ProneRotationRoll), GetWorld()->GetDeltaSeconds(), 5.0f);

		// 바닥의 방향을 반환 합니다.
		return ImpactNomal;
	}

	// 만약 바닥이 닿지 않는다면 제로를 반환합니다.
	return FVector::ZeroVector;
}

FVector UPlayerBody_AnimInstance::ProneMiddleIK(AMultiPlayerBase* Player, FName BoneName, FRotator& Rot, FVector Dir)
{
	// Trace를 쏠 위치를 지정합니다.
	FVector SocketLoc = Player->BodyMesh->GetSocketLocation(BoneName);
	FVector StartTracer = SocketLoc + Dir * 20.0f;
	FVector EndTracer = SocketLoc - Dir * 60.0f;
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool hitis = false;
	hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
		EDrawDebugTrace::None, OutHit, true);

	// 만약 바닥이 닿는다면
	if (hitis)
	{
		// 바닥이 바라보고 있는 방향입니다.
		FVector ImpactNomal = OutHit.ImpactNormal;

		// 관절이 회전해야될 각도를 구합니다.
		// ProneDegrees 함수로 상위, 하위 계층 기준에 따른 각도를 구합니다.
		float ProneRotationRoll = ProneDegrees(ImpactNomal.Y, ImpactNomal.Z, Dir.Y, Dir.Z, 1.0f);
		float ProneRotationPitch = ProneDegrees(ImpactNomal.X, ImpactNomal.Z, Dir.X, Dir.Z, -1.0f);
		Rot = FMath::RInterpTo(Rot, FRotator(ProneRotationPitch, 0.0f, ProneRotationRoll), GetWorld()->GetDeltaSeconds(), 5.0f);

		// 바닥의 방향을 반환 합니다.
		return ImpactNomal;
	}

	// 만약 바닥이 닿지 않는다면 제로를 반환합니다.
	return FVector::ZeroVector;
}

void UPlayerBody_AnimInstance::ProneEndIK(AMultiPlayerBase* Player, FName BoneName, FRotator& Rot, FVector Dir)
{
	// Trace를 쏠 위치를 지정합니다
	FVector SocketLoc = Player->BodyMesh->GetSocketLocation(BoneName);
	FVector StartTracer = SocketLoc + Dir * 20.0f;
	FVector EndTracer = SocketLoc - Dir * 60.0f;

	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool hitis = false;
	hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
		EDrawDebugTrace::None, OutHit, true);

	// 만약 바닥이 닿는다면
	if (hitis)
	{
		// 바닥이 바라보고 있는 방향입니다.
		FVector ImpactNomal = OutHit.ImpactNormal;

		// 관절이 회전해야될 각도를 구합니다.
		// ProneDegrees 함수로 상위, 하위 계층 기준에 따른 각도를 구합니다.
		float ProneRotationRoll = ProneDegrees(ImpactNomal.Y, ImpactNomal.Z, Dir.Y, Dir.Z, 1.0f);
		float ProneRotationPitch = ProneDegrees(ImpactNomal.X, ImpactNomal.Z, Dir.X, Dir.Z, -1.0f);
		Rot = FMath::RInterpTo(Rot, FRotator(ProneRotationPitch, 0.0f, ProneRotationRoll), GetWorld()->GetDeltaSeconds(), 5.0f);
	}
}

/* 상위, 하위 계층에 따른 각도 계산 함수입니다.
* ImpactNomalXY, ImpactNomalZ는 하위 계층 변수입니다.
* DirXY, DirZ 상위 계층 변수입니다.
* XY는 x축 인지 y축인지 구분하는 매개변수입니다. (1이면 x축 -1이면 y축)
*/
float UPlayerBody_AnimInstance::ProneDegrees(float ImpactNomalXY, float ImpactNomalZ, float DirXY, float DirZ, float XY)
{
	float Degrees = 0.0f;
	// 제일 높은 기준으로 각도 계산하기
	// 상위 계층의 z축 값이 높다면 상위 계층의 기준으로 계산합니다.
	// z축이 크다는 것은 그 만큼 위를 가리키고 있다는 뜻이기 때문에 z축을 기준으로 비교합니다.
	if (DirZ > ImpactNomalZ)
	{
		Degrees = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomalXY, DirZ) * XY);
	}
	else if (DirZ < ImpactNomalZ)
	{
		Degrees = FMath::RadiansToDegrees(FMath::Atan2(DirXY, ImpactNomalZ) * -1.0f * XY);
	}

	// 만약에 둘다 같은 값이라면 0도를 반환합니다.
	return Degrees;
}

/*애님 노티파티
*/
void UPlayerBody_AnimInstance::AnimNotify_ClimbEnd()
{
	if (VaultDelegate.IsBound()) VaultDelegate.Execute();
}

void UPlayerBody_AnimInstance::AnimNotify_VaultEnd()
{
	if (ClimbDelegate.IsBound()) ClimbDelegate.Execute();
}

/*몽타주 함수들
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
