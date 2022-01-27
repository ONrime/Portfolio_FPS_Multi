// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Components/TimelineComponent.h"
#include "TwoVersus_PlayerState.generated.h"

/** �÷��̾ �ٸ� Ŭ���̾�Ʈ �� ���� ���� �����ؾ��ϴ� �������� ó���մϴ�.
 * ü��, ź ��, �� �̸� ���� �ֽ��ϴ�.
 */

DECLARE_DELEGATE(FPlayerStateCheck);

UCLASS()
class DALOS_API ATwoVersus_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ATwoVersus_PlayerState();

	void DamageToHP(float damage);
	float GetPlyaerHP() { return PlayerHP; }
	int GetPlayerXP() { return PlayerXP; }

	// �ڵ� ȸ�� ���� (����� ���� �ʽ��ϴ�. ����� �ϼ� ������ �뷱���� �� �´� �Ǵ��ؼ� �� �����Դϴ�.)
	void StartHeal();
	void StopHeal();

	UPROPERTY(Transient)
	FName WeaponName = ""; // ���� �̸�
	UPROPERTY()
	int RifleAmmo = 0;     // �ڵ� ���� ź ��
	UPROPERTY()
	int PistolAmmo = 0;    // ���� ź ��
	UPROPERTY(Transient)
	int LoadedAmmo = 0;    // ������ ź ��
	UPROPERTY(Transient)
	int EquipAmmo = 0;	   // ���� ����ִ� ���� ź ��
	UPROPERTY(Replicated)
	FString TeamName = ""; 

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

protected:
	
	// , ReplicatedUsing = OnRep_UpdatePlayerHP
	UPROPERTY(Transient, Replicated) // ����ȭ���� ��ȸ��Ű�� Ű�����̴�. �����Ͱ� ��� ���ϱ� ������ �����ϴ°� �ǹ̰� ����.
	float PlayerHP = 100; // �÷��̾� ü��
	UPROPERTY(Transient)
	int PlayerXP = 0; // ��ġ���� ���� ����
	UPROPERTY()
	class AMultiPlayerBase* Player = nullptr;

	// �ڵ� ȸ��
	UPROPERTY(EditAnywhere)
	UCurveFloat* HealCurve; // TimeLine�� ���󰡴� Ŀ��
	FTimeline HealTimeline; // �ڵ� ȸ�� Ÿ�Ӷ���
	UFUNCTION()
	void SetHeal();		    // ü�� ȸ�� ���� �� �� �۵��ϴ� �Լ��Դϴ�.
	UFUNCTION()
	void SetHealFinish();   // ü�� ȸ�� ���� �� �۵��ϴ� �Լ��Դϴ�.

	// ü���� ������Ʈ �Ǹ� �÷��̾�(��Ʈ�ѷ��� ���� == ��)�� ������Ʈ�ϴ� �Լ��Դϴ�.
	UFUNCTION()
	void OnRep_UpdatePlayerHP();

private:

public:

};
