// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Dalos/Character/Public/CharacterBase.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "Interface/Public/ChangeState_Interface.h"
#include "MultiPlayerBase.generated.h"

DECLARE_DELEGATE(FMultiPlayerCheckDelegate)

/**********************************************************************************************************
 * 플레이어 입력표시
 * 플레이어의 상태 변환이 이루어질때 다른 클라이언트 속의 캐릭터는 컨트롤러가 복제되지 않아서
 * 입력에 대한 정보가 전달 되지 않습니다. 그래서 입력에 대한 정보를 Enum으로 만들어서 전달하기로 했습니다.
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
* 플레이어 상태표시
* 버튼을 눌렸을 때의 상태 변환 이외에 조건문에 상태를 써야 할 때, 강제 변환에 관련된 처리를 할 때
* State 클래스에 지금 현 상태 및 변환해야 될 상태를 전달해서 State 클래스 내에서 처리하게끔 하기 위해 꼭 필요합니다.
* (이게 없으면 APlayerCharacter 내 에서 State를 변환해야 하기 때문에 자치 잘 못 하면 모든 State와 연결을 해야 합니다.)
***********************************************************************************************************/

//플레이어 하체 상태 표시 하는 열거형
UENUM(BlueprintType)
enum class EPlayerDownState : uint8 {
	STANDING UMETA(DisplayName = "Standing"),
	CROUCH UMETA(DisplayName = "Crouch"),
	SPLINT UMETA(DisplayName = "Splint"),
	SLIDING UMETA(DisplayName = "Sliding"),
	PRONE UMETA(DisplayName = "Prone")
};
//플레이어 상체 상태 표시 하는 열거형
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
	
/*에디터용  ///////////////////////////////////////////////////////////////////////////////////////////////////////
*/
public:
	AMultiPlayerBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;   // 플레이어 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;   // 카메라 축
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AloowPrivateAccess = "true"))
	class USkeletalMeshComponent* ArmMesh;  // 팔 외형
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AloowPrivateAccess = "true"))
	class USkeletalMeshComponent* BodyMesh; // 몸 외형

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate = 45.0f;             // 카메라 좌, 우 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate = 45.0f;           // 카메라 상, 하 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapone)
	float BaseSpreadSize = 70.0f;           // 기본적인 탄퍼짐의 크기를 나타내는 변수

