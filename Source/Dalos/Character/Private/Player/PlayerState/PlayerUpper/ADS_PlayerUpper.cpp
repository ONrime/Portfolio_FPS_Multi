// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/ADS_PlayerUpper.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/Armed_PlayerUpper.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/UnArmed_PlayerUpper.h"
#include "Dalos/Widget/Public/MultiPlayer_HUD.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Dalos/Weapone/Public/WeaponeBase.h"

UADS_PlayerUpper::UADS_PlayerUpper()
{
	temp = nullptr;
	UpperState = EPlayerUpperState::ADS;

	static ConstructorHelpers::FObjectFinder<UCurveFloat>ADSCURVE(TEXT("CurveFloat'/Game/Curve/ADSChange.ADSChange'"));
	if (ADSCURVE.Succeeded()) {
		ADSCurve = ADSCURVE.Object;
	}
}

// �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
UPlayerUpperStateBase* UADS_PlayerUpper::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // �÷��̾� ��Ʈ�ѷ� �Դϴ�.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // ��Ʈ�ѷ��� �Է��� �����ϴ� ��ü�Դϴ�.

	// �� ������ Ű�� �����ɴϴ�.
	TArray <FInputActionKeyMapping> ActionUnArmed = PlayerInput->GetKeysForAction(TEXT("UnArmed")); // ���� ����(�� ����)Ű
	TArray <FInputActionKeyMapping> ActionADS = PlayerInput->GetKeysForAction(TEXT("ADS"));         // ���� ���� Ű

	if (PlayerInput->IsPressed(ActionUnArmed[0].Key)) 
	{
		temp = NewObject<UUnArmed_PlayerUpper>(this, UUnArmed_PlayerUpper::StaticClass()); // �� ���� ���� ��ü ����
	}
	else if (PlayerInput->InputKey(ActionADS[0].Key, EInputEvent::IE_Released, 1.0f, true)) 
	{
		temp = NewObject<UArmed_PlayerUpper>(this, UArmed_PlayerUpper::StaticClass());     // ���� ���� ��ü ����
	}

	return temp;
}

// ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
UPlayerUpperStateBase* UADS_PlayerUpper::SendHandleInput(EPlayerPress Press)
{
	if (Press == EPlayerPress::UNARMED)
	{
		temp = NewObject<UUnArmed_PlayerUpper>(this, UUnArmed_PlayerUpper::StaticClass());
	}
	else if (Press == EPlayerPress::ADS)
	{
		temp = NewObject<UArmed_PlayerUpper>(this, UArmed_PlayerUpper::StaticClass());
	}
	return temp;
}

// ���°� ���� �� �� �۵��ϴ� �Լ�
void UADS_PlayerUpper::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	UE_LOG(LogTemp, Warning, TEXT("ADS: StateStart"));

	PlayerCamera = Player->FollowCamera; // �÷��̾� ī�޶� ��������

	/* ���� ������ �� �� ī�޶� ����(FOV�� ����)�� �˴ϴ�.
	* ������ �Ǵ� ������ Timeline�� ����Ͽ� Curve�� ���� ������ �ӵ��� ������ �ǰ��մϴ�.
	*/
	if (ADSCurve != nullptr)
	{ // Curve�� �����ϸ�
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, "SetADS"); // Timeline�� ���� Update�� �Լ��� ���ε��մϴ�.
		FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, "SetADSFinish"); // Timeline�� ���� �� �۵��� �Լ��� ���ε��մϴ�.
		ADSTimeline.AddInterpFloat(ADSCurve, TimelineCallback); // Curve�� Timeline ����
		ADSTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
		ADSTimeline.SetPlayRate(4.0f);
		ADSTimeline.PlayFromStart(); // Timeline�� �����մϴ�.
	}
	if (Player->IsLocallyControlled()) // ���ڼ��� ������ �մϴ�.
	{
		if (Player->GetHUD()->CrossHairHideCheck.IsBound()) Player->GetHUD()->CrossHairHideCheck.Execute(true);
	}
}

