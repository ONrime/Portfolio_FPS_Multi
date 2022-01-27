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

// 입력에 따른 상태 변환값을 반환하는 함수
UPlayerUpperStateBase* UADS_PlayerUpper::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // 플레이어 컨트롤러 입니다.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // 컨트롤러의 입력을 관리하는 객체입니다.

	// 각 지정된 키를 가져옵니다.
	TArray <FInputActionKeyMapping> ActionUnArmed = PlayerInput->GetKeysForAction(TEXT("UnArmed")); // 근접 무기(비 무장)키
	TArray <FInputActionKeyMapping> ActionADS = PlayerInput->GetKeysForAction(TEXT("ADS"));         // 정밀 조준 키

	if (PlayerInput->IsPressed(ActionUnArmed[0].Key)) 
	{
		temp = NewObject<UUnArmed_PlayerUpper>(this, UUnArmed_PlayerUpper::StaticClass()); // 비 무장 상태 객체 생성
	}
	else if (PlayerInput->InputKey(ActionADS[0].Key, EInputEvent::IE_Released, 1.0f, true)) 
	{
		temp = NewObject<UArmed_PlayerUpper>(this, UArmed_PlayerUpper::StaticClass());     // 무장 상태 객체 생성
	}

	return temp;
}

// 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
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

// 상태가 시작 할 때 작동하는 함수
void UADS_PlayerUpper::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	UE_LOG(LogTemp, Warning, TEXT("ADS: StateStart"));

	PlayerCamera = Player->FollowCamera; // 플레이어 카메라 가져오기

	/* 정밀 조준을 할 때 카메라가 줌인(FOV를 조정)이 됩니다.
	* 줌인이 되는 과정을 Timeline을 사용하여 Curve에 따른 일정한 속도로 줌인이 되게합니다.
	*/
	if (ADSCurve != nullptr)
	{ // Curve가 존재하면
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, "SetADS"); // Timeline에 따라 Update될 함수를 바인드합니다.
		FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, "SetADSFinish"); // Timeline이 끝날 때 작동될 함수를 바인드합니다.
		ADSTimeline.AddInterpFloat(ADSCurve, TimelineCallback); // Curve와 Timeline 연결
		ADSTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
		ADSTimeline.SetPlayRate(4.0f);
		ADSTimeline.PlayFromStart(); // Timeline을 시작합니다.
	}
	if (Player->IsLocallyControlled()) // 십자선을 가리게 합니다.
	{
		if (Player->GetHUD()->CrossHairHideCheck.IsBound()) Player->GetHUD()->CrossHairHideCheck.Execute(true);
	}
}

