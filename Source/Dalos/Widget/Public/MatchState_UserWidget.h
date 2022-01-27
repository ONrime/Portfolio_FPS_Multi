// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchState_UserWidget.generated.h"

/** ��ġ�� ���¸� �����ݴϴ�.
 * ��ġ�� ���� �ð��� �� ���� �¸� Ƚ���� �����ݴϴ�.
 */
UCLASS()
class DALOS_API UMatchState_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	float Count = 40.0f; // ���� ��ġ �ð�
	UPROPERTY(BlueprintReadOnly)
	int RedTeamWinCount = 0; // ���� �� �¸� Ƚ��
	UPROPERTY(BlueprintReadOnly)
	int BlueTeamWinCount = 0; // ��� �� �¸� Ƚ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESlateVisibility WidgetVis = ESlateVisibility::Hidden; // �� ������ ���̱� ����

	// ������ �ð�(�Ű� ����)�� ���� ��ġ Ÿ�̸Ӹ� �����մϴ�.
	void StartTimer(float Time);
	// ��ġ Ÿ�̸Ӹ� �����մϴ�.
	void StopTimer();

private:
	// Ÿ�̸�
	FTimerHandle MatchTimer;

	// Ÿ�̸Ӱ� ������ ������ �ð��� �������� �˸��ϴ�.
	void MatchCount();
	
};
