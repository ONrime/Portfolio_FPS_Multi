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

	/*��Ÿ�� ����� ���� ���� �Լ���*/
	void PlayVaultMontage();   // �Ѿ�� ���
	void PlayClimbMontage();   // �ö󰡱� ���
	void PlaySlidingMontage(); // �����̵� ���
	void PlayFireMontage();    // �Ѿ� �߻� ���
	void PlayMeleeMontage();   // ���� ���� ���
	void StopMontage();        // ��Ÿ�� ����


	/*��������Ʈ*/
	FAnimNotifyDelegate VaultDelegate; // �Ѿ�Ⱑ ���� �� �÷��̾�� ���ε��� �Լ� ����
	FAnimNotifyDelegate ClimbDelegate; // �ö󰡱Ⱑ ���� �� �÷��̾�� ���ε��� �Լ� ����


/*
* �����ϸ� �ȵǴ� ������ �Լ���
*/
private:
	/*���� ���� ������*/
	// ������ ���� ���� ���� ������, �����ڰ� ���� ���µ��� ��Ÿ���� ������ �Դϴ�.
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	UClass* DownStateNClass = nullptr;  // (����)��ü ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	UClass* DownStateBClass = nullptr;  // (����)��ü ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	UClass* UpperStateNClass = nullptr; // (����)��ü ���� ���� ����


	// ������ ���µ�� ������ ��������� �ϴ� ���µ��� ��Ÿ���� �������Դϴ�.
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsFalling = false;   // �߶����¸� �˷��ִ� �����Դϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsJumped = false;    // ������ �ߴ��� �˷��ִ� �����Դϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsProne = false;     // ���帮�� ���������� �˷��ִ� �����Դϴ�. (��Ȯ���� ���帮�� ������ ���� ���� == StateStart())
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsProneBack = false; // ���帮��(���� �ٴ��� ���ؼ� == �����ִ� ����) ���������� �˷��ִ� �����Դϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsHandUp = false;    // ���� ���� �ε����� �Ǹ� ���Ⱑ ���� ���� �ʰ� �ϱ����� ���� �������� ��ƾ� �Ǵ� ���¸� ��Ÿ���� �����Դϴ�.
	


	/*��ü�� ��ü ������ ���õ� ������
	* ���� �������� �ִϸ��̼� ������ ���� ��ü�� ��ü�� ���� �����̴� �ý����� �����Ͽ����ϴ�.
	* �÷��̾ �������� ���� �� ��ü�� �����ǰ� ��ü�� ��Ʈ�ѷ��� ���� �����̸�
	* ��Ʈ�ѷ��� ��ü�� ���̻� ������ �� ���� ��ġ�� ���� �Ҷ� ��ü�� ȸ���ϴ� �ý��� �Դϴ�.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim, Meta = (AllowPrivateAccess = true))
	float UpperYaw = 0.0f;                           // ��ü Z�� ȸ�� ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim, Meta = (AllowPrivateAccess = true))
	float UpperPitch = 0.0f;                         // ��ü Y�� ȸ�� ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Aim, Meta = (AllowPrivateAccess = true))
	float UpperProneYaw = 0.0f;                      // ���帮�� ���� �϶��� ��ü Z�� ȸ�� ��
	bool IsTurn = false;                             // ��ü�� ȸ���� �ص� �Ǵ� ��Ȳ������ �����ϴ� ����
	FRotator StandingDir = FRotator::ZeroRotator;    // ��ü�� �������� ���ϱ� ���� �ʿ��� ��ü�� ������ ������ Rotation��
	FRotator StandingDirEnd = FRotator::ZeroRotator; // ��ü�� ȸ���� �ؾ� �� �� ����Ǵ� ������
	/* �÷��̾��� ��ü�� �����ǰ� ��ü�� �þ߿� ���� �����̰� �ϱ� ���� �ʿ��� ���� ��ȯ�ϴ� �Լ��Դϴ�.
	* �⺻������ �÷��̾�� �þ߿� ���� ���� �����Դϴ�.
	* ���� �÷��̾��� �þ߰� �����̴� ��ŭ ��ü�� �ݴ�� �����ٸ� ��ü�� ������ ���·� ��ü�� �����̴� ����� �� �� �ֽ��ϴ�.
	* �̴�θ� �Ѵٸ� ��ü�� ������ü ��ü�� �ڸ� �ٶ󺸴� ������ȭ�� ���� ��Ȳ�� �������� ������
	* ���� ������ ���ϰ� ������ �Ѿ�� ��ü�� �÷��̾ �ٶ󺸴� �������� ȸ���ϰ� �մϴ�.
	* �ڵ�� �����ϸ� �÷��̾ �������� ������ �߽���(�߽��� = StandingDirEnd)�� ���ϰ�
	* �� �߽����� �þ߿��� ���̸� ���� �� ��(Yaw)���� ��ü�� �ݴ�� �����̰� �մϴ�.
	*
	*  ���� ���⼭ ���帮�� ��ɵ� ���� �ٷ�ϴ�.
	* �ٸ� ���� �ִٸ� ���� ������ �Ѿ�� �߽���(�߽��� = StandingDirEnd)�� 180�� ȸ���ϰ� �ϴ� �Ͱ� �����ִ��� ������ִ����� �Ǵܸ� �մϴ�.
	*/
	void TurnBodyYaw(class AMultiPlayerBase* Player, float& Yaw, float& ProneYaw); // ��ü ȸ�� ���� ���� �� ���� �Լ�
	
	

	/*�ȱ�, �޸���� ���� �����ӿ� ���õ� ������
	* �÷��̾ ���� ������ ������ �������� ��ü�� �����̴� �ִϸ��̼��� �������� �ý����� �����Ͽ����ϴ�.
	* BlendSpace�� �ƴ� BlendMulti�� �̿��ϱ� ������ �� ���⿡ ���� ��ġ(��)�� �ʿ��մϴ�.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	float PlayerSpeed = 0.0f;     // �÷��̾��� �ӵ��� ��Ÿ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	float DirForward = 0.0f;      // �÷��̾��� �ӵ��� ����ؼ� �� ~ �� == 1.0 ~ -1.0 ���� ���ϴ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	float DirRight = 0.0f;        // �÷��̾��� �ӵ��� ����ؼ� �� ~ �� == 1.0 ~ -1.0 ���� ���ϴ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = true))
	FVelocityBlend VelocityBlend; // ���� �÷��̾ �����̰� �ִ� ������ ��, ��, ��, ��� ��Ÿ���� ����ü �Դϴ�.
	// VelocityBlend���� ���ϴ� �Լ�
	void GetVeloctyBlend(FVector PlayerVelocity, FRotator PlayerRotator, FVelocityBlend& Current);



	/*���帮�⿡ ���õ� ������
	* ������� �� �� ����(���帮�� or �����ֱ� or �·� ������ֱ� or ��� ������ֱ�)�� ���� ��ü�� ��, ���� �ִϸ��̼��� �޶����ϴ�.
	* �ڿ������� ������ ���ؼ��� �� ���¸� ��Ÿ���� ���� �ƴ� ��ġ(��)���� ���¸� ��Ÿ���� ����(����ü)�� �ʿ��մϴ�.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Prone, Meta = (AllowPrivateAccess = true))
	FVelocityBlend ProneRotBlend; // ���� ���帮���� ���¸� ��ġ�� ��Ÿ���ϴ�.(���帮�� or �����ֱ� or �·� ������ֱ� or ��� ������ֱ�)
	FVelocityBlend GetProneRotBlend(FVector ProneDir, FRotator PlayerRot); // ProneRotBlend�� ���ϴ� �Լ�
	


	/*Foot IK�� ���õ� ������ �Լ���
	* �߿� ��� ���� ���� ���� ������ ��ġ�� �����ϴ� ����Դϴ�.
	* �ڿ������� FootIK�� �����ϱ� ���ؼ��� ���� ������ ��ġ�� �����ϴ� ���� �ƴ�
	* ���(��Ʈ)�� ��ġ��, ������ ��ġ(����Ʈ)�� �����ϴ� ������ �ʿ��մϴ�.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float RootZLoc = 0.0f;                           // ���(��Ʈ)�� Z�� ��ġ ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float LeftFootAlpha = 0.0f;                      // �޹��� �ٴڿ� ���� �Ǿ������� ��Ÿ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float RightFootAlpha = 0.0f;                     // �������� �ٴڿ� ���� �Ǿ������� ��Ÿ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	float RootLocAlpha = 0.0f;                       // ���(��Ʈ)�� ��ġ�� ����Ǿ�� �ϴ��� ��Ÿ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	FRotator LeftFootRot = FRotator::ZeroRotator;    // �޹��� ������ ��Ÿ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	FRotator RightFootRot = FRotator::ZeroRotator;   // �������� ������ ��Ÿ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	FVector LeftFootLoc = FVector::ZeroVector;       // �޹��� ��ġ�� ��Ÿ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FootIK, Meta = (AllowPrivateAccess = true))
	FVector RightFootLoc = FVector::ZeroVector;      // �������� ��ġ�� ��Ÿ���� ����
	// �� ���� ��ġ, ������ ���ϴ� �Լ�
	bool FootIKTracer(class AMultiPlayerBase* Player, FName Name , FName Name2, FVector& Loc, FRotator& Rot, float& Distance);
	bool RootLock = false; // ��Ʈ�� �����ϰ� �����̴� ���� �����ϱ� ���� ��״� ����� �ʿ��ϴ�.

	/* Prone IK�� ���õ� ������ �Լ���
	* ������� �� �ٴ��� ������ �°� ���� ������ �����ϴ� ����Դϴ�.
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
	/* ������� �� pelvis�� ��ġ�� Trace�� ������ ���� �ٴ��� ������ ���մϴ�. (ProneRootIK �Լ�)
	* ProneMiddleIK �Լ��� ProneEndIK�� �ٴڿ� ���� ������ ���ϰ�
	* ProneRootIK �Լ��� ProneMiddleIK �Լ��� �� �ٴ��� ����(ImpactNomal)�� ��ȯ�Ͽ�
	* ProneMiddleIK �Լ�, ProneEndIK �Լ��� ������ ������ �˴ϴ�.
	* ���� ����, ���� ������ �������� �����Ƿ� ��Ȳ�� ���� ������ ����, ���� ������ �Ǿ���մϴ�.
	* �̸� �ǰ��� �ϴ� �Լ��� ProneDegrees�Դϴ�.
	*/
	// �߽�(pelvis)�� ��� (Root)
	FVector ProneRootIK(class AMultiPlayerBase* Player, FName BoneName, FRotator& Rot);
	// �߽�(Root)�࿡ ���� �߰� �� ��� ()
	FVector ProneMiddleIK(class AMultiPlayerBase* Player, FName BoneName, FRotator& Rot, FVector Dir);
	// �߰��࿡ ���� �� ���� ��� ()
	void ProneEndIK(class AMultiPlayerBase* Player, FName BoneName, FRotator& Rot, FVector Dir);
	// ����, ���� ������ z���� ũ��� �ǰ����Ͽ� ������ ��ȯ�մϴ�.
	float ProneDegrees(float ImpactNomalXY, float ImpactNomalZ, float DirXY, float DirZ, float XY);



	/*����ý��ۿ� ���õ� ������
	* ���� ������ ���� ��ü�� ô�߰� �����̰� �մϴ�.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cover, Meta = (AllowPrivateAccess = true))
	float CoverAngle = 0.0f; // ���� ������ ��Ÿ���� ���� �� ������ ���� ô�߰� ȸ���մϴ�.



	// ��Ÿ��
	UAnimMontage* Vault_Montage;   // �Ѿ�� ��Ÿ��
	UAnimMontage* Climb_Montage;   // �ö󰡱� ��Ÿ��
	UAnimMontage* Sliding_Montage; // �����̵� ��Ÿ��
	UAnimMontage* Melee_Montage;   // ���� ���� ��Ÿ��



	// �ִ� ��Ƽ���� �Լ���
	UFUNCTION()
	void AnimNotify_ClimbEnd(); // �ö󰡱Ⱑ ���� ��
	UFUNCTION()
	void AnimNotify_VaultEnd(); // �Ѿ�Ⱑ ���� ��


};
