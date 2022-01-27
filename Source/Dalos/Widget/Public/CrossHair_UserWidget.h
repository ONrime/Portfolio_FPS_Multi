// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrossHair_UserWidget.generated.h"

/** ���ڼ� �����Դϴ�.
 * 
 */
UCLASS()
class DALOS_API UCrossHair_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cross")
	ESlateVisibility corssHairVis = ESlateVisibility::Visible; // �Ϲݻ��¿����� �������� Ư�� ����(�޸���, �Ѿ�� ��)������ �����ϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cross")
	bool IsRed = false; // ���� ����ų ���� ���������� ���մϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cross")
	float CrossHairSpread = 0.0f; // ���ڼ� ���� ����

};
