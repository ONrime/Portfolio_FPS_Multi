// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Dalos/Character/Public/CharacterBase.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "Interface/Public/ChangeState_Interface.h"
#include "MultiPlayerBase.generated.h"

DECLARE_DELEGATE(FMultiPlayerCheckDelegate)

/**********************************************************************************************************
 * �÷��̾� �Է�ǥ��
 * �÷��̾��� ���� ��ȯ�� �̷������ �ٸ� Ŭ���̾�Ʈ ���� ĳ���ʹ� ��Ʈ�ѷ��� �������� �ʾƼ�
 * �Է¿� ���� ������ ���� ���� �ʽ��ϴ�. �׷��� �Է¿� ���� ������ Enum���� ���� �����ϱ�� �߽��ϴ�.
***********************************************************************************************************/
UENUM(BlueprintType)
enum class EPlayerPress : uint8 {
	CROUCH UMETA(DisplayName = "Crouch"),
	JUMP UMETA(DisplayName = "Jump"),
	SPLINT UMETA(DisplayName = "Splint"),
	ADS UMETA(DisplayName = "ADS"),
	FIRSTGUN UMETA(DisplayName = "FirstGun"),
	SECONDGUN UMETA(DisplayName = "SecondGun"),
	UNARMED UMETA(DisplayName = "UnArmed"),
	INTERACTION UMETA(DisplayName = "Interaction"),
	PRONE UMETA(DisplayName = "Prone")
};

/**********************************************************************************************************
* �÷��̾� ����ǥ��
* ��ư�� ������ ���� ���� ��ȯ �̿ܿ� ���ǹ��� ���¸� ��� �� ��, ���� ��ȯ�� ���õ� ó���� �� ��
* State Ŭ������ ���� �� ���� �� ��ȯ�ؾ� �� ���¸� �����ؼ� State Ŭ���� ������ ó���ϰԲ� �ϱ� ���� �� �ʿ��մϴ�.
* (�̰� ������ APlayerCharacter �� ���� State�� ��ȯ�ؾ� �ϱ� ������ ��ġ �� �� �ϸ� ��� State�� ������ �ؾ� �մϴ�.)
***********************************************************************************************************/

//�÷��̾� ��ü ���� ǥ�� �ϴ� ������
UENUM(BlueprintType)
enum class EPlayerDownState : uint8 {
	STANDING UMETA(DisplayName = "Standing"),
	CROUCH UMETA(DisplayName = "Crouch"),
	SPLINT UMETA(DisplayName = "Splint"),
	SLIDING UMETA(DisplayName = "Sliding"),
	PRONE UMETA(DisplayName = "Prone")
};
//�÷��̾� ��ü ���� ǥ�� �ϴ� ������
UENUM(BlueprintType)
enum class EPlayerUpperState : uint8 {
	UNARMED UMETA(DisplayName = "Unarmed"),
	ARMED UMETA(DisplayName = "Armed"),
	ADS UMETA(DisplayName = "ADS")
};

UCLASS()
class DALOS_API AMultiPlayerBase : public ACharacterBase, public IChangeState_Interface
{
	GENERATED_BODY()
	
/*�����Ϳ�  ///////////////////////////////////////////////////////////////////////////////////////////////////////
*/
public:
	AMultiPlayerBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;   // �÷��̾� ī�޶�
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;   // ī�޶� ��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AloowPrivateAccess = "true"))
	class USkeletalMeshComponent* ArmMesh;  // �� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AloowPrivateAccess = "true"))
	class USkeletalMeshComponent* BodyMesh; // �� ����

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate = 45.0f;             // ī�޶� ��, �� �ӵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate = 45.0f;           // ī�޶� ��, �� �ӵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapone)
	float BaseSpreadSize = 70.0f;           // �⺻���� ź������ ũ�⸦ ��Ÿ���� ����

/*������ ������ ��ӹ޾ƾ��ϴ� ������ �Լ��� ///////////////////////////////////////////////////////////////////////////
*/
protected:
	/*�̺�Ʈ �Լ��� **********************************************************************
	*/
	// �÷��̾ �����ǰ� ���� ������ �� �۵��Ǵ� �Լ�
	// �þ߹� ������ ����, ���� �ʱ�ȭ, ���� ����
	virtual void BeginPlay() override;                
	virtual void PostInitializeComponents() override; // ������Ʈ �ʱ�ȭ ���Ŀ� �۵��Ǵ� �Լ� (AnimIns �ҷ����� �� ��������Ʈ ����)
	virtual void Tick(float DeltaTime) override;      // ������, ī�޶�, ź �߻�, �ݵ�, ���º� Tick ó��, ���ڼ� ȿ������ ó���մϴ�.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; // �Է� ���ε��� �ϴ� �Լ�
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // �� ���Ͱ� ���� �� �۵��Ǵ� �Լ�

