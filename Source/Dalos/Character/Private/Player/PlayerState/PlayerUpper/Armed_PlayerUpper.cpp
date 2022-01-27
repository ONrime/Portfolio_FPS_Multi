// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/Armed_PlayerUpper.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/UnArmed_PlayerUpper.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/ADS_PlayerUpper.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Dalos/Weapone/Public/WeaponeBase.h"

UArmed_PlayerUpper::UArmed_PlayerUpper()
{
	temp = nullptr;
	UpperState = EPlayerUpperState::ARMED;

	static ConstructorHelpers::FObjectFinder<UCurveFloat>ADSCURVE(TEXT("CurveFloat'/Game/Curve/ADSChange.ADSChange'"));
	if (ADSCURVE.Succeeded()) {
		ADSCurve = ADSCURVE.Object;
	}
}

// 입력에 따른 상태 변환값을 반환하는 함수
UPlayerUpperStateBase* UArmed_PlayerUpper::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // 플레이어 컨트롤러 입니다.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // 컨트롤러의 입력을 관리하는 객체입니다.

	// 각 지정된 키를 가져옵니다.
	TArray <FInputActionKeyMapping> ActionUnArmed = PlayerInput->GetKeysForAction(TEXT("UnArmed")); // 근접 무기(비 무장)키
	TArray <FInputActionKeyMapping> ActionADS = PlayerInput->GetKeysForAction(TEXT("ADS"));			// 정밀 조준 키

	if (PlayerInput->IsPressed(ActionUnArmed[0].Key)) 
	{
		temp = NewObject<UUnArmed_PlayerUpper>(this, UUnArmed_PlayerUpper::StaticClass()); // 비 무장 상태 객체 생성
	}
	else if (PlayerInput->IsPressed(ActionADS[0].Key)) 
	{
		temp = NewObject<UADS_PlayerUpper>(this, UADS_PlayerUpper::StaticClass());         // 정밀 조준 상태 객체 생성
	}

	return temp;
}

// 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
UPlayerUpperStateBase* UArmed_PlayerUpper::SendHandleInput(EPlayerPress Press)
{
	if (Press == EPlayerPress::UNARMED)
	{
		temp = NewObject<UUnArmed_PlayerUpper>(this, UUnArmed_PlayerUpper::StaticClass());
	}
	else if (Press == EPlayerPress::ADS)
	{
		temp = NewObject<UADS_PlayerUpper>(this, UADS_PlayerUpper::StaticClass());
	}
	return temp;
}

// 상태가 시작 할 때 작동하는 함수
void UArmed_PlayerUpper::StateStart(AMultiPlayerBase* Player)
{
	Super::StateStart(Player);

	UE_LOG(LogTemp, Warning, TEXT("Armed: StateStart"));

	PlayerCamera = Player->FollowCamera;

	if (ADSCurve != nullptr && Player->UpperStateBClass == UADS_PlayerUpper::StaticClass()) {
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, "SetUnADS");
		FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, "SetUnADSFinish");
		ADSTimeline.AddInterpFloat(ADSCurve, TimelineCallback);
		ADSTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
		ADSTimeline.SetPlayRate(4.0f);
		ADSTimeline.PlayFromStart();
	}
}

// Tick에서 작동하는 함수
void UArmed_PlayerUpper::StateUpdate(AMultiPlayerBase* Player)
{
	ADSTimeline.TickTimeline(GetWorld()->DeltaTimeSeconds);

	float EmptyAngle = 0.0f;
	if (Player->GetEDownState() != EPlayerDownState::PRONE)
	{
		Player->IsHandUp = HandUpTracer(Player);
	}
	
}

// 상태가 끝날 때 작동하는 함수
void UArmed_PlayerUpper::StateEnd(AMultiPlayerBase* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Armed: StateEnd"));
}

// 상태의 UClass 반환
UClass* UArmed_PlayerUpper::GetState()
{
	return UArmed_PlayerUpper::StaticClass();
}

// 사격 (현재는 단발, 연사, 3점사 설정만 관리하고있습니다.)
void UArmed_PlayerUpper::PlayerFire(AMultiPlayerBase* Player, AWeaponeBase* Equip, bool& IsAuto, float& Count, FVector Loc, FRotator Rot, FVector BulletLoc)
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

// 장애물을 만나면 손을 올려야 하는지 알려주는 함수
bool UArmed_PlayerUpper::HandUpTracer(AMultiPlayerBase* Player)
{
	TArray<AActor*> actorstoIgnore;
	FHitResult outHit;
	FVector startTrace = Player->BodyMesh->GetSocketLocation("CameraLoc");
	FVector endTrace = startTrace + Player->ArmMesh->GetRightVector() * 55.0f;
	bool IsHit = UKismetSystemLibrary::SphereTraceSingle(this, startTrace, endTrace, 8.0f, ETraceTypeQuery::TraceTypeQuery1
		, false, actorstoIgnore, EDrawDebugTrace::None, outHit, true);

	return IsHit;
	
}

// 줌 아웃 시작 할 때 작동되는 함수
void UArmed_PlayerUpper::SetUnADS()
{
	if (PlayerCamera) {
		PlayerCamera->SetFieldOfView(FMath::Lerp(50.0f, 90.0f, ADSTimeline.GetPlaybackPosition()));
	}
}

// 줌 아웃이 끝나고 작동되는 함수
void UArmed_PlayerUpper::SetUnADSFinish()
{

}


