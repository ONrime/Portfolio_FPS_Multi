// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Widget/Public/MultiPlayer_HUD.h"
#include "Dalos/Widget/Public/CrossHair_UserWidget.h"
#include "Dalos/Widget/Public/HitCheck_UserWidget.h"
#include "Dalos/Widget/Public/PlayerHitCheck_UserWidget.h"
#include "Dalos/Widget/Public/Ammo_UserWidget.h"
#include "Blueprint/UserWidget.h"

AMultiPlayer_HUD::AMultiPlayer_HUD() 
{
	static ConstructorHelpers::FClassFinder<UUserWidget>CROSSHAIR_WIDGET(TEXT("WidgetBlueprint'/Game/UI/Player/CrossHair.CrossHair_C'"));
	if (CROSSHAIR_WIDGET.Succeeded()) CrossHairWidget = CreateWidget<UUserWidget>(GetWorld(), CROSSHAIR_WIDGET.Class);
	static ConstructorHelpers::FClassFinder<UUserWidget>HITCHECK_WIDGET(TEXT("WidgetBlueprint'/Game/UI/Player/HitCheck.HitCheck_C'"));
	if (HITCHECK_WIDGET.Succeeded()) HitCheckClass= HITCHECK_WIDGET.Class;
	static ConstructorHelpers::FClassFinder<UUserWidget>PLAYERHITCHECK_WIDGET(TEXT("WidgetBlueprint'/Game/UI/Player/PlayerHitCheck.PlayerHitCheck_C'"));
	if (PLAYERHITCHECK_WIDGET.Succeeded()) PlayerHitCheckClass = PLAYERHITCHECK_WIDGET.Class;
	static ConstructorHelpers::FClassFinder<UUserWidget>AMMO_WIDGET(TEXT("WidgetBlueprint'/Game/UI/Player/Ammo.Ammo_C'"));
	if (AMMO_WIDGET.Succeeded()) AmmoWidget = CreateWidget<UUserWidget>(GetWorld(), AMMO_WIDGET.Class);

}

void AMultiPlayer_HUD::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("AMultiPlayer_HUD::BeginPlay"));

	if (CrossHairWidget != nullptr) CrossHairWidget->AddToViewport();
	if (AmmoWidget != nullptr) AmmoWidget->AddToViewport();

}
void AMultiPlayer_HUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// ���ڼ��� ������ (�÷��̾ �޸��� ���ڼ��� ������ϴ�.)
	CrossHairHideCheck.BindLambda([this](bool check)->void 
	{
		UCrossHair_UserWidget* cross = Cast<UCrossHair_UserWidget>(CrossHairWidget);
		if (check)
		{
			cross->corssHairVis = ESlateVisibility::Hidden;
		}else { cross->corssHairVis = ESlateVisibility::Visible; }
	});

	// �ʻ����� ���� ����ų�� ����������
	CrossHairRedCheck.BindLambda([this](bool check)->void
	{
		UCrossHair_UserWidget* cross = Cast<UCrossHair_UserWidget>(CrossHairWidget);
		IsRed = check;
		cross->IsRed = check;
		//UE_LOG(LogTemp, Warning, TEXT("CrossHairRedCheck"));
	});

	// ���� ������ �׾��ٸ� ��Ʈ ǥ�ð� ���������� 
	HitRedCheck.BindLambda([this](bool check)->void 
	{
		auto HitCheckWidget = CreateWidget<UUserWidget>(GetWorld(), HitCheckClass);
		if (HitCheckWidget != nullptr) {
			HitCheckWidget->AddToViewport();
			UHitCheck_UserWidget* hit = Cast<UHitCheck_UserWidget>(HitCheckWidget);
			hit->RedCheck(check);
		}
	});

	// ���� �÷��̾ �������� �� �����ߴ� ��ġ�� ���� ui ǥ���մϴ�.
	PlyaerHitLocCheck.BindLambda([this](FVector loc)->void
	{
		auto PlayerHitCheckWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHitCheckClass);
		if (PlayerHitCheckWidget != nullptr) {
			PlayerHitCheckWidget->AddToViewport();
			UPlayerHitCheck_UserWidget* playerhit = Cast<UPlayerHitCheck_UserWidget>(PlayerHitCheckWidget);
			playerhit->HitLocCheck(loc);
		}
	});

	// ���� ������ ź���� �˻��մϴ�.
	LoadedAmmoCheck.BindLambda([this](float check)->void 
	{
		UAmmo_UserWidget* ammo = Cast<UAmmo_UserWidget>(AmmoWidget);
		if (ammo) ammo->LoadedAmmo = check;
	});

	// ���� ����ִ� ���� ��ü ź���� �˻��մϴ�.
	EquipAmmoCheck.BindLambda([this](float check)->void
	{
		UAmmo_UserWidget* ammo = Cast<UAmmo_UserWidget>(AmmoWidget);
		if (ammo) ammo->EquipAmmo = check;
	});

	// ���� ����ִ� ���� �̸��� �˻��մϴ�.
	WeaponNameCheck.BindLambda([this](FName check)->void
	{
		UAmmo_UserWidget* ammo = Cast<UAmmo_UserWidget>(AmmoWidget);
		if (ammo) ammo->WeaponName = check;
	});
}
void AMultiPlayer_HUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float Speed = 30.0f;
	float Target = TargetSpread;
	if (IsFire && CurrentSpread >= TargetSpread - 5.0f) { // �� �� �۵�
		IsBackSpread = true;
	}
	if (IsBackSpread) {
		Speed = 500.0f;
		Target = TargetSpread - 80.0f;
	}
	CurrentSpread = FMath::FInterpTo(CurrentSpread, Target, DeltaTime, Speed);
	
	//UE_LOG(LogTemp, Warning, TEXT("CurrentSpread: %f"), CurrentSpread);
	//UE_LOG(LogTemp, Warning, TEXT("TargetSpread: %f"), TargetSpread);
	UCrossHair_UserWidget* crossHair = Cast<UCrossHair_UserWidget>(CrossHairWidget);
	crossHair->CrossHairSpread = CurrentSpread;

	UAmmo_UserWidget* ammo = Cast<UAmmo_UserWidget>(AmmoWidget);
	//ammo->LoadedAmmo=

}
void AMultiPlayer_HUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	//if (CrossHairHideCheck.IsBound()) CrossHairHideCheck.Unbi
}

void AMultiPlayer_HUD::SetCrossHairSpread(float Cross, bool Back, bool Fire)
{
	IsFire = Fire;
	IsBackSpread = Back;
	TargetSpread = Cross;
}
