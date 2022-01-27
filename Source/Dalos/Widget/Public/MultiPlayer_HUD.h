// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiPlayer_HUD.generated.h"

DECLARE_DELEGATE_OneParam(FDele_HUDCheck, bool check);
DECLARE_DELEGATE_OneParam(FDele_HUDLocCheck, FVector check);
DECLARE_DELEGATE_OneParam(FDele_HUDFloatCheck, float check);
DECLARE_DELEGATE_OneParam(FDele_HUDNameCheck, FName check);

/** �÷��̾� UI
 * ���ڼ�, ź����� ó���մϴ�.
 */
UCLASS()
class DALOS_API AMultiPlayer_HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AMultiPlayer_HUD();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HUD)
	class UUserWidget* CrossHairWidget = nullptr; // ���ڼ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UUserWidget* AmmoWidget = nullptr;      // ź�� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	bool IsRed = false;							  // ���ڼ��� �� ���� (���� ����Ű�� ���� ������) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	float CrossHairSpread = 0.0f;				  // ���ڼ� ���� ����
	
	// ��������Ʈ
	FDele_HUDCheck CrossHairHideCheck;	 // ���ڼ��� ������ (�÷��̾ �޸��� ���ڼ��� ������ϴ�.)
	FDele_HUDCheck CrossHairRedCheck;	 // �ʻ����� ���� ����ų�� ����������
	FDele_HUDCheck HitRedCheck;			 // ���� ������ �׾��ٸ� ��Ʈ ǥ�ð� ���������� 
	FDele_HUDLocCheck PlyaerHitLocCheck; // ���� �÷��̾ �������� �� �����ߴ� ��ġ�� ���� ui ǥ���մϴ�.
	FDele_HUDFloatCheck LoadedAmmoCheck; // ���� ������ ź���� �˻��մϴ�.
	FDele_HUDFloatCheck EquipAmmoCheck;	 // ���� ����ִ� ���� ��ü ź���� �˻��մϴ�.
	FDele_HUDNameCheck WeaponNameCheck;	 // ���� ����ִ� ���� �̸��� �˻��մϴ�.

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	float CurrentSpread = 0.0f; // ���� ���ڼ� ���� ����
	float TargetSpread = 0.0f;  // ��ǥ ���ڼ� ���� ����
	bool IsFire = false;		// ��� ���� ��
	bool IsBackSpread = false;  // ���ڼ��� ���������� ���� �ϴ� �����Դϴ�. (�� ���� ������ �̷������ �ϱ� ���� �ʿ��մϴ�.)

	TSubclassOf<class UUserWidget> HitCheckClass;       // �÷��̾ ���� ���߸� �۵��Ǵ� UI
	TSubclassOf<class UUserWidget> PlayerHitCheckClass; // ���� �÷��̾ ���߸� �۵��Ǵ� UI

public:
	void SetCrossHairSpread(float Cross, bool Back, bool Fire); // ���ڼ� �������� �÷��̾�� ���� �޴� �Լ��Դϴ�.
	float GetTargetSpread() { return TargetSpread; }


};
