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

	// 십자선을 가릴때 (플레이어가 달리면 십자선이 사라집니다.)
	CrossHairHideCheck.BindLambda([this](bool check)->void 
	{
		UCrossHair_UserWidget* cross = Cast<UCrossHair_UserWidget>(CrossHairWidget);
		if (check)
		{
			cross->corssHairVis = ESlateVisibility::Hidden;
		}else { cross->corssHairVis = ESlateVisibility::Visible; }
	});

	// 십사전이 적을 가리킬때 빨간색으로
	CrossHairRedCheck.BindLambda([this](bool check)->void
	{
		UCrossHair_UserWidget* cross = Cast<UCrossHair_UserWidget>(CrossHairWidget);
		IsRed = check;
		cross->IsRed = check;
		//UE_LOG(LogTemp, Warning, TEXT("CrossHairRedCheck"));
	});

	// 적을 공격후 죽었다면 히트 표시가 빨간색으로 
	HitRedCheck.BindLambda([this](bool check)->void 
	{
		auto HitCheckWidget = CreateWidget<UUserWidget>(GetWorld(), HitCheckClass);
		if (HitCheckWidget != nullptr) {
			HitCheckWidget->AddToViewport();
			UHitCheck_UserWidget* hit = Cast<UHitCheck_UserWidget>(HitCheckWidget);
			hit->RedCheck(check);
		}
	});

	// 적이 플레이어를 공격했을 때 공격했던 위치를 토대로 ui 표시합니다.
	PlyaerHitLocCheck.BindLambda([this](FVector loc)->void
	{
		auto PlayerHitCheckWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHitCheckClass);
		if (PlayerHitCheckWidget != nullptr) {
			PlayerHitCheckWidget->AddToViewport();
			UPlayerHitCheck_UserWidget* playerhit = Cast<UPlayerHitCheck_UserWidget>(PlayerHitCheckWidget);
			playerhit->HitLocCheck(loc);
		}
	});

	// 현재 장전된 탄수를 검사합니다.
	LoadedAmmoCheck.BindLambda([this](float check)->void 
	{
		UAmmo_UserWidget* ammo = Cast<UAmmo_UserWidget>(AmmoWidget);
		if (ammo) ammo->LoadedAmmo = check;
	});

	// 현재 들고있는 총의 전체 탄수를 검사합니다.
	EquipAmmoCheck.BindLambda([this](float check)->void
	{
		UAmmo_UserWidget* ammo = Cast<UAmmo_UserWidget>(AmmoWidget);
		if (ammo) ammo->EquipAmmo = check;
	});

	// 현재 들고있는 총의 이름을 검사합니다.
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
	if (IsFire && CurrentSpread >= TargetSpread - 5.0f) { // 한 번 작동
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
