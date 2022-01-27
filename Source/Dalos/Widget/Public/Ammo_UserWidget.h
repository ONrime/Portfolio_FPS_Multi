// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ammo_UserWidget.generated.h"

/** 플레이어 무기와 탄 수를 보여줍니다.
 * 
 */
UCLASS()
class DALOS_API UAmmo_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	FName WeaponName = ""; // 무기 이름을 표시합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int LoadedAmmo = 0;    // 현재 총의 장전된 탄 수를 표시합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int EquipAmmo = 0;	   // 현재 총기의 전체 탄 수를 표시합니다.
};