// Tick���� �۵��ϴ� �Լ�
void UADS_PlayerUpper::StateUpdate(AMultiPlayerBase* Player)
{
	ADSTimeline.TickTimeline(GetWorld()->DeltaTimeSeconds);

	float ViewSize = 50.0f;
	float LeftDistance = 0.0f;
	float RightDistance = 0.0f;
	Player->IsHandUp = HandUpTracer(Player);
	if (Player->IsHandUp == true) // ��ֹ��� ������ ���� �ö� �ִٸ�
	{
		//UE_LOG(LogTemp, Warning, TEXT("IsHandUp"));

		// ���� ���¿����� �� ���� ��� (���߿� �ϼ�����)
		/*if (player->IsCoverUp == false && player->GetDownState()->GetEState() == EPlayerDownState::CROUCH)
		{
			player->IsCoverUp = CoverUpTracer(player, player->ArmMesh->GetSocketLocation("CoverLoc"));
			UE_LOG(LogTemp, Warning, TEXT("IsHandUp"));
		}*/

		if (Player->IsCoverLeft == false) 
		{
			// ���� ����Trace�� Ȯ���ؼ� ������ �Ÿ��� ���մϴ�.
			Player->IsCoverLeft = CoverTracer(Player, Player->BodyMesh->GetSocketLocation("CameraLoc") + Player->GetMesh()->GetRightVector() * -5.0f
				, Player->CoverAngleLeft, LeftDistance, 1.0f);
			//UE_LOG(LogTemp, Warning, TEXT("CoverAngleLeft2: %f"), Player->CoverAngleLeft);
		}

		if (Player->IsCoverRight == false)
		{
			// ������ ����Trace�� Ȯ���ؼ� ������ �Ÿ��� ���մϴ�.
			Player->IsCoverRight = CoverTracer(Player, Player->BodyMesh->GetSocketLocation("CameraLoc") + Player->GetMesh()->GetRightVector() * 5.0f
				, Player->CoverAngleRight, RightDistance, -1.0f);
			//if (player->IsLocallyControlled()) UE_LOG(LogTemp, Warning, TEXT("CoverAngleRight2: %f"), Player->CoverAngleRight);
		}
		if ((Player->IsCoverLeft || Player->IsCoverRight)) // ���� ������ �ϰ� �ִٸ�
		{
			if (Player->CoverAngleRight > 0.0f && Player->CoverAngleLeft > 0.0f)
			{ // ���� �� ������ ������ ���´ٸ� ���� ���� ������ ���������� �մϴ�.
				//UE_LOG(LogTemp, Warning, TEXT("LeftDistance: %f"), LeftDistance);
				//UE_LOG(LogTemp, Warning, TEXT("RightDistance: %f"), RightDistance);
				LeftDistance > RightDistance ? Player->CoverAngleRight = 0.0f : Player->CoverAngleLeft = 0.0f;
			}
		}
	}

	/* ī�޶� X��(Roll)ȸ���� ����մϴ�.
	* ���������� �ϸ� ī�޶� ȸ���� �մϴ�.
	*/
	if (Player->IsLocallyControlled()) // �÷��̾� �����̶��
	{
		FRotator InterpToAngle = (Player->GetActorRotation() - CoverRot).GetNormalized();
		float Angleyaw = FMath::ClampAngle(InterpToAngle.Yaw, -90.0f, 90.0f);
		float NowRoll = 0.0f;
		if (Player->IsHandUp == false)
		{
			NowRoll = 0.0f;
		}

		if ((Player->IsCoverLeft || Player->IsCoverRight)) // ���� ������ �ϰ� �ִٸ�
		{
			// �ٶ� ���� ���⿡ ���� ȸ���� �����մϴ�. (������ 20 ������ 0)
			if (Player->CoverAngleLeft > 0.0f) {
				NowRoll = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, -40.0f), FVector2D(-20.0f, 0.0f), Angleyaw);
			}
			if (Player->CoverAngleRight > 0.0f) {
				NowRoll = FMath::GetMappedRangeValueClamped(FVector2D(40.0f, 0.0f), FVector2D(0.0f, 20.0f), Angleyaw);
			}

			// ī�޶� X��(Roll)ȸ��
			FRotator NowFollowCameraRot = Player->FollowCamera->GetRelativeRotation();
			Player->FollowCamera->SetRelativeRotation(FRotator(NowFollowCameraRot.Pitch, NowFollowCameraRot.Yaw
				, FMath::FInterpTo(NowFollowCameraRot.Roll, NowRoll, GetWorld()->DeltaTimeSeconds, 5.0f)));

			// ���� ���� ī�޶� Z�� ��ġ ����
			CoverLocZ = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 50.0f), FVector2D(-30.0f, 0.0f), FMath::Abs(Angleyaw));
		}
		//UE_LOG(LogTemp, Warning, TEXT("CoverAngle: %f"), Player->CoverAngleLeft + Player->CoverAngleRight);
		//UE_LOG(LogTemp, Warning, TEXT("Pitch: %f"), Pitch);
	}
	
}

