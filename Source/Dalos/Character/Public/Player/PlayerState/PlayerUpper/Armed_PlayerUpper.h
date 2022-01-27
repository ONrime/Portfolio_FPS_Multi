// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/PlayerUpperStateBase.h"
#include "Components/TimelineComponent.h"
#include "Armed_PlayerUpper.generated.h"

/**무장 상태
 * 
 */
UCLASS()
class DALOS_API UArmed_PlayerUpper : public UPlayerUpperStateBase
{
	GENERATED_BODY()
	
public:
	UArmed_PlayerUpper();

	virtual UPlayerUpperStateBase* HandleInput(class AMultiPlayerBase* Player) override; // 입력에 따른 상태 변환값을 반환하는 함수
	virtual UPlayerUpperStateBase* SendHandleInput(EPlayerPress Press) override;         // 위와 똑같지만 서버 및 다른 클라이언트에게 전달하기 위한 함수
	virtual void StateStart(class AMultiPlayerBase* Player) override;                    // 상태가 시작 할 때 작동하는 함수
	virtual void StateUpdate(class AMultiPlayerBase* Player) override;                   // Tick에서 작동하는 함수
	virtual void StateEnd(class AMultiPlayerBase* Player) override;                      // 상태가 끝날 때 작동하는 함수
	virtual UClass* GetState() override;												 // 상태의 UClass 반환

	// 사격 (현재는 단발, 연사, 3점사 설정만 관리하고있습니다.)
	virtual void PlayerFire(class AMultiPlayerBase* Player, class AWeaponeBase* Equip, bool& IsAuto, float& Count, FVector Loc, FRotator Rot, FVector BulletLoc) override;
	virtual bool HandUpTracer(class AMultiPlayerBase* Player) override;  // 장애물을 만나면 손을 올려야 하는지 알려주는 함수

protected:

private:
	UPROPERTY(EditAnywhere)
	UCurveFloat* ADSCurve; // 정밀 조준에서 조준으로 돌아갈 때 카메라 줌인이 줌 아웃이 됩니다. 줌 아웃을 하기위해 보간을 하는데 그 때 커브를 사용합니다.
	FTimeline ADSTimeline; // 시간 내로 줌 아웃을 합니다.
	UFUNCTION()
	void SetUnADS();       // 줌 아웃 시작 할 때 작동되는 함수
	UFUNCTION()
	void SetUnADSFinish(); // 줌 아웃이 끝나고 작동되는 함수

	class UCameraComponent* PlayerCamera;

};
