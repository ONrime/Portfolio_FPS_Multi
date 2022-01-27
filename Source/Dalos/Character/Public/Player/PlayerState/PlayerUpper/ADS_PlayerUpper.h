// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/PlayerUpperStateBase.h"
#include "Components/TimelineComponent.h"
#include "ADS_PlayerUpper.generated.h"

/**
 * 
 */
UCLASS()
class DALOS_API UADS_PlayerUpper : public UPlayerUpperStateBase
{
	GENERATED_BODY()
	
public:
	UADS_PlayerUpper();

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
	/*엄폐 조준을 하기위한 각도, 거리(플레이어와 벽의 모서리간의 거리)를 반환하는 함수입니다.
	* Trace가 시작되는 지점(Start)과 방향(Dir)을 알려주면 그 방향의 Trace를 만들어서 각도와 거리를 구합니다.
	*/
	bool CoverTracer(class AMultiPlayerBase* Player, FVector Start, float& Angle, float& Distance, float Dir);
	// 테스트용 (앉았을 때 위로 엄폐조준을 하는 기능, 완성 X)
	bool CoverUpTracer(class AMultiPlayerBase* Player, FVector Start);

private:
	UPROPERTY(EditAnywhere)
	UCurveFloat* ADSCurve; // 카메라 줌인 보간을 할 때 사용하는 커브입니다. 
	FTimeline ADSTimeline; // 시간 내로 줌 인을 합니다.
	UFUNCTION()
	void SetADS();		   // 중인을 시작 할때 실행되는 함수입니다.
	UFUNCTION()
	void SetADSFinish();   // 중인을 끝날 때 실행되는 함수입니다.

	class UCameraComponent* PlayerCamera = nullptr;
	FRotator CoverRot = FRotator::ZeroRotator; // 엄폐를 시작 할 때의 액터 Rotation
};