	/***********************************************************************************/


	/*�÷��̾�� �����ִ� �� ����ؾ� �Ǵ� Ŭ������ *******************************************
	*/
	UPROPERTY()
	class UPlayerArm_AnimInstance* ArmAnim = nullptr;   // �� �ִϸ��̼� �ν��Ͻ�
	UPROPERTY()
	class UPlayerBody_AnimInstance* BodyAnim = nullptr; // �� �ִϸ��̼� �ν��Ͻ�
	UPROPERTY()
	class UPlayerBody_AnimInstance* LegAnim = nullptr;  // �ٸ� �ִϸ��̼� �ν��Ͻ�
	UPROPERTY()
	class AMultiPlayer_HUD* HUD = nullptr;              // �÷��̾� ui�� ����ϴ� hud

	/***********************************************************************************/


	/*���� ******************************************************************************/
	// �⺻ ���� ----------------------------------------------------------
	//
	UPROPERTY()
	class AWeaponeBase* EquipWeapone = nullptr;    // ���� ����ִ� ����(������)
	UPROPERTY()
	class AWeaponeBase* EquipWeaponeArm = nullptr; // ���� ����ִ� ����(�ȿ���)
	class AWeaponeBase* BackWeapone1 = nullptr;    // �� �ڿ� �ִ� ���� ù ��°
	class AWeaponeBase* BackWeapone2 = nullptr;    // �� �ڿ� �ִ� ���� �� ��°
	class AWeaponeBase* LookWeapone = nullptr;     // ó�� ���� �ִ� ����
	//-------------------------------------------------------------------

	// �˻�� ����ؾ� �ϴ� �Լ��� ------------------------------------------
	//
	/*���ڼ��� ���õ� ó���� �ϴ� �Լ��Դϴ�.
	* �� �Լ������� ī�޶��� ForwardVector�� �������� Trace�� �߻��Ͽ�
	* ���ڼ��� ��Ʈ�Ǵ� Actor(��, ���� ��)�� �Ÿ�, ��ġ���� ���Ͽ�
	* ź ������ �󸶳� �̷������ �ϴ��� ���������� �����Ͽ� HUD�� ǥ���ϴ� ����� �����ϰ� �ֽ��ϴ�.*/
	bool CrossHairCheck();

	/*�ݵ��� ���õ� ó���� �ϴ��Լ��Դϴ�.
	* Trace�� �߻��ؼ� ��Ʈ�� �Ÿ��� ���� ź������ ���Ǹ�
	* �ݵ� ���Ŀ� ����� ���۵� �������� �ǵ��� ���� ����� �ֽ��ϴ�.*/
	bool RecoilCheck();

	bool InteractionCheck();                  // ��ȣ�ۿ��� ������ Actor������ Ȯ���ϴ� �Լ��Դϴ�.
	void WeaponCheck(AWeaponeBase* Check);    // ���� ��� �ִ� ������� Ȯ���ؼ� �������� ������ �����մϴ�.

	// ���⸦ �ٲٴ� �Լ��Դϴ�.
	// ��� �ִ� ���⸦ ������ ������ � �ִ� ���⸦ ������ �ű�ϴ�.
	void WeaponeChange(EPlayerPress Rress);
	void EquipGunOnHand(AWeaponeBase* Equip); // ���� ����ִ� ���⸦ �Ű������� �ٲٴ� �Լ�
	void FireAutoOn();                        // ���� �ð����� ���縦 �� �� �ְ� Ÿ�̸Ӹ� �����߰� �� Ÿ�̸Ӱ� ������ �۵��Ǵ� �Լ��Դϴ�.
	//-------------------------------------------------------------------

