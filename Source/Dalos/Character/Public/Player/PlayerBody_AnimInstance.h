// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "PlayerBody_AnimInstance.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(FAnimNotifyDelegate);

UCLASS()
class DALOS_API UPlayerBody_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPlayerBody_AnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/*몽타주 재생및 정지 관련 함수들*/
	void PlayVaultMontage();   // 넘어가기 재생
	void PlayClimbMontage();   // 올라가기 재생
	void PlaySlidingMontage(); // 슬라이딩 재생
	void PlayFireMontage();    // 총알 발사 재생
	void PlayMeleeMontage();   // 근접 공격 재생
	void StopMontage();        // 몽타주 정지


	/*딜리게이트*/
	FAnimNotifyDelegate VaultDelegate; // 넘어가기가 끝날 때 플레이어에서 바인딩된 함수 실행
	FAnimNotifyDelegate ClimbDelegate; // 올라가기가 끝날 때 플레이어에서 바인딩된 함수 실행


/*
* 공개하면 안되는 변수와 함수들
*/
private:
	/*상태 관련 변수들*/
	// 정해진 범위 내의 상태 변수들, 개발자가 정한 상태들을 나타내는 변수들 입니다.
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	UClass* DownStateNClass = nullptr;  // (현재)하체 관련 상태 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	UClass* DownStateBClass = nullptr;  // (과거)하체 관련 상태 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	UClass* UpperStateNClass = nullptr; // (현재)상체 관련 상태 변수


	// 정해진 상태들과 별개로 구분지어야 하는 상태들을 나타내는 변수들입니다.
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsFalling = false;   // 추락상태를 알려주는 변수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsJumped = false;    // 점프를 했는지 알려주는 변수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsProne = false;     // 엎드리기 상태인지를 알려주는 변수입니다. (정확히는 엎드리기 시작할 때의 상태 == StateStart())
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsProneBack = false; // 엎드리기(등이 바닥을 향해서 == 누어있는 상태) 상태인지를 알려주는 변수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsHandUp = false;    // 벽에 손을 부딪히게 되면 무기가 벽에 닿지 않게 하기위해 손을 안쪽으로 모아야 되는 상태를 나타내는 변수입니다.
	


	/*상체와 하체 구현에 관련된 변수들
	* 좀더 현실적인 애니메이션 구현을 위해 상체와 하체가 따로 움직이는 시스템을 구현하였습니다.
	* 플레이어가 움직이지 않을 때 하체는 고정되고 상체만 컨트롤러에 따라 움직이며
	* 컨트롤러가 상체가 더이상 움직일 수 없는 수치에 도달 할때 하체가 회전하는 시스템 입니다.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim, Meta = (AllowPrivateAccess = true))
	float UpperYaw = 0.0f;                           // 상체 Z축 회전 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim, Meta = (AllowPrivateAccess = true))
	float UpperPitch = 0.0f;                         // 상체 Y축 회전 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim, Meta = (AllowPrivateAccess = true))
	float UpperProneYaw = 0.0f;                      // 엎드리기 상태 일때의 상체 Z축 회전 값
	bool IsTurn = false;                             // 하체가 회전을 해도 되는 상황인지를 구분하는 변수
	FRotator StandingDir = FRotator::ZeroRotator;    // 상체의 움직임을 구하기 위해 필요한 하체가 고정된 시점의 Rotation값
	FRotator StandingDirEnd = FRotator::ZeroRotator; // 하체가 회전을 해야 할 때 변경되는 고정값
	/* 플레이어의 하체는 고정되고 상체만 시야에 따라 움직이게 하기 위해 필요한 값을 반환하는 함수입니다.
	* 기본적으로 플레이어는 시야에 따라 같이 움직입니다.
	* 만약 플레이어의 시야가 움직이는 만큼 하체를 반대로 돌린다면 하체가 고정된 상태로 상체만 움직이는 모습을 볼 수 있습니다.
	* 이대로만 한다면 하체가 고정된체 상체가 뒤를 바라보는 공포영화와 같은 상황이 펼쳐지기 때문에
	* 일정 범위를 정하고 범위를 넘어가면 하체가 플레이어가 바라보는 지점으로 회전하게 합니다.
	* 코드로 설명하면 플레이어가 움직이지 않으면 중심점(중심점 = StandingDirEnd)을 정하고
	* 그 중심점과 시야와의 차이를 구해 이 값(Yaw)으로 하체를 반대로 움직이게 합니다.
	*
	*  이제 여기서 엎드리기 기능도 같이 다룹니다.
	* 다른 점이 있다면 일정 범위를 넘어가면 중심점(중심점 = StandingDirEnd)을 180도 회전하게 하는 것과 누워있는지 엎드려있는지만 판단만 합니다.
	*/
	void TurnBodyYaw(class AMultiPlayerBase* Player, float& Yaw, float& ProneYaw); // 상체 회전 값을 구할 때 쓰는 함수
	
	

	/*걷기, 달리기와 같이 움직임에 관련된 변수들
	* 플레이어가 보는 시점의 방향을 기준으로 하체의 움직이는 애니메이션이 정해지는 시스템을 구현하였습니다.
	* BlendSpace가 아닌 BlendMulti를 이용하기 때문에 각 방향에 대한 수치(값)가 필요합니다.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	float PlayerSpeed = 0.0f;     // 플레이어의 속도를 나타내는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	float DirForward = 0.0f;      // 플레이어의 속도에 비례해서 전 ~ 후 == 1.0 ~ -1.0 값을 구하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	float DirRight = 0.0f;        // 플레이어의 속도에 비례해서 우 ~ 좌 == 1.0 ~ -1.0 값을 구하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	FVelocityBlend VelocityBlend; // 현재 플레이어가 움직이고 있는 방향을 전, 후, 좌, 우로 나타내는 구조체 입니다.
	// VelocityBlend값을 구하는 함수
	void GetVeloctyBlend(FVector PlayerVelocity, FRotator PlayerRotator, FVelocityBlend& Current);



	/*엎드리기에 관련된 변수들
	* 엎드렸을 때 각 상태(엎드리기 or 누워있기 or 좌로 엎드려있기 or 우로 엎드려있기)에 따라 상체의 상, 하의 애니메이션이 달라집니다.
	* 자연스러운 블랜딩을 위해서는 각 상태를 나타내는 값이 아닌 수치(값)으로 상태를 나타내는 변수(구조체)가 필요합니다.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prone, Meta = (AllowPrivateAccess = true))
	FVelocityBlend ProneRotBlend; // 현재 엎드리기의 상태를 수치로 나타냅니다.(엎드리기 or 누워있기 or 좌로 엎드려있기 or 우로 엎드려있기)
	FVelocityBlend GetProneRotBlend(FVector ProneDir, FRotator PlayerRot); // ProneRotBlend를 구하는 함수
	


	/*Foot IK에 관련된 변수와 함수들
	* 발에 닿는 땅에 따라 발의 각도와 위치를 변경하는 기능입니다.
	* 자연스러운 FootIK를 구현하기 위해서는 발의 각도와 위치만 구현하는 것이 아닌
	* 골반(루트)의 위치나, 무릎의 위치(조인트)를 보정하는 값들이 필요합니다.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float RootZLoc = 0.0f;                           // 골반(루트)의 Z축 위치 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float LeftFootAlpha = 0.0f;                      // 왼발이 바닥에 접지 되었는지를 나타내는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float RightFootAlpha = 0.0f;                     // 오른발이 바닥에 접지 되었는지를 나타내는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float RootLocAlpha = 0.0f;                       // 골반(루트)의 위치가 변경되어야 하는지 나타내는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	FRotator LeftFootRot = FRotator::ZeroRotator;    // 왼발의 각도를 나타내는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	FRotator RightFootRot = FRotator::ZeroRotator;   // 오른발의 각도를 나타내는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	FVector LeftFootLoc = FVector::ZeroVector;       // 왼발의 위치를 나타내는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	FVector RightFootLoc = FVector::ZeroVector;      // 오른발의 위치를 나타내는 변수
	// 각 발의 위치, 각도를 구하는 함수
	bool FootIKTracer(class AMultiPlayerBase* Player, FName Name , FName Name2, FVector& Loc, FRotator& Rot, float& Distance);
	bool RootLock = false; // 루트가 무리하게 움직이는 것을 방지하기 위해 잠그는 기능이 필요하다.

	/* Prone IK에 관련된 변수와 함수들
	* 엎드렸을 때 바닥의 각도에 맞게 몸의 각도를 변경하는 기능입니다.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prone, Meta = (AllowPrivateAccess = true))
	FRotator ProneIK_Upper_Rot = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prone, Meta = (AllowPrivateAccess = true))
	FRotator ProneIK_Pelvis_Rot = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prone, Meta = (AllowPrivateAccess = true))
	FRotator ProneIK_Right_Knee_Rot = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prone, Meta = (AllowPrivateAccess = true))
	FRotator ProneIK_Left_Knee_Rot = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prone, Meta = (AllowPrivateAccess = true))
	FRotator ProneIK_Right_Foot_Rot = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prone, Meta = (AllowPrivateAccess = true))
	FRotator ProneIK_Left_Foot_Rot = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prone, Meta = (AllowPrivateAccess = true))
	FRotator ProneIK_Right_Hand_Rot = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prone, Meta = (AllowPrivateAccess = true))
	FRotator ProneIK_Left_Hand_Rot = FRotator::ZeroRotator;
	// Prone IK 
	/* 엎드렸을 때 pelvis뼈 위치에 Trace를 밑으로 쏴서 바닥의 각도를 구합니다. (ProneRootIK 함수)
	* ProneMiddleIK 함수와 ProneEndIK도 바닥에 따른 각도를 구하고
	* ProneRootIK 함수와 ProneMiddleIK 함수는 각 바닥의 방향(ImpactNomal)을 반환하여
	* ProneMiddleIK 함수, ProneEndIK 함수의 각도에 기준이 됩니다.
	* 뼈는 상위, 하위 계층이 나뉘어져 있으므로 상황에 따라 기준이 상위, 하위 계층이 되어야합니다.
	* 이를 판가름 하는 함수가 ProneDegrees입니다.
	*/
	// 중심(pelvis)축 계산 (Root)
	FVector ProneRootIK(class AMultiPlayerBase* Player, FName BoneName, FRotator& Rot);
	// 중심(Root)축에 따른 중간 축 계산 ()
	FVector ProneMiddleIK(class AMultiPlayerBase* Player, FName BoneName, FRotator& Rot, FVector Dir);
	// 중간축에 따른 끝 지점 계산 ()
	void ProneEndIK(class AMultiPlayerBase* Player, FName BoneName, FRotator& Rot, FVector Dir);
	// 상위, 하위 계층을 z축의 크기로 판가름하여 각도를 반환합니다.
	float ProneDegrees(float ImpactNomalXY, float ImpactNomalZ, float DirXY, float DirZ, float XY);



	/*엄폐시스템에 관련된 변수들
	* 엄폐 각도에 따라 상체의 척추가 움직이게 합니다.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cover, Meta = (AllowPrivateAccess = true))
	float CoverAngle = 0.0f; // 엄폐 각도를 나타내는 변수 이 각도에 따라 척추가 회전합니다.



	// 몽타주
	UAnimMontage* Vault_Montage;   // 넘어가기 몽타주
	UAnimMontage* Climb_Montage;   // 올라가기 몽타주
	UAnimMontage* Sliding_Montage; // 슬라이딩 몽타주
	UAnimMontage* Melee_Montage;   // 근접 공격 몽타주



	// 애님 노티파이 함수들
	UFUNCTION()
	void AnimNotify_ClimbEnd(); // 올라가기가 끝날 때
	UFUNCTION()
	void AnimNotify_VaultEnd(); // 넘어가기가 끝날 때


};
