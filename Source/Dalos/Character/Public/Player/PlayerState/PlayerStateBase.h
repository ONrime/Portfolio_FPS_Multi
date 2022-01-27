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
	virtual void StateStart(class AMultiPlayerBase* Player);  // 각 상태가 시작할 때 작동되는 함수이다.
	virtual void StateUpdate(class AMultiPlayerBase* Player); // 각 상태마다 Tick에서 작동되는 함수이다.
	virtual void StateEnd(class AMultiPlayerBase* Player);    // 각 상태가 마무리를 해야 할때 작동되는 함수이다.
	virtual void StateChangeStart(class AMultiPlayerBase* Player, float Speed);    // 각 상태가 바뀌기 시작할 때 작동되는 함수이다.(Tick에서 작동)
	virtual void StateChangeUpdate(class AMultiPlayerBase* Player);    // 각 상태가 바뀔때 작동되는 함수이다.(Tick에서 작동)
	virtual UClass* GetState();                               // 상태의 StaticClass를 반환한다.
	
	bool GetIsChangeState() { return IsChangeState; }
	void SetIsChangeState(bool Set) { IsChangeState = Set; }
	bool GetIsStart() { return IsStart; }
	float GetChangeSpeed() { return ChangeSpeed; }

protected:
	bool IsChangeState = false;
	bool IsStart = false;			// 시작 했는지를 알려줍니다. (Change를 할 때 두번 발동되는 것을 방지하기위해 넣었습니다.)
	float ChangeSpeed = 0.0f;

};