	// ��ݿ� ���Ǵ� ������ ----------------------------------------------
	//
	bool IsFire = false;     // true�� ��� �� �Դϴ�.
	bool IsFireAuto = false; // true�� ���縦 �ؾ��մϴ�.
	FTimerHandle FireTimer;  // ���縦 �Ҷ� ���Ǹ� ź�� ���� ���� �θ� ����� �ǰ� �ϱ� ���ؼ� ���Ǵ� Ÿ�̸��Դϴ�.
	float ThreeCount = 0.0f; // ������ üũ�� �����Դϴ�.
	//-------------------------------------------------------------------

	// �ݵ��� ���Ǵ� ������ ----------------------------------------------
	//
	bool IsCrossHairRed = false;                      // ���ڼ��� ���������� Ȯ���ϴ� ����
	FRotator RecoilReturnRot = FRotator::ZeroRotator; // �ݵ��� �ǵ��� ���� Rotation
	FVector RecoilReturnLoc = FVector::ZeroVector;    // �ݵ��� �ǵ��� ���� ���� ��ġ (��ɻ����� ������ ������ Trace ������ �ֽ��ϴ�.)
	FVector BulletFireLoc = FVector::ZeroVector;      // ź������ ���� ������ �Ѿ��� �������� ��ġ
	float CurrentSpreadSize = 0.0f;                   // ���� ź������ ũ�⸦ ��Ÿ���� ����
	//-------------------------------------------------------------------
	
	/***********************************************************************************/

	/*��Ʈ�ѷ� **************************************************************************/
	FRotator ControllerRot = FRotator::ZeroRotator; // �ٸ� Ŭ���̾�Ʈ���� ��Ʈ�ѷ��� ȸ������ ������ ���� �뵵�� �����Դϴ�.
	float AimDirRight = 0.0f, AimDirUp = 0.0f;      // ��Ʈ�ѷ��� ���� ���� ���� Ű�� (AnimBP���� ����ϱ� ����)
	float InputDirForward = 0.0f;                   // ��Ʈ�ѷ��� ���� ���� �̵�(��, ��) Ű�� (AnimBP���� ����ϱ� ����)
	float InputDirRight = 0.0f;                     // ��Ʈ�ѷ��� ���� ���� �̵�(��, ��) Ű�� (AnimBP���� ����ϱ� ����)
	FVector MoveDir = FVector::ZeroVector;          // �������� �Ǵ� ������ ��Ÿ���� �����Դϴ�.
	UPROPERTY()
	EPlayerPress PlayerPress;                       // �÷��̾ � ��ư�� ���������� ��Ÿ���ϴ�.
	bool IsCameraLock = false;                      // true�� ī�޶��� �������� ��ݴϴ�.
	void PlayerMove();                              // �Է¿� ���� ���� ���� ���� ĳ���͸� �̵���Ű�� �Լ��Դϴ�.

	/***********************************************************************************/

	/*���¿� ���õ� ������� �Լ� **********************************************************/
	UPROPERTY()
	class UPlayerUpperStateBase* UpperState = nullptr; // ��ü ���¸� ��Ÿ���ϴ�.
	UPROPERTY()
	class UPlayerDownStateBase* DownState = nullptr;   // ��ü ���¸� ��Ÿ���ϴ�.
	EPlayerUpperState EUpperState;					   // ��ü ���¸� ��Ÿ���� enum
	EPlayerDownState EDownState;					   // ��ü ���¸� ��Ÿ���� enum
	bool IsDead = false;							   // ���� ���¸� �˷��ִ� ����
	FTimerHandle HealTimer;							   // ���� �ð��� ������ ü���� ȸ���Ǵµ� �� �ð��� ���� Ÿ�̸��Դϴ�.(���� �������� ���� ���)
	bool IsProne = false;							   // ���帰 ���¸� �˷��ݴϴ�.(���帮��� ���ϴ����� �ƴ� ���帮�Ⱑ �������� �� �۵�)
	void PlayerDead();								   // �÷��̾ �״� ���·� ����ϴ�.

	/***********************************************************************************/

