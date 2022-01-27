// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TwoVersus_GameState.generated.h"

/** ���� ��ü�� ���õ� �����͸� ó���մϴ�.
 * ��ġ �¸� Ƚ ��, �� ��ü ü��, ��� �÷��̾� ��Ʈ�ѷ�, ��� ����������
 */

DECLARE_DELEGATE(FGameStateCheckDelegate)

UCLASS()
class DALOS_API ATwoVersus_GameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ATwoVersus_GameState();
	// PlayerState�� ���� �� �÷��̾��� ü���� ������ �� ��ü ü���� ���մϴ�.
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	TArray<class APlayerController*> AllPlayerController; // ��� �÷��̾� ��Ʈ�ѷ�
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = ServerSettings, meta = (AllowPrivateAccess = "true"))
	TArray<class AActor*> AllPlayerStart;  // ��� ��������

	FGameStateCheckDelegate ChangeTeamHPCheck;
	int GetRedTeamHP() { return RedTeamHP; } int GetBlueTeamHP() { return BlueTeamHP; }

protected:
	UPROPERTY(Replicated)
	int RedTeamHP = 0;  // ���� �� ��ü ü��
	UPROPERTY(Replicated)
	int BlueTeamHP = 0; // ��� �� ��ü ü��

public:
	int16 RedTeamWin = 0;
	int16 BlueTeamWin = 0;

private:


};