/*가려야 하지만 상속받아야하는 변수및 함수들 ///////////////////////////////////////////////////////////////////////////
*/
protected:
	/*이벤트 함수들 **********************************************************************
	*/
	// 플레이어가 스폰되고 나서 시작할 때 작동되는 함수
	// 시야및 움직임 제한, 상태 초기화, 외형 설정
	virtual void BeginPlay() override;                
	virtual void PostInitializeComponents() override; // 컴포넌트 초기화 이후에 작동되는 함수 (AnimIns 불러오기 및 딜리게이트 설정)
	virtual void Tick(float DeltaTime) override;      // 움직임, 카메라, 탄 발사, 반동, 상태별 Tick 처리, 십자선 효과등을 처리합니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; // 입력 바인딩을 하는 함수
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // 이 액터가 끝날 때 작동되는 함수

	/***********************************************************************************/


	/*플레이어와 관련있는 및 사용해야 되는 클래스들 *******************************************
	*/
	UPROPERTY()
	class UPlayerArm_AnimInstance* ArmAnim = nullptr;   // 팔 애니메이션 인스턴스
	UPROPERTY()
	class UPlayerBody_AnimInstance* BodyAnim = nullptr; // 몸 애니메이션 인스턴스
	UPROPERTY()
	class UPlayerBody_AnimInstance* LegAnim = nullptr;  // 다리 애니메이션 인스턴스
	UPROPERTY()
	class AMultiPlayer_HUD* HUD = nullptr;              // 플레이어 ui를 담당하는 hud

	/***********************************************************************************/


	/*무기 ******************************************************************************/
	// 기본 변수 ----------------------------------------------------------
	//
	UPROPERTY()
	class AWeaponeBase* EquipWeapone = nullptr;    // 현재 들고있는 무기(몸에서)
	UPROPERTY()
	class AWeaponeBase* EquipWeaponeArm = nullptr; // 현재 들고있는 무기(팔에서)
	class AWeaponeBase* BackWeapone1 = nullptr;    // 등 뒤에 있는 무기 첫 번째
	class AWeaponeBase* BackWeapone2 = nullptr;    // 등 뒤에 있는 무기 두 번째
	class AWeaponeBase* LookWeapone = nullptr;     // 처다 보고 있는 무기
	//-------------------------------------------------------------------

	// 검사및 사용해야 하는 함수들 ------------------------------------------
	//
	/*십자선에 관련된 처리를 하는 함수입니다.
	* 이 함수에서는 카메라의 ForwardVector를 기준으로 Trace를 발사하여
	* 십자선과 히트되는 Actor(벽, 적군 등)의 거리, 위치등을 구하여
	* 탄 퍼짐이 얼마나 이루어져야 하는지 적군인지를 구분하여 HUD에 표시하는 기능을 수행하고 있습니다.*/
	bool CrossHairCheck();

	/*반동에 관련된 처리를 하는함수입니다.
	* Trace를 발사해서 히트된 거리에 따라 탄퍼짐이 계산되며
	* 반동 이후에 사격이 시작된 지점으로 되돌아 오는 기능이 있습니다.*/
	bool RecoilCheck();

	bool InteractionCheck();                  // 상호작용이 가능한 Actor인지를 확인하는 함수입니다.
	void WeaponCheck(AWeaponeBase* Check);    // 현제 들고 있는 무기들을 확인해서 장착할지 버릴지 결정합니다.

	// 무기를 바꾸는 함수입니다.
	// 들고 있는 무기를 등으로 보내고 등에 있는 무기를 손으로 옮깁니다.
	void WeaponeChange(EPlayerPress Rress);
	void EquipGunOnHand(AWeaponeBase* Equip); // 현재 들고있는 무기를 매개변수로 바꾸는 함수
	void FireAutoOn();                        // 일정 시간마다 연사를 할 수 있게 타이머를 설정했고 그 타이머가 끝나면 작동되는 함수입니다.
	//-------------------------------------------------------------------

	// 사격에 사용되는 변수들 ----------------------------------------------
	//
	bool IsFire = false;     // true면 사격 중 입니다.
	bool IsFireAuto = false; // true면 연사를 해야합니다.
	FTimerHandle FireTimer;  // 연사를 할때 사용되며 탄이 일정 텀을 두며 사격이 되게 하기 위해서 사용되는 타이머입니다.
	float ThreeCount = 0.0f; // 삼점사 체크용 변수입니다.
	//-------------------------------------------------------------------

	// 반동에 사용되는 변수들 ----------------------------------------------
	//
	bool IsCrossHairRed = false;                      // 십자선을 빨간색인지 확인하는 변수
	FRotator RecoilReturnRot = FRotator::ZeroRotator; // 반동이 되돌아 오는 Rotation
	FVector RecoilReturnLoc = FVector::ZeroVector;    // 반동이 되돌아 오는 지점 위치 (기능상으로 쓰지는 않지만 Trace 때문에 있습니다.)
	FVector BulletFireLoc = FVector::ZeroVector;      // 탄퍼짐에 의해 퍼지는 총알의 최종적인 위치
	float CurrentSpreadSize = 0.0f;                   // 현재 탄퍼짐의 크기를 나타내는 변수
	//-------------------------------------------------------------------
	
	/***********************************************************************************/

	/*컨트롤러 **************************************************************************/
	FRotator ControllerRot = FRotator::ZeroRotator; // 다른 클라이언트에게 컨트롤러의 회전값을 보내기 위한 용도의 변수입니다.
	float AimDirRight = 0.0f, AimDirUp = 0.0f;      // 컨트롤러로 부터 받은 시점 키값 (AnimBP에서 사용하기 위해)
	float InputDirForward = 0.0f;                   // 컨트롤러로 부터 받은 이동(전, 후) 키값 (AnimBP에서 사용하기 위해)
	float InputDirRight = 0.0f;                     // 컨트롤러로 부터 받은 이동(좌, 우) 키값 (AnimBP에서 사용하기 위해)
	FVector MoveDir = FVector::ZeroVector;          // 움직여야 되는 방향을 나타내는 벡터입니다.
	UPROPERTY()
	EPlayerPress PlayerPress;                       // 플레이어가 어떤 버튼을 눌렀는지를 나타냅니다.
	bool IsCameraLock = false;                      // true면 카메라의 움직임을 잠금니다.
	void PlayerMove();                              // 입력에 따른 방향 값을 통해 캐릭터를 이동시키는 함수입니다.

	/***********************************************************************************/

	/*상태에 관련된 변수들과 함수 **********************************************************/
	UPROPERTY()
	class UPlayerUpperStateBase* UpperState = nullptr; // 상체 상태를 나타냅니다.
	UPROPERTY()
	class UPlayerDownStateBase* DownState = nullptr;   // 하체 상태를 나타냅니다.
	EPlayerUpperState EUpperState;					   // 상체 상태를 나타내는 enum
	EPlayerDownState EDownState;					   // 하체 상태를 나타내는 enum
	bool IsDead = false;							   // 죽은 상태를 알려주는 변수
	FTimerHandle HealTimer;							   // 일정 시간이 지나면 체력이 회복되는데 그 시간을 새는 타이머입니다.(현재 쓰고있지 않은 기능)
	bool IsProne = false;							   // 엎드린 상태를 알려줍니다.(엎드리기로 변하는중이 아닌 엎드리기가 시작했을 때 작동)
	void PlayerDead();								   // 플레이어를 죽는 상태로 만듭니다.

	/***********************************************************************************/

	/*기타(서로 다른 기능별로 나눈 것 입니다.)************************************************/
	// 올라가기 및 넘어가기 ------------------------------------------------
	//
	FVector WallLoc = FVector::ZeroVector;        // 인식된 벽의 위치를 나타내는 벡터입니다.
	FVector WallHeight = FVector::ZeroVector;     // 인식된 벽의 높이위치를 나타내는 벡터입니다.
	FVector WallBackHeight = FVector::ZeroVector; // 인식된 벽의 뒷 공간의 높이를 나타내는 백터입니다.
	FVector WallNomal = FVector::ZeroVector;      // 인식된 벽의 방향을 나타내는 벡터입니다.
	bool WallForwardTracer(FVector& Loc, FVector& Nomal);               // 플레이어의 앞에 벽이 있는지를 인식하는 Trace를 쏘는 함수입니다.
	bool WallHeightTracer(FVector Loc, FVector Nomal, FVector& Height); // 인식된 벽의 높이를 알기 위해 Trace를 쏘는 함수입니다.
	bool WallBackHeightTracer(FVector Loc, FVector& BackHeight);        // 인식된 벽의 뒷 공간의 높이를 알기 위해 Trace를 쏘는 함수입니다.
	void PlayerVault(FVector Loc, FVector Nomal, FVector Height);       // 벽의 상태에 따라 넘어가기 및 올라가기 작동
	void StopVault();         // 넘어가기를 멈춥니다.
	void StopClimb();         // 올라가기를 멈춥니다.
	bool IsWall = false;      // true면 벽이 인지됨
	bool IsWallThick = false; // true면 벽이 얇습니다.
	//-------------------------------------------------------------------

	// 데미지 ------------------------------------------------------------
	//
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* HitDecal;     // 공격 히트표시를 나타내는 파티클
	UFUNCTION()
	void HitReactionEnd();         // 히트 리액션이 끝나고 작동하는 함수(물리표현이 원래대로 돌아가는 것)
	FTimerHandle HitReactionTimer; // 히트 리액션이 끝나는 시간을 재는 타이머입니다.
	bool IsHitReactionEnd = false; // 히트 리액션이 끝나는지를 표현하는 변수입니다.
	float HitReactionAlpha = 0.0f; // 물리표현의 정도로를 나타내는 값입니다.
	void StratAutoHeal(); // 자동 체력 회복을 시작하는 함수입니다.(현재는 카메라 복구만 가능하고 힐을 꺼둔 상태입니다.)

	//-------------------------------------------------------------------

	// 사운드 ------------------------------------------------------------
	//
	class USoundBase* BulletHitHitSound; // 탄약에 히트 될때를 재생되는 사운드입니다.
	class USoundBase* MeleeHitHitSound;  // 근접 공격에 히트 될때를 재생되는 사운드입니다.
	//-------------------------------------------------------------------

	// 피부 --------------------------------------------------------------
	//
	UMaterialInterface* BlueMat; // 블루팀 스킨입니다.
	UMaterialInterface* RedMat;  // 레드팀 스킨입니다.
	//-------------------------------------------------------------------

	// 카메라 위치 --------------------------------------------------------
	//
	float RootDistance = 0.0f; // FootIK에 따라 달라지는 루트본의 z값을 뜻한다.
	//-------------------------------------------------------------------


	/***********************************************************************************/