	/*��Ÿ(���� �ٸ� ��ɺ��� ���� �� �Դϴ�.)************************************************/
	// �ö󰡱� �� �Ѿ�� ------------------------------------------------
	//
	FVector WallLoc = FVector::ZeroVector;        // �νĵ� ���� ��ġ�� ��Ÿ���� �����Դϴ�.
	FVector WallHeight = FVector::ZeroVector;     // �νĵ� ���� ������ġ�� ��Ÿ���� �����Դϴ�.
	FVector WallBackHeight = FVector::ZeroVector; // �νĵ� ���� �� ������ ���̸� ��Ÿ���� �����Դϴ�.
	FVector WallNomal = FVector::ZeroVector;      // �νĵ� ���� ������ ��Ÿ���� �����Դϴ�.
	bool WallForwardTracer(FVector& Loc, FVector& Nomal);               // �÷��̾��� �տ� ���� �ִ����� �ν��ϴ� Trace�� ��� �Լ��Դϴ�.
	bool WallHeightTracer(FVector Loc, FVector Nomal, FVector& Height); // �νĵ� ���� ���̸� �˱� ���� Trace�� ��� �Լ��Դϴ�.
	bool WallBackHeightTracer(FVector Loc, FVector& BackHeight);        // �νĵ� ���� �� ������ ���̸� �˱� ���� Trace�� ��� �Լ��Դϴ�.
	void PlayerVault(FVector Loc, FVector Nomal, FVector Height);       // ���� ���¿� ���� �Ѿ�� �� �ö󰡱� �۵�
	void StopVault();         // �Ѿ�⸦ ����ϴ�.
	void StopClimb();         // �ö󰡱⸦ ����ϴ�.
	bool IsWall = false;      // true�� ���� ������
	bool IsWallThick = false; // true�� ���� ����ϴ�.
	//-------------------------------------------------------------------

	// ������ ------------------------------------------------------------
	//
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* HitDecal;     // ���� ��Ʈǥ�ø� ��Ÿ���� ��ƼŬ
	UFUNCTION()
	void HitReactionEnd();         // ��Ʈ ���׼��� ������ �۵��ϴ� �Լ�(����ǥ���� ������� ���ư��� ��)
	FTimerHandle HitReactionTimer; // ��Ʈ ���׼��� ������ �ð��� ��� Ÿ�̸��Դϴ�.
	bool IsHitReactionEnd = false; // ��Ʈ ���׼��� ���������� ǥ���ϴ� �����Դϴ�.
	float HitReactionAlpha = 0.0f; // ����ǥ���� �����θ� ��Ÿ���� ���Դϴ�.
	void StratAutoHeal(); // �ڵ� ü�� ȸ���� �����ϴ� �Լ��Դϴ�.(����� ī�޶� ������ �����ϰ� ���� ���� �����Դϴ�.)

	//-------------------------------------------------------------------

	// ���� ------------------------------------------------------------
	//
	class USoundBase* BulletHitHitSound; // ź�࿡ ��Ʈ �ɶ��� ����Ǵ� �����Դϴ�.
	class USoundBase* MeleeHitHitSound;  // ���� ���ݿ� ��Ʈ �ɶ��� ����Ǵ� �����Դϴ�.
	//-------------------------------------------------------------------

	// �Ǻ� --------------------------------------------------------------
	//
	UMaterialInterface* BlueMat; // ����� ��Ų�Դϴ�.
	UMaterialInterface* RedMat;  // ������ ��Ų�Դϴ�.
	//-------------------------------------------------------------------

	// ī�޶� ��ġ --------------------------------------------------------
	//
	float RootDistance = 0.0f; // FootIK�� ���� �޶����� ��Ʈ���� z���� ���Ѵ�.
	//-------------------------------------------------------------------


