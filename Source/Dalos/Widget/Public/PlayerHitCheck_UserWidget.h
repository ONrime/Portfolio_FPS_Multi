// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHitCheck_UserWidget.generated.h"

/** �÷��̾ ������ ������ ���ϸ� ������ �� ��ġ�� ��������� �����ִ� �����Դϴ�.
 * 
 */
UCLASS()
class DALOS_API UPlayerHitCheck_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// ������ ���� ��ġ�� ������Ʈ�մϴ�.
	UFUNCTION(BlueprintNativeEvent)
	void HitLocCheck(FVector loc);
	virtual void HitLocCheck_Implementation(FVector loc);

};