/*입력에 관련된 함수들 입니다. //////////////////////////////////////////////////////////////////////////////////////
*/
protected:
	// 입력에 관련된 함수들입니다. **********************************************************
	void PlayerCrouch();      // 앉기(C) 키와 바인딩 함수
	void PlayerJump();		  // 점프(Space) 키와 바인딩 함수
	void PlayerSplint();	  // 달리기(Shift) 키와 바인딩 함수
	void PlayerFirstGun();    // 1번 무기 선택(1) 키와 바인딩 함수
	void PlayerSecondGun();   // 2번 무기 선택(2) 키와 바인딩 함수
	void PlayerUnArmed();     // 비무장(3) 키와 바인딩 함수
	void PlayerInteraction(); // 상호작용(F) 키와 바인딩 함수
	void PlayerADS();		  // 정밀조준(마우스 오른쪽키) 키를 누를 때 바인딩 함수
	void PlayerUnADS();		  // 정밀조준(마우스 오른쪽키) 키를 땔 때 바인딩 함수
	void PlayerFire();		  // 사격(마우스 왼쪽키) 키를 누를 때 바인딩 함수
	void PlayerUnFire();	  // 사격(마우스 왼쪽키) 키를 땔 때 바인딩 함수
	void PlayerReload();	  // 장전(R) 키와 바인딩 함수
	void PlayerProne();		  // 엎드리기(Z) 키와 바인딩 함수
	void PlayerSelfDead();	  // 테스트용
	void PlayerWeaponLever();	  // 테스트용

	/***********************************************************************************/


	// Axis에 관련된 함수들입니다.(이동, 시점등) ********************************************
	void MoveForward(float Value); // 전, 후 이동키(W, S) 바인딩 함수
	void MoveRight(float Value);   // 좌, 우 이동키(A, D) 바인딩 함수
	void LookUpAtRate(float Rate); // 상, 하 시점키(마우스 이동) 바인딩 함수
	void TurnAtRate(float Rate);   // 좌, 우 시점키(마우스 이동) 바인딩 함수

	/***********************************************************************************/