	/***********************************************************************************/


/*�Է¿� ���õ� �Լ��� �Դϴ�. //////////////////////////////////////////////////////////////////////////////////////
*/
protected:
	// �Է¿� ���õ� �Լ����Դϴ�. **********************************************************
	void PlayerCrouch();      // �ɱ�(C) Ű�� ���ε� �Լ�
	void PlayerJump();		  // ����(Space) Ű�� ���ε� �Լ�
	void PlayerSplint();	  // �޸���(Shift) Ű�� ���ε� �Լ�
	void PlayerFirstGun();    // 1�� ���� ����(1) Ű�� ���ε� �Լ�
	void PlayerSecondGun();   // 2�� ���� ����(2) Ű�� ���ε� �Լ�
	void PlayerUnArmed();     // ����(3) Ű�� ���ε� �Լ�
	void PlayerInteraction(); // ��ȣ�ۿ�(F) Ű�� ���ε� �Լ�
	void PlayerADS();		  // ��������(���콺 ������Ű) Ű�� ���� �� ���ε� �Լ�
	void PlayerUnADS();		  // ��������(���콺 ������Ű) Ű�� �� �� ���ε� �Լ�
	void PlayerFire();		  // ���(���콺 ����Ű) Ű�� ���� �� ���ε� �Լ�
	void PlayerUnFire();	  // ���(���콺 ����Ű) Ű�� �� �� ���ε� �Լ�
	void PlayerReload();	  // ����(R) Ű�� ���ε� �Լ�
	void PlayerProne();		  // ���帮��(Z) Ű�� ���ε� �Լ�
	void PlayerSelfDead();	  // �׽�Ʈ��
	void PlayerWeaponLever();	  // �׽�Ʈ��

	/***********************************************************************************/


	// Axis�� ���õ� �Լ����Դϴ�.(�̵�, ������) ********************************************
	void MoveForward(float Value); // ��, �� �̵�Ű(W, S) ���ε� �Լ�
	void MoveRight(float Value);   // ��, �� �̵�Ű(A, D) ���ε� �Լ�
	void LookUpAtRate(float Rate); // ��, �� ����Ű(���콺 �̵�) ���ε� �Լ�
	void TurnAtRate(float Rate);   // ��, �� ����Ű(���콺 �̵�) ���ε� �Լ�

	/***********************************************************************************/


/*�����ִ� ������ �����ϴ� �Լ� or ������ ������ //////////////////////////////////////////////////////////////////////
*/
public:
	// �������� ������ � ������ �ؾ��ϴ����� ��Ÿ���� �Լ��Դϴ�.
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	/*���¿� ���õ� ������� �Լ� **********************************************************/
	// �����ڰ� ���� ���µ� ------------------------------------------------
	//
	UPlayerUpperStateBase* GetUpperState() { return UpperState; };
	UPlayerDownStateBase* GetDownState() { return DownState; };
	UClass* UpperStateBClass = nullptr; UClass* DownStateBClass = nullptr;   // �� ���¿� ���� Class�� (AnimBP���� ����ϱ� ����)
	UClass* UpperStateNClass = nullptr; UClass* DownStateNClass = nullptr;   // �� ���¿� ���� Class�� (AnimBP���� ����ϱ� ����)
	void UpperPress(class UPlayerUpperStateBase* state);					 // ��ü�� ���õ� Ű�� ������ �� ���°� ���ϴ� �Լ��Դϴ�.
	void DownPress(class UPlayerDownStateBase* state);						 // ��ü�� ���õ� Ű�� ������ �� ���°� ���ϴ� �Լ��Դϴ�.
	EPlayerUpperState GetEUpperState() { return EUpperState; }
	EPlayerDownState GetEDownState() { return EDownState; }

	/*���³��� ���ϰ� �ִ� ��Ȳ���� ���ϴ°� ������
	* (�ִϸ��̼��� ������ AnimNotify�� �ؼ� �������̽��� �÷��̾�� ���ϴ� �ִϸ��̼��� �������� StateStart�� �϶�� �˷��ݴϴ�.)
	* StateStart�� �۵��ϴ� �Լ��Դϴ�.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "State")
	void CurrentStateStart(bool IsUpper);
	virtual void CurrentStateStart_Implementation(bool IsUpper) override;
	//-------------------------------------------------------------------

	// ���س��� ���µ�� ������ �������� �ٷ��� �ϴ� ���µ� -------------------
	//
	bool IsMove = true;             // false�� �÷��̾ ������ �� �����ϴ�.
	bool IsPlayerCameraTurn = true; // false�� �÷��̾��� ī�޶� �������� �ʽ��ϴ�.
	UPROPERTY(Replicated)
	bool IsJumped = false;          // ������ ��Ÿ���ϴ�.
	bool IsVault = false;           // true�� �Ѿ�� �� �ö󰡱⸦ �ϴ� �����Դϴ�.
	float PlayerSpeed = 70.0f;      // �÷��̾� �̵��ӵ��� ��Ÿ���ϴ�.(�� ���º��� ���� �޶����ϴ�.)

	bool GetIsDead() { return IsDead; }
	bool GetIsProne() { return IsProne; } 
	void SetIsProne(bool Set) { IsProne = Set; }
	void CheckPlayerHP(float hp); // �׾����� ü���� ������(ü���� 40���ϸ� ȭ���� ȸ���� �����Ѵ�.)�� �Ǵ��ϴ� �Լ��Դϴ�.
	//-------------------------------------------------------------------

	/***********************************************************************************/
	