// Tick에서 작동하는 함수
void UADS_PlayerUpper::StateUpdate(AMultiPlayerBase* Player)
{
	ADSTimeline.TickTimeline(GetWorld()->DeltaTimeSeconds);

	float ViewSize = 50.0f;
	float LeftDistance = 0.0f;
	float RightDistance = 0.0f;
	Player->IsHandUp = HandUpTracer(Player);
	if (Player->IsHandUp == true) // 장애물에 가려져 손이 올라가 있다면
	{
		//UE_LOG(LogTemp, Warning, TEXT("IsHandUp"));

		// 앉은 상태에서의 위 엄폐 기능 (나중에 완성하자)
		/*if (player->IsCoverUp == false && player->GetDownState()->GetEState() == EPlayerDownState::CROUCH)
		{
			player->IsCoverUp = CoverUpTracer(player, player->ArmMesh->GetSocketLocation("CoverLoc"));
			UE_LOG(LogTemp, Warning, TEXT("IsHandUp"));
		}*/

		if (Player->IsCoverLeft == false) 
		{
			// 왼쪽 엄폐Trace를 확인해서 각도와 거리를 구합니다.
			Player->IsCoverLeft = CoverTracer(Player, Player->BodyMesh->GetSocketLocation("CameraLoc") + Player->GetMesh()->GetRightVector() * -5.0f
				, Player->CoverAngleLeft, LeftDistance, 1.0f);
			//UE_LOG(LogTemp, Warning, TEXT("CoverAngleLeft2: %f"), Player->CoverAngleLeft);
		}

		if (Player->IsCoverRight == false)
		{
			// 오른쪽 엄폐Trace를 확인해서 각도와 거리를 구합니다.
			Player->IsCoverRight = CoverTracer(Player, Player->BodyMesh->GetSocketLocation("CameraLoc") + Player->GetMesh()->GetRightVector() * 5.0f
				, Player->CoverAngleRight, RightDistance, -1.0f);
			//if (player->IsLocallyControlled()) UE_LOG(LogTemp, Warning, TEXT("CoverAngleRight2: %f"), Player->CoverAngleRight);
		}
		if ((Player->IsCoverLeft || Player->IsCoverRight)) // 엄폐 조준을 하고 있다면
		{
			if (Player->CoverAngleRight > 0.0f && Player->CoverAngleLeft > 0.0f)
			{ // 만약 두 방향의 각도가 나온다면 가장 낮은 쪽으로 엄폐조준을 합니다.
				//UE_LOG(LogTemp, Warning, TEXT("LeftDistance: %f"), LeftDistance);
				//UE_LOG(LogTemp, Warning, TEXT("RightDistance: %f"), RightDistance);
				LeftDistance > RightDistance ? Player->CoverAngleRight = 0.0f : Player->CoverAngleLeft = 0.0f;
			}
		}
	}

	/* 카메라 X축(Roll)회전을 계산합니다.
	* 엄폐조준을 하면 카메라 회전을 합니다.
	*/
	if (Player->IsLocallyControlled()) // 플레이어 본인이라면
	{
		FRotator InterpToAngle = (Player->GetActorRotation() - CoverRot).GetNormalized();
		float Angleyaw = FMath::ClampAngle(InterpToAngle.Yaw, -90.0f, 90.0f);
		float NowRoll = 0.0f;
		if (Player->IsHandUp == false)
		{
			NowRoll = 0.0f;
		}

		if ((Player->IsCoverLeft || Player->IsCoverRight)) // 엄폐 조준을 하고 있다면
		{
			// 바라 보는 방향에 따라 회전을 결정합니다. (정면은 20 옆면은 0)
			if (Player->CoverAngleLeft > 0.0f) {
				NowRoll = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, -40.0f), FVector2D(-20.0f, 0.0f), Angleyaw);
			}
			if (Player->CoverAngleRight > 0.0f) {
				NowRoll = FMath::GetMappedRangeValueClamped(FVector2D(40.0f, 0.0f), FVector2D(0.0f, 20.0f), Angleyaw);
			}

			// 카메라 X축(Roll)회전
			FRotator NowFollowCameraRot = Player->FollowCamera->GetRelativeRotation();
			Player->FollowCamera->SetRelativeRotation(FRotator(NowFollowCameraRot.Pitch, NowFollowCameraRot.Yaw
				, FMath::FInterpTo(NowFollowCameraRot.Roll, NowRoll, GetWorld()->DeltaTimeSeconds, 5.0f)));

			// 엄폐에 따른 카메라 Z축 위치 조정
			CoverLocZ = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 50.0f), FVector2D(-30.0f, 0.0f), FMath::Abs(Angleyaw));
		}
		//UE_LOG(LogTemp, Warning, TEXT("CoverAngle: %f"), Player->CoverAngleLeft + Player->CoverAngleRight);
		//UE_LOG(LogTemp, Warning, TEXT("Pitch: %f"), Pitch);
	}
	
}

// 상태가 끝날 때 작동하는 함수
void UADS_PlayerUpper::StateEnd(AMultiPlayerBase* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("ADS: StateEnd"));
	if (Player->IsLocallyControlled()) // 십자선을 보이게 합니다.
	{
		if (Player->GetHUD()->CrossHairHideCheck.IsBound()) Player->GetHUD()->CrossHairHideCheck.Execute(false);
	}

	//Player->SpringArm->SetRelativeLocation(FVector(25.0f, 0.0f, 65.0f));
	//Player->SpringArm->AttachToComponent(player->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	
	// 초기화를 합니다.
	Player->IsCoverUp = false;
	Player->IsCoverLeft = false;
	Player->IsCoverRight = false;
	Player->CoverAngleLeft = 0.0f;
	Player->CoverAngleRight = 0.0f;
}

// 상태의 UClass 반환
UClass* UADS_PlayerUpper::GetState()
{
	return UADS_PlayerUpper::StaticClass();
}