/*막혀있는 변수들 공개하는 함수 or 공개용 변수들 //////////////////////////////////////////////////////////////////////
*/
public:
	// 데미지를 받으면 어떤 반응을 해야하는지를 나타내는 함수입니다.
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	/*상태에 관련된 변수들과 함수 **********************************************************/
	// 개발자가 정한 상태들 ------------------------------------------------
	//
	UPlayerUpperStateBase* GetUpperState() { return UpperState; };
	UPlayerDownStateBase* GetDownState() { return DownState; };
	UClass* UpperStateBClass = nullptr; UClass* DownStateBClass = nullptr;   // 전 상태에 대한 Class값 (AnimBP에서 사용하기 위해)
	UClass* UpperStateNClass = nullptr; UClass* DownStateNClass = nullptr;   // 전 상태에 대한 Class값 (AnimBP에서 사용하기 위해)
	void UpperPress(class UPlayerUpperStateBase* state);					 // 상체와 관련된 키가 눌렀을 때 상태가 변하는 함수입니다.
	void DownPress(class UPlayerDownStateBase* state);						 // 하체와 관련된 키가 눌렀을 때 상태가 변하는 함수입니다.
	EPlayerUpperState GetEUpperState() { return EUpperState; }
	EPlayerDownState GetEDownState() { return EDownState; }

	/*상태끼리 변하고 있는 상황에서 변하는게 끝나면
	* (애니메이션이 끝나면 AnimNotify를 해서 인터페이스로 플레이어에게 변하는 애니메이션이 끝났으니 StateStart를 하라고 알려줍니다.)
	* StateStart를 작동하는 함수입니다.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "State")
	void CurrentStateStart(bool IsUpper);
	virtual void CurrentStateStart_Implementation(bool IsUpper) override;
	//-------------------------------------------------------------------

	// 정해놓은 상태들과 별개로 공통으로 다루어야 하는 상태들 -------------------
	//
	bool IsMove = true;             // false면 플레이어가 움직일 수 없습니다.
	bool IsPlayerCameraTurn = true; // false면 플레이어의 카메라가 움직이지 않습니다.
	UPROPERTY(Replicated)
	bool IsJumped = false;          // 점프를 나타냅니다.
	bool IsVault = false;           // true면 넘어가기 및 올라가기를 하는 상태입니다.
	float PlayerSpeed = 70.0f;      // 플레이어 이동속도를 나타냅니다.(각 상태별로 값이 달라집니다.)

	bool GetIsDead() { return IsDead; }
	bool GetIsProne() { return IsProne; } 
	void SetIsProne(bool Set) { IsProne = Set; }
	void CheckPlayerHP(float hp); // 죽었는지 체력이 낮은지(체력이 40이하면 화면을 회색을 띄어야한다.)를 판단하는 함수입니다.
	//-------------------------------------------------------------------

	/***********************************************************************************/
	

	/*기타(서로 다른 기능별로 나눈 것 입니다.)***********************************************/
	// 컨트롤러 ----------------------------------------------------------
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

	// 플레이어와 관련된 class --------------------------------------------
	//
	class UPlayerBody_AnimInstance* GetBodyAnim() { return BodyAnim; }
	class UPlayerBody_AnimInstance* GetLegAnim() { return LegAnim; }
	class UPlayerArm_AnimInstance* GetArmAnim() { return ArmAnim; }
	class AMultiPlayer_HUD* GetHUD() { return HUD; }
	//-------------------------------------------------------------------

	// 무기 --------------------------------------------------------------
	//
	void FireBullet(FVector MuzzleLoc, FRotator MuzzleRot, FVector BulletLoc); // 현재 들고있는 총으로 사격하는 함수입니다.
	//-------------------------------------------------------------------

	// 카메라 -----------------------------------------------------------
	//
	float CameraPitch = 0.0f; // 컨트롤러의 상, 하의 시점에 따라 카메라 위치가 상, 하로 움직이게 하기 위해 필요한 변수입니다.
	//-------------------------------------------------------------------

	// 엄폐 -------------------------------------------------------------
	//
	//UPROPERTY(Replicated)
	bool IsHandUp = false;        // 벽에 히트되서 손을 올려야 되는 상황인지를 나타내는 변수입니다.
	UPROPERTY(Replicated)
	bool IsCoverLeft = false;     // 엄폐를 왼쪽으로 해야 되는지를 나타내는 변수입니다.
	UPROPERTY(Replicated)
	bool IsCoverRight = false;    // 엄폐를 오른쪽으로 해야 되는지를 나타내는 변수입니다.
	//UPROPERTY(Replicated)
	bool IsCoverUp = false;       // 엄폐를 위쪽으로 해야 되는지를 나타내는 변수입니다. (앉았을 때 위쪽을 바라보며 엄폐를 하는것. 아직 미완성)
	UPROPERTY(Replicated)
	float CoverAngleLeft = 0.0f;  // 왼쪽 엄폐각도를 나타냅니다.
	UPROPERTY(Replicated)
	float CoverAngleRight = 0.0f; // 오른쪽 엄폐각도를 나타냅니다.
	//-------------------------------------------------------------------

	// 게임 규칙 ---------------------------------------------------------
	//
	FMultiPlayerCheckDelegate CountDownEndCheck; // 카운트 다운이 끝나면 플레이어가 움직일 수 있게 하는 딜리게이트 입니다.
	UPROPERTY(ReplicatedUsing = OnRep_TeamNameChange)
	FString TeamName = "";                       // 플레이어의 팀을 나타냅니다.
	//-------------------------------------------------------------------

	// IK ---------------------------------------------------------------
	//
	float GetRootDistance() { return RootDistance; }
	void SetRootDistance(float Set) { RootDistance = Set; }
	//-------------------------------------------------------------------

	// 총 ---------------------------------------------------------------
	//
	class AWeaponeBase* GetEquipWeapone() { return EquipWeapone; }
	class AWeaponeBase* GetEquipWeaponeArm() { return EquipWeaponeArm; }
	//-------------------------------------------------------------------

	/***********************************************************************************/