// ���°� ���� �� �۵��ϴ� �Լ�
void UADS_PlayerUpper::StateEnd(AMultiPlayerBase* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("ADS: StateEnd"));
	if (Player->IsLocallyControlled()) // ���ڼ��� ���̰� �մϴ�.
	{
		if (Player->GetHUD()->CrossHairHideCheck.IsBound()) Player->GetHUD()->CrossHairHideCheck.Execute(false);
	}

	//Player->SpringArm->SetRelativeLocation(FVector(25.0f, 0.0f, 65.0f));
	//Player->SpringArm->AttachToComponent(player->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	
	// �ʱ�ȭ�� �մϴ�.
	Player->IsCoverUp = false;
	Player->IsCoverLeft = false;
	Player->IsCoverRight = false;
	Player->CoverAngleLeft = 0.0f;
	Player->CoverAngleRight = 0.0f;
}

// ������ UClass ��ȯ
UClass* UADS_PlayerUpper::GetState()
{
	return UADS_PlayerUpper::StaticClass();
}

/* ���� ��ư�� ������ �� �۵��ϴ� �Լ��Դϴ�.
* ����� �ܹ�, 3����, ���� �����ϰ� �߻��ϴ� ��ɸ� �־� �ξ����ϴ�.
*/
void UADS_PlayerUpper::PlayerFire(AMultiPlayerBase* Player, AWeaponeBase* Equip, bool& IsAuto, float& Count, FVector Loc, FRotator Rot, FVector BulletLoc)
{
	if (Equip) {
		if (Equip->GetWeaponeLever() == WEAPONLEVER::FULLAUTO) {
			IsAuto = true;
			Count = 0.0f;
		}
		else if (Equip->GetWeaponeLever() == WEAPONLEVER::SINGLEFIRE) {
			IsAuto = false;
			Count = 0.0f;
		}
		else {
			IsAuto = true;
			Count = 3.0f;
		}
		Player->FireBullet(Loc, Rot, BulletLoc);
	}
	
}

/* �÷��̾� �տ� ��ֹ�(��)�� �ִ����� �˻��Ͽ� ���� �÷��� �ϴ����� �˸��� �Լ��Դϴ�.
*/
bool UADS_PlayerUpper::HandUpTracer(AMultiPlayerBase* player)
{
	if (player->GetEDownState() != EPlayerDownState::PRONE) 
	{
		TArray<AActor*> actorstoIgnore;
		FHitResult outHit;
		FVector startTrace = player->BodyMesh->GetSocketLocation("CameraLoc");
		FVector endTrace = startTrace + player->ArmMesh->GetRightVector() * 55.0f;
		bool IsHit = UKismetSystemLibrary::SphereTraceSingle(this, startTrace, endTrace, 3.0f, ETraceTypeQuery::TraceTypeQuery1
			, false, actorstoIgnore, EDrawDebugTrace::None, outHit, true);

		return IsHit;
	}
	return false;
}