	/*��Ÿ(���� �ٸ� ��ɺ��� ���� �� �Դϴ�.)***********************************************/
	// ��Ʈ�ѷ� ----------------------------------------------------------
	//
	FRotator GetControllerRot() { return ControllerRot; }
	float GetAimDirRight() { return AimDirRight; }
	float GetAimDirUp() { return AimDirUp; }
	float GetInputDirForward() { return InputDirForward; }
	float GetInputDirRight() { return InputDirRight; }
	EPlayerPress GetPlayerPress() { return PlayerPress; }
	bool GetIsCameraLock() { return IsCameraLock; }
	void SetIsCameraLock(bool Set) { IsCameraLock = Set; }
	//-------------------------------------------------------------------

	// �÷��̾�� ���õ� class --------------------------------------------
	//
	class UPlayerBody_AnimInstance* GetBodyAnim() { return BodyAnim; }
	class UPlayerBody_AnimInstance* GetLegAnim() { return LegAnim; }
	class UPlayerArm_AnimInstance* GetArmAnim() { return ArmAnim; }
	class AMultiPlayer_HUD* GetHUD() { return HUD; }
	//-------------------------------------------------------------------

	// ���� --------------------------------------------------------------
	//
	void FireBullet(FVector MuzzleLoc, FRotator MuzzleRot, FVector BulletLoc); // ���� ����ִ� ������ ����ϴ� �Լ��Դϴ�.
	//-------------------------------------------------------------------

	// ī�޶� -----------------------------------------------------------
	//
	float CameraPitch = 0.0f; // ��Ʈ�ѷ��� ��, ���� ������ ���� ī�޶� ��ġ�� ��, �Ϸ� �����̰� �ϱ� ���� �ʿ��� �����Դϴ�.
	//-------------------------------------------------------------------

	// ���� -------------------------------------------------------------
	//
	//UPROPERTY(Replicated)
	bool IsHandUp = false;        // ���� ��Ʈ�Ǽ� ���� �÷��� �Ǵ� ��Ȳ������ ��Ÿ���� �����Դϴ�.
	UPROPERTY(Replicated)
	bool IsCoverLeft = false;     // ���� �������� �ؾ� �Ǵ����� ��Ÿ���� �����Դϴ�.
	UPROPERTY(Replicated)
	bool IsCoverRight = false;    // ���� ���������� �ؾ� �Ǵ����� ��Ÿ���� �����Դϴ�.
	//UPROPERTY(Replicated)
	bool IsCoverUp = false;       // ���� �������� �ؾ� �Ǵ����� ��Ÿ���� �����Դϴ�. (�ɾ��� �� ������ �ٶ󺸸� ���� �ϴ°�. ���� �̿ϼ�)
	UPROPERTY(Replicated)
	float CoverAngleLeft = 0.0f;  // ���� ���󰢵��� ��Ÿ���ϴ�.
	UPROPERTY(Replicated)
	float CoverAngleRight = 0.0f; // ������ ���󰢵��� ��Ÿ���ϴ�.
	//-------------------------------------------------------------------

	// ���� ��Ģ ---------------------------------------------------------
	//
	FMultiPlayerCheckDelegate CountDownEndCheck; // ī��Ʈ �ٿ��� ������ �÷��̾ ������ �� �ְ� �ϴ� ��������Ʈ �Դϴ�.
	UPROPERTY(ReplicatedUsing = OnRep_TeamNameChange)
	FString TeamName = "";                       // �÷��̾��� ���� ��Ÿ���ϴ�.
	//-------------------------------------------------------------------

