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

// �Է¿� ���� ���� ��ȯ���� ��ȯ�ϴ� �Լ�
UPlayerUpperStateBase* UArmed_PlayerUpper::HandleInput(AMultiPlayerBase* Player)
{
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()); // �÷��̾� ��Ʈ�ѷ� �Դϴ�.
	UPlayerInput* PlayerInput = Cast<UPlayerInput>(PlayerController->PlayerInput); // ��Ʈ�ѷ��� �Է��� �����ϴ� ��ü�Դϴ�.

	// �� ������ Ű�� �����ɴϴ�.
	TArray <FInputActionKeyMapping> ActionUnArmed = PlayerInput->GetKeysForAction(TEXT("UnArmed")); // ���� ����(�� ����)Ű
	TArray <FInputActionKeyMapping> ActionADS = PlayerInput->GetKeysForAction(TEXT("ADS"));			// ���� ���� Ű

	if (PlayerInput->IsPressed(ActionUnArmed[0].Key)) 
	{
		temp = NewObject<UUnArmed_PlayerUpper>(this, UUnArmed_PlayerUpper::StaticClass()); // �� ���� ���� ��ü ����
	}
	else if (PlayerInput->IsPressed(ActionADS[0].Key)) 
	{
		temp = NewObject<UADS_PlayerUpper>(this, UADS_PlayerUpper::StaticClass());         // ���� ���� ���� ��ü ����
	}

	return temp;
}

// ���� �Ȱ����� ���� �� �ٸ� Ŭ���̾�Ʈ���� �����ϱ� ���� �Լ�
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

// ���°� ���� �� �� �۵��ϴ� �Լ�
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

// Tick���� �۵��ϴ� �Լ�
void UArmed_PlayerUpper::StateUpdate(AMultiPlayerBase* Player)
{
	ADSTimeline.TickTimeline(GetWorld()->DeltaTimeSeconds);

	float EmptyAngle = 0.0f;
	if (Player->GetEDownState() != EPlayerDownState::PRONE)
	{
		Player->IsHandUp = HandUpTracer(Player);
	}
	
}

// ���°� ���� �� �۵��ϴ� �Լ�
void UArmed_PlayerUpper::StateEnd(AMultiPlayerBase* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Armed: StateEnd"));
}

// ������ UClass ��ȯ
UClass* UArmed_PlayerUpper::GetState()
{
	return UArmed_PlayerUpper::StaticClass();
}

// ��� (����� �ܹ�, ����, 3���� ������ �����ϰ��ֽ��ϴ�.)
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

// ��ֹ��� ������ ���� �÷��� �ϴ��� �˷��ִ� �Լ�
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

// �� �ƿ� ���� �� �� �۵��Ǵ� �Լ�
void UArmed_PlayerUpper::SetUnADS()
{
	if (PlayerCamera) {
		PlayerCamera->SetFieldOfView(FMath::Lerp(50.0f, 90.0f, ADSTimeline.GetPlaybackPosition()));
	}
}

// �� �ƿ��� ������ �۵��Ǵ� �Լ�
void UArmed_PlayerUpper::SetUnADSFinish()
{

}


