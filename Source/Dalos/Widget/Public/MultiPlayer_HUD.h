// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiPlayer_HUD.generated.h"

DECLARE_DELEGATE_OneParam(FDele_HUDCheck, bool check);
DECLARE_DELEGATE_OneParam(FDele_HUDLocCheck, FVector check);
DECLARE_DELEGATE_OneParam(FDele_HUDFloatCheck, float check);
DECLARE_DELEGATE_OneParam(FDele_HUDNameCheck, FName check);

/** 플레이어 UI
 * 십자선, 탄약등을 처리합니다.
 */
UCLASS()
class DALOS_API AMultiPlayer_HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AMultiPlayer_HUD();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HUD)
	class UUserWidget* CrossHairWidget = nullptr; // 십자선 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UUserWidget* AmmoWidget = nullptr;      // 탄약 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	bool IsRed = false;							  // 십자선의 색 상태 (적을 가리키면 빨간 색으로) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	float CrossHairSpread = 0.0f;				  // 십자선 퍼짐 강도
	
	// 딜리게이트
	FDele_HUDCheck CrossHairHideCheck;	 // 십자선을 가릴때 (플레이어가 달리면 십자선이 사라집니다.)
	FDele_HUDCheck CrossHairRedCheck;	 // 십사전이 적을 가리킬때 빨간색으로
	FDele_HUDCheck HitRedCheck;			 // 적을 공격후 죽었다면 히트 표시가 빨간색으로 
	FDele_HUDLocCheck PlyaerHitLocCheck; // 적이 플레이어를 공격했을 때 공격했던 위치를 토대로 ui 표시합니다.
	FDele_HUDFloatCheck LoadedAmmoCheck; // 현재 장전된 탄수를 검사합니다.
	FDele_HUDFloatCheck EquipAmmoCheck;	 // 현재 들고있는 총의 전체 탄수를 검사합니다.
	FDele_HUDNameCheck WeaponNameCheck;	 // 현재 들고있는 총의 이름을 검사합니다.

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	float CurrentSpread = 0.0f; // 현재 십자선 퍼짐 정도
	float TargetSpread = 0.0f;  // 목표 십자선 퍼짐 정도
	bool IsFire = false;		// 사격 중일 때
	bool IsBackSpread = false;  // 십자선이 벌여질때를 조절 하는 변수입니다. (한 번씩 퍼짐이 이루어지게 하기 위해 필요합니다.)

	TSubclassOf<class UUserWidget> HitCheckClass;       // 플레이어가 적을 맞추면 작동되는 UI
	TSubclassOf<class UUserWidget> PlayerHitCheckClass; // 적이 플레이어를 맞추면 작동되는 UI

public:
	void SetCrossHairSpread(float Cross, bool Back, bool Fire); // 십자선 벌여짐을 플레이어로 부터 받는 함수입니다.
	float GetTargetSpread() { return TargetSpread; }


};
