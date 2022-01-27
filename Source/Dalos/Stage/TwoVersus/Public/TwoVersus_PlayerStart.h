// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "TwoVersus_PlayerStart.generated.h"

/** �÷��̾� ��ŸƮ ������ ��Ÿ���� ������Ʈ�Դϴ�.
 * �÷��̾ �����ǰ� ������ �� �̸��� �ش��ϴ� ��ŸƮ �������� �Ű����ϴ�.
 */
UCLASS()
class DALOS_API ATwoVersus_PlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	FString TeamName = "Red"; // ��ŸƮ ������ �ش�Ǵ� ��

	bool IsUse = false; // ���Ǿ������� ��Ÿ���ϴ�. (1ȸ��)

};
