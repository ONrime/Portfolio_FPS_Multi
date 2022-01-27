// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStateBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DALOS_API UPlayerStateBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerStateBase();
	virtual void StateStart(class AMultiPlayerBase* Player);  // �� ���°� ������ �� �۵��Ǵ� �Լ��̴�.
	virtual void StateUpdate(class AMultiPlayerBase* Player); // �� ���¸��� Tick���� �۵��Ǵ� �Լ��̴�.
	virtual void StateEnd(class AMultiPlayerBase* Player);    // �� ���°� �������� �ؾ� �Ҷ� �۵��Ǵ� �Լ��̴�.
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed);    // �� ���°� �ٲ�� ������ �� �۵��Ǵ� �Լ��̴�.(Tick���� �۵�)
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player);    // �� ���°� �ٲ� �۵��Ǵ� �Լ��̴�.(Tick���� �۵�)
	virtual UClass* GetState();                               // ������ StaticClass�� ��ȯ�Ѵ�.
	
	bool GetIsChangeState() { return IsChangeState; }
	void SetIsChangeState(bool Set) { IsChangeState = Set; }
	bool GetIsStart() { return IsStart; }
	float GetChangeSpeed() { return ChangeSpeed; }

protected:
	bool IsChangeState = false;
	bool IsStart = false;			// ���� �ߴ����� �˷��ݴϴ�. (Change�� �� �� �ι� �ߵ��Ǵ� ���� �����ϱ����� �־����ϴ�.)
	float ChangeSpeed = 0.0f;

};