	// IK ---------------------------------------------------------------
	//
	float GetRootDistance() { return RootDistance; }
	void SetRootDistance(float Set) { RootDistance = Set; }
	//-------------------------------------------------------------------

	// �� ---------------------------------------------------------------
	//
	class AWeaponeBase* GetEquipWeapone() { return EquipWeapone; }
	class AWeaponeBase* GetEquipWeaponeArm() { return EquipWeaponeArm; }
	//-------------------------------------------------------------------

	/***********************************************************************************/


/*��Ƽ�÷��̿� ���õ� �Լ��� ///////////////////////////////////////////////////////////////////////////////////////
*/
public:
	/*��Ʈ�ѷ�(����)�� ���� �����ؾߵǴ� RPC ************************************************/
	// ��Ʈ�ѷ� Rotation���� ----------------------------------------------
	//
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void NetMulticast_SendControllerRot(FRotator rot);
	bool NetMulticast_SendControllerRot_Validate(FRotator rot);
	void NetMulticast_SendControllerRot_Implementation(FRotator rot);
	//-------------------------------------------------------------------

	// ��Ʈ�ѷ� �Է�Ű ����(���º�ȯ�� ���� ����մϴ�.) -----------------------
	//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendDownPress(EPlayerPress press); // ��ü ���� ��ȯ (������ ����)
	bool Server_SendDownPress_Validate(EPlayerPress press);
	void Server_SendDownPress_Implementation(EPlayerPress press);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendDownPress(EPlayerPress press); // ��ü ���� ��ȯ (����, �ٸ� Ŭ���̾�Ʈ�� ����)
	bool NetMulticast_SendDownPress_Validate(EPlayerPress press);
	void NetMulticast_SendDownPress_Implementation(EPlayerPress press);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendUpperPress(EPlayerPress press); // ��ü ���� ��ȯ (������ ����)
	bool Server_SendUpperPress_Validate(EPlayerPress press);
	void Server_SendUpperPress_Implementation(EPlayerPress press);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendUpperPress(EPlayerPress press); // ��ü ���� ��ȯ (����, �ٸ� Ŭ���̾�Ʈ�� ����)
	bool NetMulticast_SendUpperPress_Validate(EPlayerPress press);
	void NetMulticast_SendUpperPress_Implementation(EPlayerPress press);
	//-------------------------------------------------------------------

	// ��Ʈ�ѷ� �Է��� �ൿ ���� --------------------------------------------
	//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendReloadAmmo(int loadAmmo, int EquipAmmo); // ���� �� ������ ���� ���� ���� (������ ����)
	bool Server_SendReloadAmmo_Validate(int loadAmmo, int EquipAmmo);
	void Server_SendReloadAmmo_Implementation(int loadAmmo, int EquipAmmo);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendIsJumped(bool jumped);                   // ������ ���� ���� ���� (������ ����)
	bool Server_SendIsJumped_Validate(bool jumped);
	void Server_SendIsJumped_Implementation(bool jumped);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendValutCheck();                            // �Ѿ��, �ö󰡱⿡ ���� �ൿ ���� (������ ����)
	bool Server_SendValutCheck_Validate();
	void Server_SendValutCheck_Implementation();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendValutCheck();						 // �Ѿ��, �ö󰡱⿡ ���� �ൿ ���� (����, �ٸ� Ŭ���̾�Ʈ�� ����)
	bool NetMulticast_SendValutCheck_Validate();
	void NetMulticast_SendValutCheck_Implementation();
	//-------------------------------------------------------------------

	/***********************************************************************************/