/* ������ Trace�� �˻��Ͽ� ô���� ȸ��(X�� Roll) ������ ���ϴ� �Լ��Դϴ�.
* Start�� Trace���� ������ ���ϰ�, Angle�� ������ ��ȯ, Distance�� ����(��)�� �������� �÷��̾���� �Ÿ�
* , Dir�� Trace�� ����(��, ��)�� ��Ÿ���ϴ�.
*/
bool UADS_PlayerUpper::CoverTracer(AMultiPlayerBase* Player, FVector Start, float& Angle, float& Distance, float Dir)
{
	CoverRot = Player->GetActorRotation();

	Angle = 0.0f;
	FVector StartTracer_Wall = Start;
	FVector EndTracer_Wall = StartTracer_Wall + Player->GetMesh()->GetRightVector() * 80.0f;
	TArray<AActor*> ActorsToIgnore_Wall;
	FHitResult OutHit_Wall;

	// ���� Ʈ���̼�
	bool hitis_wall = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer_Wall, EndTracer_Wall, 5.0f
		, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore_Wall, EDrawDebugTrace::None, OutHit_Wall, true);

	if (hitis_wall)
	{ // ���濡 ���� ���� �Ѵٸ�
		FMatrix OutHit_Wall_Matrix = FRotationMatrix::MakeFromX(OutHit_Wall.Normal);
		FVector Wall_Axis_X = OutHit_Wall_Matrix.GetUnitAxis(EAxis::X);
		FVector Wall_Axis_Y = OutHit_Wall_Matrix.GetUnitAxis(EAxis::Y);

		FVector StartTracer_Cover = OutHit_Wall.Location + (OutHit_Wall.Normal * (-5.0f));
		FVector EndTracer_Cover = StartTracer_Cover + (Wall_Axis_Y * (42.0f * Dir));
		TArray<AActor*> ActorsToIgnore_Cover;
		FHitResult OutHit_Cover;

		// �¿� Ʈ���̼�
		bool hitis_cover = UKismetSystemLibrary::SphereTraceSingle(this, EndTracer_Cover, StartTracer_Cover, 5.0f
			, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore_Cover, EDrawDebugTrace::None, OutHit_Cover, true);

		if (hitis_cover)
		{ // ���� ���� ��, �츦 Ȯ���ϱ�
			Distance = OutHit_Cover.Distance;

			FVector StartTracer_Aim = OutHit_Cover.Location + (Wall_Axis_Y * (5.0f * Dir));
			FVector EndTracer_Aim = StartTracer_Aim + (Wall_Axis_X * (-20.0f));
			TArray<AActor*> ActorsToIgnore_Aim;
			FHitResult OutHit_Aim;

			bool hitis_aim = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer_Aim, EndTracer_Aim, 2.0f
				, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore_Aim, EDrawDebugTrace::None, OutHit_Aim, true);

			// �� ������ �ƴϸ� ��ȯ
			if (hitis_aim) {
				Angle = 0.0f;
				return false;
			}
			else 
			{ // ���� ���� ��, �쿡 ������ ���� �ϸ� 
				FVector EndPoint = OutHit_Cover.Location + Player->GetActorForwardVector() * -50.0f;
				FVector StartPoint = Player->GetActorLocation();
				FVector HitDir = EndPoint - StartPoint;
				HitDir.Normalize();

				/* �� ���� ������ ���� ���ϱ�
				* �� ������ ���� == �� ������ ũ�� x cos��Ÿ
				* arccos(cos��Ÿ) == ����
				*/
				float Dot = FVector::DotProduct(Player->GetActorUpVector(), HitDir);
				Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
				
				// �׽�Ʈ
				/*Player->GetActorUpVector();
				FVector Test1 = OutHit_Cover.Location - Player->GetActorLocation();
				Test1.Normalize();

				float Dot2 = FVector::DotProduct(Player->GetActorUpVector(), Test1);
				float Dot3 = FVector::DotProduct(Player->GetActorRightVector(), Test1);
				float test2 = (Player->GetActorUpVector().Y * Test1.Z) - (Player->GetActorUpVector().Z * Test1.Y);
				
				float Angle2 = FMath::RadiansToDegrees(FMath::Atan2(Dot3, Dot2));
				UE_LOG(LogTemp, Warning, TEXT("CoverAngle2: %f"), Angle2);*/
				/*if (Angle < 55.0f) {
					if (Angle > 40.0f) { Angle = 55.0f; }
					if (Angle < 5.0f) { Angle = 10.0f; }
				}
				else {
					Angle = 0.0f;
					return false;
				}*/
				Angle += 20.0f;

				UE_LOG(LogTemp, Warning, TEXT("CoverAngle: %f"), Angle);
				return true;
			}
		}
		else { return false; }
	}
	else { return false; }
}