/*멀티플레이에 관련된 함수들 ///////////////////////////////////////////////////////////////////////////////////////
*/
public:
	/*컨트롤러(조작)에 대해 전달해야되는 RPC ************************************************/
	// 컨트롤러 Rotation전달 ----------------------------------------------
	//
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void NetMulticast_SendControllerRot(FRotator rot);
	bool NetMulticast_SendControllerRot_Validate(FRotator rot);
	void NetMulticast_SendControllerRot_Implementation(FRotator rot);
	//-------------------------------------------------------------------

	// 컨트롤러 입력키 전달(상태변환을 위해 사용합니다.) -----------------------
	//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendDownPress(EPlayerPress press); // 하체 상태 변환 (서버에 전달)
	bool Server_SendDownPress_Validate(EPlayerPress press);
	void Server_SendDownPress_Implementation(EPlayerPress press);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendDownPress(EPlayerPress press); // 하체 상태 변환 (서버, 다른 클라이언트에 전달)
	bool NetMulticast_SendDownPress_Validate(EPlayerPress press);
	void NetMulticast_SendDownPress_Implementation(EPlayerPress press);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendUpperPress(EPlayerPress press); // 상체 상태 변환 (서버에 전달)
	bool Server_SendUpperPress_Validate(EPlayerPress press);
	void Server_SendUpperPress_Implementation(EPlayerPress press);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendUpperPress(EPlayerPress press); // 상체 상태 변환 (서버, 다른 클라이언트에 전달)
	bool NetMulticast_SendUpperPress_Validate(EPlayerPress press);
	void NetMulticast_SendUpperPress_Implementation(EPlayerPress press);
	//-------------------------------------------------------------------

	// 컨트롤러 입력후 행동 전달 --------------------------------------------
	//
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendReloadAmmo(int loadAmmo, int EquipAmmo); // 장전 후 장전에 대한 정보 전달 (서버에 전달)
	bool Server_SendReloadAmmo_Validate(int loadAmmo, int EquipAmmo);
	void Server_SendReloadAmmo_Implementation(int loadAmmo, int EquipAmmo);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendIsJumped(bool jumped);                   // 점프에 대한 정보 전달 (서버에 전달)
	bool Server_SendIsJumped_Validate(bool jumped);
	void Server_SendIsJumped_Implementation(bool jumped);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendValutCheck();                            // 넘어가기, 올라가기에 대한 행동 전달 (서버에 전달)
	bool Server_SendValutCheck_Validate();
	void Server_SendValutCheck_Implementation();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendValutCheck();						 // 넘어가기, 올라가기에 대한 행동 전달 (서버, 다른 클라이언트에 전달)
	bool NetMulticast_SendValutCheck_Validate();
	void NetMulticast_SendValutCheck_Implementation();
	//-------------------------------------------------------------------

	/***********************************************************************************/

	/*공격 대해 전달해야되는 RPC ***********************************************************
	*/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendFireBullet(FVector loc); // 사격을 하여 총알이 발사해야된다고 알리는 함수입니다.(방향을 매개변수로 보내어 서버에서 발사합니다.)
	bool Server_SendFireBullet_Validate(FVector loc);
	void Server_SendFireBullet_Implementation(FVector loc);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendFireBullet(FVector loc); // 다른 클라이언트에 있는 플레이어의 캐릭터가 사격을 하게 합니다.
	bool NetMulticast_SendFireBullet_Validate(FVector loc);
	void NetMulticast_SendFireBullet_Implementation(FVector loc);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMelee(FVector Loc, FVector Dir); // 근접공격(애니메이션) 실행, Trace로 검사후 데미지 전달합니다.
	bool Server_SendMelee_Validate(FVector Loc, FVector Dir);
	void Server_SendMelee_Implementation(FVector Loc, FVector Dir);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendMelee();  // 다른 클라이언트의 캐릭터의 애니메이션 실행 (근접공격)
	bool NetMulticast_SendMelee_Validate();
	void NetMulticast_SendMelee_Implementation();

	// 근접 공격을 맞은 플레이어가 다른 클라이언트의 캐릭터에게 데미지 전달하는 함수입니다.
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendPlayerHit(float Damage, FVector Dir, FHitResult Hit, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);
	bool NetMulticast_SendPlayerHit_Validate(float Damage, FVector Dir, FHitResult Hit, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);
	void NetMulticast_SendPlayerHit_Implementation(float Damage, FVector Dir, FHitResult Hit, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType);

	/***********************************************************************************/


	/*무기 대해 전달해야되는 RPC ***********************************************************
	*/
	// 현재 무기를 주으려고 할 때 들고있는 무기의 갯수에 따라 기존의 총기를 버리고(어떤 총기를 버릴지도 결정) 주을지
	// 혹은 플레이어의 등뒤의 무기가 비어있다면 주을지를 검사하는 함수입니다.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendWeaponeCheck(AWeaponeBase* check); // (서버에 전달)
	bool Server_SendWeaponeCheck_Validate(AWeaponeBase* check);
	void Server_SendWeaponeCheck_Implementation(AWeaponeBase* check);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendWeaponeCheck(AWeaponeBase* check); // (서버, 다른 클라이언트에 전달)
	bool NetMulticast_SendWeaponeCheck_Validate(AWeaponeBase* check);
	void NetMulticast_SendWeaponeCheck_Implementation(AWeaponeBase* check);

	// 등뒤의 무기와 바꾸려 할 때 작동되는 함수입니다.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendWeaponeChange(EPlayerPress press); // (서버에 전달)
	bool Server_SendWeaponeChange_Validate(EPlayerPress press);
	void Server_SendWeaponeChange_Implementation(EPlayerPress press);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendWeaponeChange(EPlayerPress press); // (서버, 다른 클라이언트에 전달)
	bool NetMulticast_SendWeaponeChange_Validate(EPlayerPress press);
	void NetMulticast_SendWeaponeChange_Implementation(EPlayerPress press);

	/***********************************************************************************/
	

	/*게임 준비에 대해 전달해야되는 RPC *****************************************************
	*/
	// 카운트 다운(5초)이 끝나고 플레이가 움직일 수 있게 해주는 함수입니다. 
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendPlayerStart();
	bool Server_SendPlayerStart_Validate();
	void Server_SendPlayerStart_Implementation();
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void NetMulticast_SendPlayerStart();
	bool NetMulticast_SendPlayerStart_Validate();
	void NetMulticast_SendPlayerStart_Implementation();

	// 플레이어가 스폰되고 팀에 따라 어느 위치에서 시작해야될지 결정되는 함수입니다.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPlayerStartLoc();
	bool Server_SetPlayerStartLoc_Validate();
	void Server_SetPlayerStartLoc_Implementation();

	/***********************************************************************************/

	/*리플리게이티드 변수와 같이 발동되는 함수 ************************************************
	*/
	UFUNCTION()
	void OnRep_TeamNameChange(); // TeamName을 전달 받고 플레이어의 Material을 팀의 색과 맞게 변경을 하는 함수입니다.

	/***********************************************************************************/
};