	/*���� ���� �����ؾߵǴ� RPC ***********************************************************
	*/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendFireBullet(FVector loc); // ����� �Ͽ� �Ѿ��� �߻��ؾߵȴٰ� �˸��� �Լ��Դϴ�.(������ �Ű������� ������ �������� �߻��մϴ�.)
	bool Server_SendFireBullet_Validate(FVector loc);
	void Server_SendFireBullet_Implementation(FVector loc);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendFireBullet(FVector loc); // �ٸ� Ŭ���̾�Ʈ�� �ִ� �÷��̾��� ĳ���Ͱ� ����� �ϰ� �մϴ�.
	bool NetMulticast_SendFireBullet_Validate(FVector loc);
	void NetMulticast_SendFireBullet_Implementation(FVector loc);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMelee(FVector Loc, FVector Dir); // ��������(�ִϸ��̼�) ����, Trace�� �˻��� ������ �����մϴ�.
	bool Server_SendMelee_Validate(FVector Loc, FVector Dir);
	void Server_SendMelee_Implementation(FVector Loc, FVector Dir);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendMelee();  // �ٸ� Ŭ���̾�Ʈ�� ĳ������ �ִϸ��̼� ���� (��������)
	bool NetMulticast_SendMelee_Validate();
	void NetMulticast_SendMelee_Implementation();

	// ���� ������ ���� �÷��̾ �ٸ� Ŭ���̾�Ʈ�� ĳ���Ϳ��� ������ �����ϴ� �Լ��Դϴ�.
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendPlayerHit(float Damage, FVector Dir, FHitResult Hit, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);
	bool NetMulticast_SendPlayerHit_Validate(float Damage, FVector Dir, FHitResult Hit, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);
	void NetMulticast_SendPlayerHit_Implementation(float Damage, FVector Dir, FHitResult Hit, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);

	/***********************************************************************************/


	/*���� ���� �����ؾߵǴ� RPC ***********************************************************
	*/
	// ���� ���⸦ �������� �� �� ����ִ� ������ ������ ���� ������ �ѱ⸦ ������(� �ѱ⸦ �������� ����) ������
	// Ȥ�� �÷��̾��� ����� ���Ⱑ ����ִٸ� �������� �˻��ϴ� �Լ��Դϴ�.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendWeaponeCheck(AWeaponeBase* check); // (������ ����)
	bool Server_SendWeaponeCheck_Validate(AWeaponeBase* check);
	void Server_SendWeaponeCheck_Implementation(AWeaponeBase* check);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendWeaponeCheck(AWeaponeBase* check); // (����, �ٸ� Ŭ���̾�Ʈ�� ����)
	bool NetMulticast_SendWeaponeCheck_Validate(AWeaponeBase* check);
	void NetMulticast_SendWeaponeCheck_Implementation(AWeaponeBase* check);

	// ����� ����� �ٲٷ� �� �� �۵��Ǵ� �Լ��Դϴ�.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendWeaponeChange(EPlayerPress press); // (������ ����)
	bool Server_SendWeaponeChange_Validate(EPlayerPress press);
	void Server_SendWeaponeChange_Implementation(EPlayerPress press);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendWeaponeChange(EPlayerPress press); // (����, �ٸ� Ŭ���̾�Ʈ�� ����)
	bool NetMulticast_SendWeaponeChange_Validate(EPlayerPress press);
	void NetMulticast_SendWeaponeChange_Implementation(EPlayerPress press);

	/***********************************************************************************/
	

	/*���� �غ� ���� �����ؾߵǴ� RPC *****************************************************
	*/
	// ī��Ʈ �ٿ�(5��)�� ������ �÷��̰� ������ �� �ְ� ���ִ� �Լ��Դϴ�. 
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendPlayerStart();
	bool Server_SendPlayerStart_Validate();
	void Server_SendPlayerStart_Implementation();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendPlayerStart();
	bool NetMulticast_SendPlayerStart_Validate();
	void NetMulticast_SendPlayerStart_Implementation();

	// �÷��̾ �����ǰ� ���� ���� ��� ��ġ���� �����ؾߵ��� �����Ǵ� �Լ��Դϴ�.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPlayerStartLoc();
	bool Server_SetPlayerStartLoc_Validate();
	void Server_SetPlayerStartLoc_Implementation();

	/***********************************************************************************/

	/*���ø�����Ƽ�� ������ ���� �ߵ��Ǵ� �Լ� ************************************************
	*/
	UFUNCTION()
	void OnRep_TeamNameChange(); // TeamName�� ���� �ް� �÷��̾��� Material�� ���� ���� �°� ������ �ϴ� �Լ��Դϴ�.

	/***********************************************************************************/
};