/* �ɾ��� �� ��, �찡 �ƴ� �� �������� ���������� �ϰ� �ϴ� �Լ��Դϴ�.
* �׽�Ʈ��
*/
bool UADS_PlayerUpper::CoverUpTracer(AMultiPlayerBase* Player, FVector Start)
{
	FVector StartTracer_Wall = Start;
	FVector EndTracer_Wall = StartTracer_Wall + Player->GetMesh()->GetRightVector() * 80.0f;
	TArray<AActor*> ActorsToIgnore_Wall;
	FHitResult OutHit_Wall;


	bool hitis_wall = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer_Wall, EndTracer_Wall, 5.0f
		, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore_Wall, EDrawDebugTrace::None, OutHit_Wall, true);

	if (hitis_wall)
	{
		FVector StartTracer_Cover = OutHit_Wall.Location + (FRotationMatrix::MakeFromZ(OutHit_Wall.Normal).GetUnitAxis(EAxis::X) * (-5.0f));
		FVector EndTracer_Cover = StartTracer_Cover + (FRotationMatrix::MakeFromZ(OutHit_Wall.Normal).GetUnitAxis(EAxis::Y) * 42.0f);
		TArray<AActor*> ActorsToIgnore_Cover;
		FHitResult OutHit_Cover;

		
		bool hitis_cover = UKismetSystemLibrary::SphereTraceSingle(this, EndTracer_Cover, StartTracer_Cover, 5.0f
			, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore_Cover, EDrawDebugTrace::None, OutHit_Cover, true);

		if (hitis_cover)
		{
			FVector StartTracer_Aim = OutHit_Cover.Location + FVector(0.0f, 0.0f, 10.0f);
			FVector EndTracer_Aim = StartTracer_Aim + FRotationMatrix::MakeFromX(OutHit_Wall.Normal).GetUnitAxis(EAxis::X) * (-35.0f);
			TArray<AActor*> ActorsToIgnore_Aim;
			FHitResult OutHit_Aim;

			bool hitis_aim = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer_Aim, EndTracer_Aim, 2.0f
				, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore_Aim, EDrawDebugTrace::None, OutHit_Aim, true);

			
			if (hitis_aim) {
				return false;
			}
			else
			{
				Player->SetRootDistance(OutHit_Cover.Distance);
				UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), OutHit_Cover.Distance);
				return true;
			}
		}
		else { return false; }
	}
	else { return false; }
}

/* ī�޶� FOV ���� �Լ� (ī�޶� ����)
*/
void UADS_PlayerUpper::SetADS()
{
	//UE_LOG(LogTemp, Warning, TEXT("ADS: SetADS %f"), ADSTimeline.GetPlaybackPosition());
	if (PlayerCamera) {
		PlayerCamera->SetFieldOfView(FMath::Lerp(90.0f, 50.0f, ADSTimeline.GetPlaybackPosition()));
	}

}

/* ī�޶� FOV ���� ������ �ߵ� �Լ�
*/
void UADS_PlayerUpper::SetADSFinish()
{
	//UE_LOG(LogTemp, Warning, TEXT("ADS: SetADSFinish"));
}