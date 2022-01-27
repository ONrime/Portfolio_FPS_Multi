// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Components/TimelineComponent.h"
#include "TwoVersus_PlayerState.generated.h"

/** 플레이어가 다른 클라이언트 및 게임 모드와 공유해야하는 정보들을 처리합니다.
 * 체력, 탄 수, 팀 이름 등이 있습니다.
 */

DECLARE_DELEGATE(FPlayerStateCheck);

UCLASS()
class DALOS_API ATwoVersus_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ATwoVersus_PlayerState();

	void DamageToHP(float damage);
	float GetPlyaerHP() { return PlayerHP; }
	int GetPlayerXP() { return PlayerXP; }

	// 자동 회복 관련 (현재는 쓰지 않습니다. 기능은 완성 하지만 밸런스에 안 맞다 판단해서 뺀 상태입니다.)
	void StartHeal();
	void StopHeal();

	UPROPERTY(Transient)
	FName WeaponName = ""; // 무기 이름
	UPROPERTY()
	int RifleAmmo = 0;     // 자동 소총 탄 수
	UPROPERTY()
	int PistolAmmo = 0;    // 권총 탄 수
	UPROPERTY(Transient)
	int LoadedAmmo = 0;    // 장전된 탄 수
	UPROPERTY(Transient)
	int EquipAmmo = 0;	   // 현재 들고있는 총의 탄 수
	UPROPERTY(Replicated)
	FString TeamName = ""; 

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

protected:
	
	// , ReplicatedUsing = OnRep_UpdatePlayerHP
	UPROPERTY(Transient, Replicated) // 직렬화에서 제회시키는 키워드이다. 데이터가 상시 변하기 때문에 보관하는게 의미가 없다.
	float PlayerHP = 100; // 플레이어 체력
	UPROPERTY(Transient)
	int PlayerXP = 0; // 매치마다 얻은 점수
	UPROPERTY()
	class AMultiPlayerBase* Player = nullptr;

	// 자동 회복
	UPROPERTY(EditAnywhere)
	UCurveFloat* HealCurve; // TimeLine이 따라가는 커브
	FTimeline HealTimeline; // 자동 회복 타임라인
	UFUNCTION()
	void SetHeal();		    // 체력 회복 시작 할 때 작동하는 함수입니다.
	UFUNCTION()
	void SetHealFinish();   // 체력 회복 끝날 때 작동하는 함수입니다.

	// 체력이 업데이트 되면 플레이어(컨트롤러가 없는 == 적)에 업데이트하는 함수입니다.
	UFUNCTION()
	void OnRep_UpdatePlayerHP();

private:

public:

};