/* 공격 버튼을 눌렀을 때 작동하는 함수입니다.
* 현재는 단발, 3점사, 연사 구분하고 발사하는 기능만 넣어 두었습니다.
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

/* 플레이어 앞에 장애물(벽)이 있는지를 검사하여 손을 올려야 하는지를 알리는 함수입니다.
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

/* 엄폐물을 Trace로 검사하여 척추의 회전(X축 Roll) 각도를 구하는 함수입니다.
* Start는 Trace시작 지점을 정하고, Angle은 각도를 반환, Distance는 엄폐물(벽)의 꼭지점와 플레이어와의 거리
* , Dir는 Trace의 방향(좌, 우)을 나타냅니다.
*/
bool UADS_PlayerUpper::CoverTracer(AMultiPlayerBase* Player, FVector Start, float& Angle, float& Distance, float Dir)
{
	CoverRot = Player->GetActorRotation();

	Angle = 0.0f;
	FVector StartTracer_Wall = Start;
	FVector EndTracer_Wall = StartTracer_Wall + Player->GetMesh()->GetRightVector() * 80.0f;
	TArray<AActor*> ActorsToIgnore_Wall;
	FHitResult OutHit_Wall;

	// 전방 트레이서
	bool hitis_wall = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer_Wall, EndTracer_Wall, 5.0f
		, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore_Wall, EDrawDebugTrace::None, OutHit_Wall, true);

	if (hitis_wall)
	{ // 전방에 벽이 존재 한다면
		FMatrix OutHit_Wall_Matrix = FRotationMatrix::MakeFromX(OutHit_Wall.Normal);
		FVector Wall_Axis_X = OutHit_Wall_Matrix.GetUnitAxis(EAxis::X);
		FVector Wall_Axis_Y = OutHit_Wall_Matrix.GetUnitAxis(EAxis::Y);

		FVector StartTracer_Cover = OutHit_Wall.Location + (OutHit_Wall.Normal * (-5.0f));
		FVector EndTracer_Cover = StartTracer_Cover + (Wall_Axis_Y * (42.0f * Dir));
		TArray<AActor*> ActorsToIgnore_Cover;
		FHitResult OutHit_Cover;

		// 좌우 트레이서
		bool hitis_cover = UKismetSystemLibrary::SphereTraceSingle(this, EndTracer_Cover, StartTracer_Cover, 5.0f
			, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore_Cover, EDrawDebugTrace::None, OutHit_Cover, true);

		if (hitis_cover)
		{ // 전방 벽의 좌, 우를 확인하기
			Distance = OutHit_Cover.Distance;

			FVector StartTracer_Aim = OutHit_Cover.Location + (Wall_Axis_Y * (5.0f * Dir));
			FVector EndTracer_Aim = StartTracer_Aim + (Wall_Axis_X * (-20.0f));
			TArray<AActor*> ActorsToIgnore_Aim;
			FHitResult OutHit_Aim;

			bool hitis_aim = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer_Aim, EndTracer_Aim, 2.0f
				, ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore_Aim, EDrawDebugTrace::None, OutHit_Aim, true);

			// 빈 공간이 아니면 반환
			if (hitis_aim) {
				Angle = 0.0f;
				return false;
			}
			else 
			{ // 전방 벽의 좌, 우에 공간이 존재 하면 
				FVector EndPoint = OutHit_Cover.Location + Player->GetActorForwardVector() * -50.0f;
				FVector StartPoint = Player->GetActorLocation();
				FVector HitDir = EndPoint - StartPoint;
				HitDir.Normalize();

				/* 두 벡터 사이의 각도 구하기
				* 두 벡터의 내적 == 두 벡터의 크기 x cos세타
				* arccos(cos세타) == 각도
				*/
				float Dot = FVector::DotProduct(Player->GetActorUpVector(), HitDir);
				Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
				
				// 테스트
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

/* 앉았을 때 좌, 우가 아닌 위 방향으로 엄폐조준을 하게 하는 함수입니다.
* 테스트중
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

/* 카메라 FOV 설정 함수 (카메라 줌인)
*/
void UADS_PlayerUpper::SetADS()
{
	//UE_LOG(LogTemp, Warning, TEXT("ADS: SetADS %f"), ADSTimeline.GetPlaybackPosition());
	if (PlayerCamera) {
		PlayerCamera->SetFieldOfView(FMath::Lerp(90.0f, 50.0f, ADSTimeline.GetPlaybackPosition()));
	}

}

/* 카메라 FOV 설정 끝날때 발동 함수
*/
void UADS_PlayerUpper::SetADSFinish()
{
	//UE_LOG(LogTemp, Warning, TEXT("ADS: SetADSFinish"));
}