// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ammo_UserWidget.generated.h"

/** �÷��̾� ����� ź ���� �����ݴϴ�.
 * 
 */
UCLASS()
class DALOS_API UAmmo_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	FName WeaponName = ""; // ���� �̸��� ǥ���մϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int LoadedAmmo = 0;    // ���� ���� ������ ź ���� ǥ���մϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int EquipAmmo = 0;	   // ���� �ѱ��� ��ü ź ���� ǥ���մϴ�.
};
