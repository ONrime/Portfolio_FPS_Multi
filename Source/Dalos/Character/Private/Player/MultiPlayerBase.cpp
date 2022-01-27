// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Character/Public/Player/MultiPlayerBase.h"
#include "Dalos/Weapone/Public/WeaponeBase.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/PlayerUpperStateBase.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/PlayerDownStateBase.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerUpper/UnArmed_PlayerUpper.h"
#include "Dalos/Character/Public/Player/PlayerState/PlayerDown/Standing_PlayerDown.h"
#include "Dalos/Character/Public/Player/PlayerArm_AnimInstance.h"
#include "Dalos/Character/Public/Player/PlayerBody_AnimInstance.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_GameState.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerState.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_GameMode.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerController.h"
#include "Dalos/Stage/TwoVersus/Public/TwoVersus_PlayerStart.h"
#include "Dalos/Game/Public/GameInfo_Instance.h"
#include "Dalos/Widget/Public/MultiPlayer_HUD.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/BlendableInterface.h"
#include "Materials/MaterialInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Damage/Public/Melee_DamageType.h"


AMultiPlayerBase::AMultiPlayerBase()
{
	SetReplicates(true);
	SetReplicateMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = true;

	// 충돌체(캡슐)
	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->InitCapsuleSize(50.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");

	// Mesh(다리)
	GetMesh()->SetupAttachment(RootComponent);
	GetMesh()->SetIsReplicated(true);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -97.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName("CharacterMesh");

	// Mesh(몸)
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BODY"));
	BodyMesh->SetGenerateOverlapEvents(true); // Overlap으로 충돌할 때 반응하게 합니다.
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -97.0f));
	BodyMesh->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	BodyMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	BodyMesh->SetCollisionProfileName("EBodyMesh");

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>FULLBODY_SKELETALMESH(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (FULLBODY_SKELETALMESH.Succeeded()) 
	{ // Mesh 설정
		GetMesh()->SetSkeletalMesh(FULLBODY_SKELETALMESH.Object);
		BodyMesh->SetSkeletalMesh(FULLBODY_SKELETALMESH.Object);
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance>FULLBODY_ANIMBP(TEXT("AnimBlueprint'/Game/Player/Anim/PlayerBodyAnimBP.PlayerBodyAnimBP_C'"));
	if (FULLBODY_ANIMBP.Succeeded()) 
	{ // AnimIns 설정
		GetMesh()->SetAnimInstanceClass(FULLBODY_ANIMBP.Class);
		BodyMesh->SetAnimInstanceClass(FULLBODY_ANIMBP.Class);
	}
	

	// SpringArm(카메라 다리)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(25.0f, 0.0f, 65.0f));
	SpringArm->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
	SpringArm->bDoCollisionTest = true;
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bUsePawnControlRotation = true; // 컨트롤러에 따라 회전 
	SpringArm->bInheritPitch = true; // 컨트롤러에 따라 Y축 회전 
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = true;   // 컨트롤러에 따라 Z축 회전  


	// 카메라
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(SpringArm);
	FollowCamera->SetIsReplicated(true);
	FollowCamera->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	FollowCamera->bUsePawnControlRotation = false; // 컨트롤러에 따라 회전 X
	FollowCamera->PostProcessBlendWeight = 0.0f;


	// Mesh(팔)
	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	ArmMesh->SetupAttachment(FollowCamera);
	ArmMesh->SetCollisionProfileName("CharacterMesh");
	ArmMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -155.712738));
	ArmMesh->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	ArmMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>ARM_SKELETALMESH(TEXT("SkeletalMesh'/Game/FirstPerson/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));
	if (ARM_SKELETALMESH.Succeeded()) 
	{ // Mesh 설정
		ArmMesh->SetSkeletalMesh(ARM_SKELETALMESH.Object); 
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance>ARM_ANIMBP(TEXT("AnimBlueprint'/Game/Player/Anim/PlayerArmAnimBP.PlayerArmAnimBP_C'"));
	if (ARM_ANIMBP.Succeeded()) 
	{ // AnimIns 설정
		ArmMesh->SetAnimInstanceClass(ARM_ANIMBP.Class);
	}


	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // 이동방향에 맞춰서 회전을 보간해준다.(회전은 내가 바꾸기 때문에 false)	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); // 회전 속도 0.0으로 초기화 (왜 했지?)
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;  // 컨트롤러 방향으로 캐릭터 회전(무브먼트 기준)
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	Cast<UCharacterMovementComponent>(GetMovementComponent())->NetworkSmoothingMode = ENetworkSmoothingMode::Linear;


	// 파티클
	// 플레이어가 공격을 당했을 때 효과
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BULLETPARTICLE(TEXT("ParticleSystem'/Game/BallisticsVFX/Particles/Destruction/Fractures/Legacy/Metal_fracture.Metal_fracture'"));
	if (BULLETPARTICLE.Succeeded()) {
		HitDecal = BULLETPARTICLE.Object;
	}


	// 사운드
	// 카메라 체력 낮을 때의 회색 화면
	static ConstructorHelpers::FObjectFinder<USoundBase>BULLETHIT_SOUND(TEXT("SoundCue'/Game/BallisticsVFX/SFX/Impacts/MetalDull/MetalHit.MetalHit'"));
	if (BULLETHIT_SOUND.Succeeded()) {
		BulletHitHitSound = BULLETHIT_SOUND.Object;
	}

	// 카메라 체력 낮을 때의 회색 화면
	static ConstructorHelpers::FObjectFinder<USoundBase>MELEEHIT_SOUND(TEXT("SoundCue'/Game/BallisticsVFX/SFX/Impacts/Crack/HitCue.HitCue'"));
	if (MELEEHIT_SOUND.Succeeded()) {
		MeleeHitHitSound = MELEEHIT_SOUND.Object;
	}


	// Material
	// 카메라 체력 낮을 때의 회색 화면
	static ConstructorHelpers::FObjectFinder<UObject>GRAY_MATERIAL(TEXT("Material'/Game/Player/Grey_Camera.Grey_Camera'"));
	if (GRAY_MATERIAL.Succeeded())
	{
		FollowCamera->AddOrUpdateBlendable(GRAY_MATERIAL.Object, 1.0f);
	}

	// 블루팀 플레이어 색
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>BLUETEAM_MATERIAL(TEXT("Material'/Game/Player/Team/Character_BlueTeam_Mat.Character_BlueTeam_Mat'"));
	if (BLUETEAM_MATERIAL.Succeeded()) 
	{
		BlueMat = BLUETEAM_MATERIAL.Object;
	}

	// 레드팀 플레이어 색
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>REDTEAM_MATERIAL(TEXT("Material'/Game/Player/Team/Character_RedTeam_Mat.Character_RedTeam_Mat'"));
	if (REDTEAM_MATERIAL.Succeeded()) 
	{
		RedMat = REDTEAM_MATERIAL.Object;
	}
	
}

// 플레이어가 스폰되고 나서 시작할 때 작동되는 함수
// 시야및 움직임 제한, 상태 초기화, 외형 설정
void AMultiPlayerBase::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어가 스폰되고 서버에 시작 위치를 정하라고 알립니다.
	if(IsLocallyControlled()) Server_SetPlayerStartLoc();

	IsPlayerCameraTurn = false; // 카메라를 움직이지 못 하게 합니다. (카운트 다운이 끝나면 움직입니다.)

	HUD = Cast<AMultiPlayer_HUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	// 플레이어, 다른 플레이어에게 보이거나 가려야 하는 부분을 정합니다.
	GetMesh()->HideBoneByName(FName("spine_02"), PBO_None);
	GetMesh()->SetOnlyOwnerSee(true); // 오직 플레이어에게만 보이게 합니다. (다리)
	GetMesh()->SetCastShadow(false);  // 모두에게 그림자가 보이지 않게 합니다. (다리)
	ArmMesh->SetOnlyOwnerSee(true);   // 오직 플레이어에게만 보이게 합니다. (팔)
	ArmMesh->SetCastShadow(false);    // 모두에게 그림자가 보이지 않게 합니다. (팔)
	//ArmMesh->bCastHiddenShadow = true;

	// 컨트롤러가 존재하지 않다면 공격을 허용되는 충돌체가 되게 합니다.
	if(!IsLocallyControlled()) BodyMesh->SetCollisionProfileName("EBodyMesh");
	BodyMesh->SetOwnerNoSee(true);      // 오직 플레이어에게만 보이지 않게 합니다. (몸)		
	BodyMesh->bCastHiddenShadow = true; // 모두에게 그림자가 보이게 합니다. (몸)

	// 상태 초기화
	UpperState = NewObject<UUnArmed_PlayerUpper>(this, UUnArmed_PlayerUpper::StaticClass()); // 비 무장 상태로 초기화
	DownState = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass());  // 서있기 상태로 초기화

	// 각 상태별로 시작하기
	UpperState->StateStart(this);
	DownState->StateStart(this);
	UpperStateNClass = UpperState->GetState();
	DownStateNClass = DownState->GetState();

	IsMove = false;				// 움직이지 못 하게 합니다. (카운트 다운이 끝나면 움직입니다.)

	CharacterHp = 100.0f; // 사용하지 않습니다.

	// 팀에 따른 색 분배
	if (TeamName == "Blue")
	{
		ArmMesh->SetMaterial(0.0f, BlueMat);
		BodyMesh->SetMaterial(0.0f, BlueMat);
		GetMesh()->SetMaterial(0.0f, BlueMat);
	}
	else
	{
		ArmMesh->SetMaterial(0.0f, RedMat);
		BodyMesh->SetMaterial(0.0f, RedMat);
		GetMesh()->SetMaterial(0.0f, RedMat);
	}
	UE_LOG(LogTemp, Warning, TEXT("BeginePlay"));

}

// 컴포넌트 초기화 이후에 작동되는 함수 (AnimIns 불러오기 및 딜리게이트 설정)
void AMultiPlayerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// AnimationInstance 불러오기
	BodyAnim = Cast<UPlayerBody_AnimInstance>(BodyMesh->GetAnimInstance());
	ArmAnim = Cast<UPlayerArm_AnimInstance>(ArmMesh->GetAnimInstance());
	LegAnim = Cast<UPlayerBody_AnimInstance>(GetMesh()->GetAnimInstance());

	// 딜리게이트에 함수를 바인드 합니다.
	LegAnim->VaultDelegate.BindLambda([this]()->void 
	{ // 넘어가기 멈추게하기
		StopVault();
		});
	BodyAnim->VaultDelegate.BindLambda([this]()->void 
	{ // 넘어가기 멈추게하기
		StopVault();
		});
	LegAnim->ClimbDelegate.BindLambda([this]()->void 
	{ // 올라가기 멈추게하기
		StopClimb();
		});
	BodyAnim->ClimbDelegate.BindLambda([this]()->void 
	{ // 올라가기 멈추게하기
		StopClimb();
		});
	CountDownEndCheck.BindLambda([this]()->void 
	{ // 첫 카운트다운이 끝나면 플레이어를 움직이게 합니다.
		Server_SendPlayerStart();
		});
}

// 움직임, 카메라, 탄 발사, 반동, 상태별 Tick 처리, 십자선 효과등을 처리합니다.
void AMultiPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/*공통된 동작(서버와 클라이언트 둘다 작동)***************************************************************/

	// 플레이어 움직임
	PlayerMove();

	// 상태별 Update작동
	DownState->StateUpdate(this);
	UpperState->StateUpdate(this);

	// 특정 상태로 변경할때 //
	// 슬라이딩에서 앉기 상태로 변해야 할때
	if (DownState->GetEState() == EPlayerDownState::SLIDING && GetVelocity().Size() < 80.0f)
	{
		DownPress(DownState->ChangeState(EPlayerDownState::CROUCH));
	}
	// 뛰는 상태에서 속도가 줄면 서있기 상태로 변해야 할때
	if (GetVelocity().Size() < 0.3f && DownState->GetEState() == EPlayerDownState::SPLINT)
	{
		DownPress(DownState->ChangeState(EPlayerDownState::STANDING));
	}

	// 점프와 Fall을 구분 짖기 위해 넣은 것 입니다.
	if (IsJumped && GetMovementComponent()->IsFalling() == false) IsJumped = false;

	// 히트 리액션
	if (IsHitReactionEnd && HitReactionAlpha != 0.0f)
	{
		HitReactionAlpha = FMath::FInterpTo(HitReactionAlpha, 0.0f, DeltaTime, 8.0f);
		BodyMesh->SetAllBodiesBelowPhysicsBlendWeight("spine_01", HitReactionAlpha, false, true);
	}
	if (IsHitReactionEnd && HitReactionAlpha == 0.0f && !IsDead)
	{
		IsHitReactionEnd = false;
		BodyMesh->SetAllBodiesPhysicsBlendWeight(0.0f, false); // Timeline으로 바꿔서 이거 Finish에 넣기 ?
		BodyMesh->SetAllBodiesSimulatePhysics(false);
		//UE_LOG(LogTemp, Warning, TEXT("IsHitReactionEnd: End"));
	}

	// 탄 발사
	if (IsFire && IsFireAuto) {
		FireBullet(EquipWeaponeArm->BodyMesh->GetSocketLocation("Muzzle"), FollowCamera->GetComponentRotation(), BulletFireLoc);

		//if (ThreeCount != 0) ThreeCount--;

		if (HUD && EquipWeapone) {
			float spread = EquipWeapone->GetFireStartSpreadSize();
			if (HUD->GetTargetSpread() <= spread) 
			{
				HUD->SetCrossHairSpread(HUD->GetTargetSpread() + 80.0f, false, IsFire);
			}
			else 
			{
				HUD->SetCrossHairSpread(spread + 30.0f, false, IsFire);
			}
			if (CurrentSpreadSize > 10.0f) 
			{
				CurrentSpreadSize = CurrentSpreadSize - 30.0f;
			}
		}
	}

	/**************************************************************************************************/

	/*컨트롤러가 존재할때만 작동***************************************************************************/
	if (IsLocallyControlled()) {

		// 카메라 위치 조정 (각 상태마다 혹은 엄폐, FootIK와 같은 특수한 상황마다 카메라위치가 조정되는 부분입니다.)
		FVector CameraLoc = GetTransform().InverseTransformPosition(GetMesh()->GetSocketLocation("CameraLoc"));
		if (!GetIsCameraLock()) 
		{
			if (DownState->GetEState() == EPlayerDownState::SLIDING) RootDistance = 0.0f;
			SpringArm->SetRelativeLocation(FVector(CameraLoc.X, CameraLoc.Y
				, FMath::FInterpTo(SpringArm->GetRelativeLocation().Z
					, DownState->GetPelvisLocZ() + GetRootDistance() + CameraPitch + UpperState->GetCoverLocZ()
					, GetWorld()->GetDeltaSeconds(), 10.0f - DownState->GetChangeSpeed())));
		}
		else 
		{
			/*SpringArm->SetRelativeLocation(FVector(CameraLoc.X, CameraLoc.Y
				, FMath::FInterpTo(SpringArm->GetRelativeLocation().Z, CameraLoc.Z
					, GetWorld()->GetDeltaSeconds(), 10.0f - DownState->GetChangeSpeed())));*/
			SpringArm->SetRelativeLocation(FVector::ZeroVector);
		}

		// 십자선 기능
		CrossHairCheck();
		if (HUD && EquipWeapone && !IsFire) 
		{
			HUD->SetCrossHairSpread(FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 330.0f), FVector2D(0.0f, EquipWeapone->GetWalkSpreadSize()), GetVelocity().Size()), false, IsFire);
		}

		// 반동 기능
		RecoilCheck();

		// 상호작용 기능 (상호작용 가능한지 검사합니다.)
		InteractionCheck();

		// 체력이 낮을 때 카메라 화면 어둡게 하는 기능
		ATwoVersus_PlayerState* playerstate = Cast<ATwoVersus_PlayerState>(GetPlayerState());
		if (playerstate && playerstate->GetPlyaerHP() > 40.0f) 
		{
			FollowCamera->PostProcessBlendWeight = 0.0f;
		}
	}
	/**************************************************************************************************/

	/*서버와 클라이언트***********************************************************************************/ 
	if (HasAuthority()) 
	{ // 서버에서만 작동 /////////////////////////////////////////////////////////////////////////////////
		if (IsLocallyControlled()) {
		}
		else {
		}
		ControllerRot = GetControlRotation();
		NetMulticast_SendControllerRot(ControllerRot);
		//UE_LOG(LogTemp, Warning, TEXT("server_Updates: %s ControllerRot %f"), *GetName(), ControllerRot.Pitch);
		//UE_LOG(LogTemp, Warning, TEXT("server_Updates: %s CoverAngleRight %f"), *GetName(), CoverAngleRight);
		//UE_LOG(LogTemp, Warning, TEXT("server_Updates: %s SocketLocX %f"), *GetName(), BodyMesh->GetSocketLocation("VB foot_l_foot_l").X);
		//UE_LOG(LogTemp, Warning, TEXT("server_Updates: %s SocketLocY %f"), *GetName(), BodyMesh->GetSocketLocation("VB foot_l_foot_l").Y);
		
	}
	else 
	{ // 클라이언트에서만 작동  ///////////////////////////////////////////////////////////////////////////
		if (IsLocallyControlled()) {
		}
		else {
		}
		//UE_LOG(LogTemp, Warning, TEXT("cla_Updates: %s CoverAngleRight %f"), *GetName(), CoverAngleRight);
		//UE_LOG(LogTemp, Warning, TEXT("cla_Updates: %s SocketLocX %f"), *GetName(), BodyMesh->GetSocketLocation("VB foot_l_foot_l").X);
		//UE_LOG(LogTemp, Warning, TEXT("cla_Updates: %s SocketLocY %f"), *GetName(), BodyMesh->GetSocketLocation("VB foot_l_foot_l").Y);
	}
	/**************************************************************************************************/
}

// 입력 바인딩을 하는 함수
void AMultiPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerJump);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerCrouch);
	PlayerInputComponent->BindAction("Splint", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerSplint);
	PlayerInputComponent->BindAction("Prone", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerProne);
	PlayerInputComponent->BindAction("Dead", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerSelfDead);

	PlayerInputComponent->BindAction("FirstGun", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerFirstGun);
	PlayerInputComponent->BindAction("SecondGun", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerSecondGun);
	PlayerInputComponent->BindAction("UnArmed", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerUnArmed);
	PlayerInputComponent->BindAction("Interaction", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerInteraction);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerReload);
	PlayerInputComponent->BindAction("WeaponLever", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerWeaponLever);
	PlayerInputComponent->BindAction("ADS", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerADS);
	PlayerInputComponent->BindAction("ADS", EInputEvent::IE_Released, this, &AMultiPlayerBase::PlayerUnADS);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AMultiPlayerBase::PlayerFire);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AMultiPlayerBase::PlayerUnFire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiPlayerBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiPlayerBase::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMultiPlayerBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMultiPlayerBase::LookUpAtRate);
}

// 이 액터가 끝날 때 작동되는 함수
void AMultiPlayerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

/*십자선에 관련된 처리를 하는 함수입니다.
* 이 함수에서는 카메라의 ForwardVector를 기준으로 Trace를 발사하여
* 십자선과 히트되는 Actor(벽, 적군 등)의 거리, 위치등을 구하여
* 탄 퍼짐이 얼마나 이루어져야 하는지 적군인지를 구분하여 HUD에 표시하는 기능을 수행하고 있습니다.
*
* ***나중에는 투영행렬을 이용해보자***
*
*/
bool AMultiPlayerBase::CrossHairCheck()
{
	FHitResult OutHit;                             // Trace에 히트된 결과물을 담는 구조체입니다.
	TArray<AActor*> ActorsToIgnore;
	FVector StartTracer = FollowCamera->GetComponentLocation();                   // Trace 시작지점
	FVector EndTracer = StartTracer + FollowCamera->GetForwardVector() * 2500.0f; // Trace 끝나는 지점
	FVector MuzzleLoc = FVector::ZeroVector;       // 총구의 위치
	FVector BulletEndLoc = FVector::ZeroVector;    // 탄퍼짐에 의해 결정되는 최종적인 총알의 위치
	FVector CrossHairEndLoc = FVector::ZeroVector; // 십자선에 히트된 위치
	float Distance = 0.0f;                         // 십자선에 히트된 위치와 플레이어간의 거리

	// Trace
	bool hitis = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartTracer, EndTracer, ETraceTypeQuery::TraceTypeQuery4, false
		, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	// 총구의 위치를 찾습니다.
	if (EquipWeapone != nullptr)
	{
		MuzzleLoc = EquipWeapone->BodyMesh->GetSocketLocation("Muzzle");
	}
	else { MuzzleLoc = FollowCamera->GetComponentLocation(); }


	bool RedCheck = false;
	if (hitis && OutHit.GetActor() != this)
	{ // 히트된 엑터가 자기자신이 아니면
		CrossHairEndLoc = OutHit.Location; // 십자선의 최종적인 위치
		Distance = OutHit.Distance;        // 최종적인 거리
		if (OutHit.GetComponent()->GetCollisionProfileName() == "EBodyMesh")
		{ // 히트된 컴포넌트가 적이면
			RedCheck = true; // 십자선을 빨간색으로 바꾸기
		}
	}
	else { // 히트가 안되면
		CrossHairEndLoc = EndTracer;
		Distance = 2000.0f;
	}
	//FMath::Atan2()
	/* 탄퍼짐의 범위
	* 히트된 거리에 따른 범위를 구합니다.
	* 카메라의 FOV(시야각)을 이용해서 시야각과 히트거리간의 길이를 구하여 그 길이 만큼 탄을 퍼트립니다.*/
	float Radius = (FMath::Tan(FollowCamera->FieldOfView / 2.0f) * Distance) / CurrentSpreadSize;
	float BulletEndLocX = FMath::RandRange((CrossHairEndLoc + (FollowCamera->GetForwardVector() * Radius)).X,
		(CrossHairEndLoc + (-(FollowCamera->GetRightVector()) * Radius)).X);
	float BulletEndLocY = FMath::RandRange((CrossHairEndLoc + (FollowCamera->GetRightVector() * Radius)).Y,
		(CrossHairEndLoc + (-(FollowCamera->GetRightVector()) * Radius)).Y);
	float BulletEndLocZ = FMath::RandRange((CrossHairEndLoc + (FollowCamera->GetUpVector() * Radius)).Z,
		(CrossHairEndLoc + (-(FollowCamera->GetUpVector()) * Radius)).Z);

	// 탄퍼짐에 의해 결정된 최종적인 탄의 위치입니다.
	BulletFireLoc = FVector(BulletEndLocX, BulletEndLocY, BulletEndLocZ);

	// 십자선의 색을 결정합니다.
	if (RedCheck != IsCrossHairRed && HUD)
	{ // 항상 바뀌게 하는 것이 아닌 기존의 있는 데이터(IsCrossHairRed)와 다르면 작동되게 만들었습니다.
		IsCrossHairRed = RedCheck;
		HUD->CrossHairRedCheck.Execute(IsCrossHairRed);
	}
	return hitis;
}

/*반동에 관련된 처리를 하는함수입니다.
* Trace를 발사해서 히트된 거리에 따라 탄퍼짐이 계산되며
* 반동 이후에 사격이 시작된 지점으로 되돌아 오는 기능이 있습니다.
*/
bool AMultiPlayerBase::RecoilCheck()
{
	// Trace 부분은 현재 작동되는 여부를 나타내기 위해서만 존재하는 상태입니다. 
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	FVector StartTracer = FollowCamera->GetComponentLocation();
	FVector EndTracer = StartTracer + FollowCamera->GetForwardVector() * 2500.0f;
	FVector CameraEnd = StartTracer + FollowCamera->GetForwardVector() * 2500.0f;
	FVector FinalTracer = FVector::ZeroVector;
	if (RecoilReturnLoc != FVector::ZeroVector && IsFire) {
		FVector Dirdir = FVector(FollowCamera->GetForwardVector().X, FollowCamera->GetForwardVector().Y, RecoilReturnRot.Vector().Z);
		EndTracer = StartTracer + Dirdir * 2500.0f;//RecoilReturnLocation;
	}
	bool hitis = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartTracer, EndTracer, ETraceTypeQuery::TraceTypeQuery4, false
		, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	// 손이 일반지점으로 되돌아와야하는지를 알려줍니다.
	if (RecoilReturnRot != FRotator::ZeroRotator && !IsFire)
	{
		float RecoilNowRotatorPitch = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 360.0f), FVector2D(-90.0f, 90.0f), (Controller->GetControlRotation()).Pitch);
		float RecoilRotatorPitch = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 360.0f), FVector2D(-90.0f, 90.0f), RecoilReturnRot.Pitch);

		if (RecoilNowRotatorPitch >= 85.0f && RecoilRotatorPitch <= -85.0f) {
			return hitis;
		}
		if (RecoilRotatorPitch >= 80.0f && RecoilNowRotatorPitch <= -80.0f) {
			if (RecoilRotatorPitch <= RecoilNowRotatorPitch) {
				RecoilReturnRot = FRotator::ZeroRotator;
				return hitis;
			}
		}
		else {
			if (RecoilRotatorPitch >= RecoilNowRotatorPitch) {
				RecoilReturnRot = FRotator::ZeroRotator;
				return hitis;
			}
		}
		//UE_LOG(LogTemp, Warning, TEXT("RecodddilNowRotatorPitch: %f"), RecoilNowRotatorPitch);
		//UE_LOG(LogTemp, Warning, TEXT("RecoilRotatorPitch: %f"), RecoilRotatorPitch);
		AddControllerPitchInput(0.1f);
	}

	return hitis;
}

// 상호작용이 가능한 Actor인지를 확인하는 함수입니다.
bool AMultiPlayerBase::InteractionCheck()
{
	FHitResult outHit;
	FVector StartTracer = FollowCamera->GetComponentLocation();
	FVector EndTracer = StartTracer + FollowCamera->GetForwardVector() * 160.0f;
	TArray<AActor*> ActorsToIgnore;
	bool hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 20.0f, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore
		, EDrawDebugTrace::None, outHit, true);

	if (hitis) 
	{
		if (outHit.GetActor()->GetClass()->GetSuperClass() == AWeaponeBase::StaticClass()) 
		{ // 바라보는 Actor가 무기라면 
			LookWeapone = Cast<AWeaponeBase>(outHit.Actor);
			if (LookWeapone->GetWeaponeState() == WEAPONSTATE::DROP && LookWeapone->GetIsEmpty() == false) 
			{  
				// 무기의 UI를 On합니다. (Press F)
				LookWeapone->Execute_InteractionStart(LookWeapone);
			}
		}
		else { return hitis; }
	}
	else {
		if (LookWeapone != nullptr) 
		{
			// 무기의 UI를 Off합니다. (Press F)
			LookWeapone->Execute_InteractionEnd(LookWeapone);
			LookWeapone = nullptr;
		}
	}

	return hitis;
}

// 현제 들고 있는 무기들을 확인해서 장착할지 버릴지 결정합니다.
void AMultiPlayerBase::WeaponCheck(AWeaponeBase* check)
{
	if (check != nullptr) {
		if (BackWeapone1 != nullptr && BackWeapone2 != nullptr) 
		{ // 무기가 두개 있을 때
			if (EquipWeapone != nullptr) 
			{
				FDetachmentTransformRules DetachmentRule = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
				EquipWeapone->SetWeaponeState(WEAPONSTATE::DROP);
				EquipWeapone->DetachFromActor(DetachmentRule);
				EquipWeapone->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
				if (EquipWeapone == BackWeapone1) 
				{
					BackWeapone1 = check;
				}
				else BackWeapone2 = check;
			}
			UE_LOG(LogTemp, Warning, TEXT("TwoWeaponCheck"));
		}
		else {
			if (EquipWeapone != nullptr) 
			{ // 무기가 하나 있을 때
				/*if (EquipWeapone->GetStaticClass() != check->GetStaticClass())
				{
					
				}
				else 
				{ // 주우려는 무기가 같을 때는 탄약 수 더하기 

				}*/
				if (EquipWeapone == BackWeapone1) 
				{
					EquipWeapone->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("BackWeapone1"));
					BackWeapone2 = check;
				}
				else 
				{
					EquipWeapone->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("BackWeapone2"));
					BackWeapone1 = check;
				}
				EquipWeapone->SetWeaponeState(WEAPONSTATE::BACKEQUIP);
			}
			else 
			{ // 무기가 없을 때
				BackWeapone1 = check;
			}
		}

		// 탄약 계산을 합니다.
		ATwoVersus_PlayerState* State = Cast<ATwoVersus_PlayerState>(GetPlayerState());
		if (EquipWeapone && EquipWeapone->GetWeaponType() == WEAPONTYPE::RIFLE)
		{
			if (EquipWeapone->GetLimitAmmo() - EquipWeapone->GetBaseLoadedAmmo() >= State->RifleAmmo)
			{
				if (State->EquipAmmo != 0) State->RifleAmmo += EquipWeapone->GetBaseLoadedAmmo();
			}
			State->EquipAmmo = State->RifleAmmo;
		}
		if (HUD) HUD->EquipAmmoCheck.Execute(State->EquipAmmo);
		//UE_LOG(LogTemp, Warning, TEXT("WeaponCheck"));
		EquipGunOnHand(check);
	}
}

// 무기를 바꾸는 함수입니다.
// 들고 있는 무기를 등으로 보내고 등에 있는 무기를 손으로 옮깁니다.
void AMultiPlayerBase::WeaponeChange(EPlayerPress press)
{
	switch (press) 
	{
	case EPlayerPress::FIRSTGUN:
		if (BackWeapone1) 
		{
			if (EquipWeapone == BackWeapone2) 
			{
				EquipWeapone->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("BackWeapone2"));
				EquipWeapone->SetWeaponeState(WEAPONSTATE::BACKEQUIP);
			}
			EquipGunOnHand(BackWeapone1);
		}
		break;
	case EPlayerPress::SECONDGUN:
		if (BackWeapone2) {
			if (EquipWeapone == BackWeapone1) 
			{
				EquipWeapone->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("BackWeapone1"));
				EquipWeapone->SetWeaponeState(WEAPONSTATE::BACKEQUIP);
			}
			EquipGunOnHand(BackWeapone2);
		}
		break;
	case EPlayerPress::UNARMED:
		if (EquipWeapone) 
		{
			if (EquipWeapone == BackWeapone1) 
			{
				EquipWeapone->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("BackWeapone1"));
			}
			else EquipWeapone->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("BackWeapone2"));
			EquipWeapone->SetWeaponeState(WEAPONSTATE::BACKEQUIP);
		}
		break;
	}
	UE_LOG(LogTemp, Warning, TEXT("WeaponeChange"));
}

// 현재 들고있는 무기를 매개변수로 바꾸는 함수입니다.
void AMultiPlayerBase::EquipGunOnHand(AWeaponeBase* equip)
{
	EquipWeapone = equip;
	EquipWeapone->SetWeaponeState(WEAPONSTATE::EQUIP);
	EquipWeapone->SetOwner(this); // 여기 문제 영상 참고
	EquipWeapone->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponeLoc"));
	EquipWeapone->BodyMesh->SetOwnerNoSee(true);

	if (IsLocallyControlled()) 
	{
		if (EquipWeaponeArm) 
		{
			EquipWeaponeArm->Destroy();
			EquipWeaponeArm = nullptr;
		}
		EquipWeaponeArm = EquipWeapone->SpawnToHand(this, ArmMesh->GetSocketLocation(TEXT("WeaponeLoc")), ArmMesh->GetSocketRotation(TEXT("WeaponeLoc")));
		EquipWeaponeArm->SetWeaponeState(WEAPONSTATE::EQUIP);
		EquipWeaponeArm->BodyMesh->SetOnlyOwnerSee(true);
		EquipWeaponeArm->BodyMesh->SetOwnerNoSee(false);
		EquipWeaponeArm->SetOwner(this); // 여기 문제 영상 참고
		EquipWeaponeArm->AttachToComponent(ArmMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponeLoc"));

		ATwoVersus_PlayerState* state = Cast<ATwoVersus_PlayerState>(GetPlayerState());
		if (EquipWeapone->GetWeaponType() == WEAPONTYPE::RIFLE)
		{
			if (state->RifleAmmo == 0) state->RifleAmmo = EquipWeapone->GetBaseKeepAmmo();
			state->EquipAmmo = state->RifleAmmo;
			state->WeaponName = EquipWeapone->GetWeaponName();
		}
		state->LoadedAmmo = EquipWeapone->LoadedAmmo;
		if (HUD)
		{
			HUD->LoadedAmmoCheck.Execute(EquipWeapone->LoadedAmmo);
			HUD->EquipAmmoCheck.Execute(state->EquipAmmo);
			HUD->WeaponNameCheck.Execute(EquipWeapone->GetWeaponName());
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("EquipGunOnHand"));
}

// 일정 시간마다 연사를 할 수 있게 타이머를 설정했고 그 타이머가 끝나면 작동되는 함수입니다.
void AMultiPlayerBase::FireAutoOn()
{
	IsFireAuto = true;
	if (EquipWeaponeArm && EquipWeaponeArm->GetWeaponeLever() == WEAPONLEVER::TRIPLE)
	{
		if (ThreeCount > 1)
		{
			ThreeCount--;
			//UE_LOG(LogTemp, Warning, TEXT("ThreeCount: %f"), ThreeCount);
		}
		else {
			IsFire = false;                     // 비사격으로 체크
			IsFireAuto = false;                 // 연발사격을 해제
		}
	}
}

// 입력에 따른 방향 값을 통해 캐릭터를 이동시키는 함수입니다.
void AMultiPlayerBase::PlayerMove()
{
	if (IsMove)
	{
		if (MoveDir.IsZero()) 
		{ // 움직이는 방향이 없다면
			return;
		}
		MoveDir.Normalize();

		// Input에 따른 움직이는 힘 결정
		float InputForce = 0.0f;
		if (FMath::Abs(InputDirForward) > FMath::Abs(InputDirRight)) 
		{
			InputForce = FMath::Abs(InputDirForward);
		}
		else { InputForce = FMath::Abs(InputDirRight); }

		//UE_LOG(LogTemp, Warning, TEXT("Input: %f"), FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(0.0f, PlayerSpeed), InputDir));
		
		// 방향과 힘에 따른 움직임
		AddMovementInput(MoveDir, FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(0.0f, PlayerSpeed), InputForce) * 0.008f);
		// 하체 상태에 따른 움직임 추가 계산
		DownState->PlayerMove(this, InputForce, InputDirRight);
	}
}

// 플레이어를 죽는 상태로 만듭니다.
void AMultiPlayerBase::PlayerDead()
{
	IsMove = false; // 움직이지 못하게 합니다.

	GetWorldTimerManager().ClearTimer(HealTimer);	     // 치료 타이머 Off
	GetWorldTimerManager().ClearTimer(HitReactionTimer); // 물리 효과 타이머 Off

	// 무기를 바닥에 떨어트리기
	if (EquipWeapone) 
	{
		EquipWeapone->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		EquipWeapone->SetWeaponeState(WEAPONSTATE::DROP);
	}
	if (EquipWeaponeArm)
	{
		EquipWeaponeArm->Destroy();
	}
	if (BackWeapone1)
	{
		BackWeapone1->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		BackWeapone1->SetWeaponeState(WEAPONSTATE::DROP);
	}
	if (BackWeapone2) 
	{
		BackWeapone2->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		BackWeapone2->SetWeaponeState(WEAPONSTATE::DROP);
	}

	// 플레이어 설정
	// 충돌체 및 가렸던 몸 보이게 합니다.
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("DeadColl"));
	ArmMesh->SetCollisionProfileName(TEXT("DeadNomal"));
	ArmMesh->HideBoneByName(FName("root"), PBO_None);
	GetMesh()->SetCollisionProfileName(TEXT("DeadNomal"));
	GetMesh()->HideBoneByName(FName("root"), PBO_None);
	BodyMesh->SetCollisionProfileName(TEXT("DeadMesh"));
	BodyMesh->SetOnlyOwnerSee(false);
	BodyMesh->SetOwnerNoSee(false);
	BodyMesh->SetSimulatePhysics(true);

	// 컨트롤러에게 알려 서버의 GameMode에게 전달합니다.
	UE_LOG(LogTemp, Warning, TEXT("PlayerDead"));
	ATwoVersus_PlayerController* Ctrl = Cast<ATwoVersus_PlayerController>(GetController());
	if (Ctrl && Ctrl->PlayerDeadCheck.IsBound()) Ctrl->PlayerDeadCheck.Execute();
}

// 플레이어의 앞에 벽이 있는지를 인식하는 Trace를 쏘는 함수입니다.
bool AMultiPlayerBase::WallForwardTracer(FVector& Loc, FVector& Nomal)
{
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;
	FVector StartTracer = GetMesh()->GetComponentLocation() + FVector(0.0f, 0.0f, 80.0f);
	FVector EndTracer = StartTracer + GetMesh()->GetRightVector() * 40.0f;
	bool IsHit = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 20.0f, ETraceTypeQuery::TraceTypeQuery5, false
		, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	Loc = OutHit.Location;
	Nomal = OutHit.Normal;

	return IsHit;
}

// 인식된 벽의 높이를 알기 위해 Trace를 쏘는 함수입니다.
bool AMultiPlayerBase::WallHeightTracer(FVector Loc, FVector Nomal, FVector& Height)
{
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;
	FVector EndTracer = Loc + (FRotationMatrix::MakeFromX(Nomal).GetUnitAxis(EAxis::X) * (-10.0f));
	FVector StartTracer = EndTracer + FVector(0.0f, 0.0f, 100.0f);
	bool IsHit = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 20.0f, ETraceTypeQuery::TraceTypeQuery5, false
		, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	Height = OutHit.Location;
	if (Height.Z >= StartTracer.Z - 2.0f)
	{
		return false;
	}
	//UE_LOG(LogTemp, Warning, TEXT("WallHeight: %f, %f, %f"), WallHeight.X, WallHeight.Y, WallHeight.Z);
	//UE_LOG(LogTemp, Warning, TEXT("WallLocation: %f, %f, %f"), WallLocation.X, WallLocation.Y, WallLocation.Z);

	float MinHeight = Height.Z - Loc.Z;
	if (MinHeight > 60.0f)
	{
		IsWall = true;
	}
	else { IsWall = false; }
	return IsHit;
}

// 인식된 벽의 뒷 공간의 높이를 알기 위해 Trace를 쏘는 함수입니다.
bool AMultiPlayerBase::WallBackHeightTracer(FVector Loc, FVector& BackHeight)
{
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;
	FVector EndTracer = Loc + GetActorForwardVector() * 90.0f;
	FVector StartTracer = EndTracer + FVector(0.0f, 0.0f, 100.0f);
	bool IsHit = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 15.0f, ETraceTypeQuery::TraceTypeQuery5, false
		, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	BackHeight = OutHit.Location;
	if (!IsHit)
	{
		IsWallThick = true;
	}
	else { IsWallThick = false; }

	return IsHit;
}

// 벽의 상태에 따라 넘어가기 및 올라가기 작동
void AMultiPlayerBase::PlayerVault(FVector Loc, FVector Nomal, FVector Height)
{
	if (BodyAnim == nullptr || GetMesh() == nullptr) {
		return;
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;

	// WallNomal로 뽑아낸 벽의 방향을 Z축 으로 180도 회전하여 플레이어가 바라 보는 방향으로 바꿔서 WallNomalXRotator저장한다.
	FRotator WallNomalXRotator = FRotator(Nomal.Rotation().Pitch, Nomal.Rotation().Yaw - 180.0f, Nomal.Rotation().Roll);;
	SetActorRotation(FRotator(0.0f, WallNomalXRotator.Yaw, 0.0f));

	ArmMesh->SetOwnerNoSee(true);
	BodyMesh->SetOwnerNoSee(false);
	//IsCameraLock = true;
	if (EquipWeaponeArm) EquipWeaponeArm->SetHidden(true);
	// 벽의 X,Y 좌표로 부터 플레이어가 정확하게 서야할(넘어가야할)위치를 찾는다.
	//벽의 좌표를 뽑아낼때 정확한 위치가 아닌 벽의 중앙 위치만 뽑아 낼것이다.
	//그래서 아래의 과정을 통해 플레이어 기준의 벽의 위치가 나온다.
	FVector WallLocationXY = Loc + (FRotationMatrix::MakeFromX(Nomal).GetUnitAxis(EAxis::X) * (40.0f));
	// 플레이어가 넘어가야할 벽의 정확한 좌표
	FVector WallLocationTracer = FVector::ZeroVector;
	if (IsWallThick == false) {
		//UE_LOG(LogTemp, Warning, TEXT("ClimbMontage"));
		WallLocationTracer = FVector(WallLocationXY.X, WallLocationXY.Y, Height.Z - 15.0f);
		BodyAnim->PlayClimbMontage();
		LegAnim->PlayClimbMontage();
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("VaultMontage"));
		WallLocationTracer = FVector(WallLocationXY.X, WallLocationXY.Y, Height.Z - 25.0f);
		BodyAnim->PlayVaultMontage();
		LegAnim->PlayVaultMontage();

	}
	// 플레이어의 위치를 조정한다.
	SetActorLocation(WallLocationTracer);

	// 카메라 조정
	//SpringArm->AttachToComponent(BodyMesh, FAttachmentTransformRules::KeepRelativeTransform, "CameraLoc");
	//SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

// 넘어가기를 멈춥니다.
void AMultiPlayerBase::StopVault()
{
	// 보이는 몸 설정
	ArmMesh->SetOwnerNoSee(false);
	BodyMesh->SetOwnerNoSee(true);
	BodyAnim->StopMontage();
	LegAnim->StopMontage();

	// 카메라 설정
	//IsCameraLock = false;
	//SpringArm->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	//SpringArm->SetRelativeLocation(FVector(25.0f, 0.0f, 65.0f));

	if (EquipWeaponeArm) EquipWeaponeArm->SetHidden(false);
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	IsVault = false;
	//UE_LOG(LogTemp, Warning, TEXT("Vault"));
}

// 올라가기를 멈춥니다.
void AMultiPlayerBase::StopClimb()
{
	ArmMesh->SetOwnerNoSee(false);
	BodyMesh->SetOwnerNoSee(true);
	BodyAnim->StopMontage();
	LegAnim->StopMontage();
	IsCameraLock = false;
	if (EquipWeaponeArm) EquipWeaponeArm->SetHidden(false);
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	IsVault = false;
}

// 히트 리액션이 끝나고 작동하는 함수(물리표현이 원래대로 돌아가는 것)
void AMultiPlayerBase::HitReactionEnd()
{
	IsHitReactionEnd = true;
	HitReactionAlpha = 0.5f;
}

// 자동 체력 회복을 시작하는 함수입니다.(현재는 카메라 복구만 가능하고 힐을 꺼둔 상태입니다.)
void AMultiPlayerBase::StratAutoHeal()
{
	UE_LOG(LogTemp, Warning, TEXT("StratAutoHeal"));
	FollowCamera->PostProcessBlendWeight = 0.0f;
	/*ATwoVersus_PlayerState* state = Cast<ATwoVersus_PlayerState>(GetPlayerState());
	if (state) {
		state->StartHeal();
	}*/
}

// 앉기(C) 키와 바인딩 함수
void AMultiPlayerBase::PlayerCrouch()
{
	DownPress(nullptr);
	
	if (!HasAuthority()) 
	{
		Server_SendDownPress(EPlayerPress::CROUCH);
	}
	else { NetMulticast_SendDownPress(EPlayerPress::CROUCH); }
}

// 점프(Space) 키와 바인딩 함수
void AMultiPlayerBase::PlayerJump()
{
	IsJumped = true;
	Server_SendIsJumped(IsJumped);
	auto jump = DownState->PlayerJump(this);
	if (jump != nullptr)
	{
		DownPress(jump);
		/*if (!HasAuthority()) {
			Server_SendDownPress(EPlayerPress::CROUCH);
		}
		else { NetMulticast_SendDownPress(EPlayerPress::CROUCH); }*/
	}

	if (WallForwardTracer(WallLoc, WallNomal) && WallHeightTracer(WallLoc, WallNomal, WallHeight) && !IsVault) {
		WallBackHeightTracer(WallLoc, WallBackHeight);
		IsVault = true;
		PlayerVault(WallLoc, WallNomal, WallHeight);
		Server_SendValutCheck();
	}
}

// 달리기(Shift) 키와 바인딩 함수
void AMultiPlayerBase::PlayerSplint()
{
	DownPress(nullptr);
	if (!HasAuthority()) {
		Server_SendDownPress(EPlayerPress::SPLINT);
	}
	else {  NetMulticast_SendDownPress(EPlayerPress::SPLINT); }
}

// 1번 무기 선택(1) 키와 바인딩 함수
void AMultiPlayerBase::PlayerFirstGun()
{
	UpperPress(nullptr);
	WeaponeChange(EPlayerPress::FIRSTGUN);
	if (!HasAuthority()) {
		Server_SendUpperPress(EPlayerPress::FIRSTGUN);
		Server_SendWeaponeChange(EPlayerPress::FIRSTGUN);
	}
	else { 
		NetMulticast_SendUpperPress(EPlayerPress::FIRSTGUN);
		NetMulticast_SendWeaponeChange(EPlayerPress::FIRSTGUN);
	}
}

// 2번 무기 선택(2) 키와 바인딩 함수
void AMultiPlayerBase::PlayerSecondGun()
{
	UpperPress(nullptr);
	WeaponeChange(EPlayerPress::SECONDGUN);
	if (!HasAuthority()) {
		Server_SendUpperPress(EPlayerPress::SECONDGUN);
		Server_SendWeaponeChange(EPlayerPress::SECONDGUN);
	}
	else { 
		NetMulticast_SendUpperPress(EPlayerPress::SECONDGUN); 
		NetMulticast_SendWeaponeChange(EPlayerPress::SECONDGUN);
	}
}

// 비무장(3) 키와 바인딩 함수
void AMultiPlayerBase::PlayerUnArmed()
{
	UpperPress(nullptr);
	WeaponeChange(EPlayerPress::UNARMED);
	if (IsLocallyControlled() && EquipWeaponeArm)
	{
		EquipWeaponeArm->SetActorHiddenInGame(true);
	}
	if (!HasAuthority()) {
		Server_SendUpperPress(EPlayerPress::UNARMED);
		Server_SendWeaponeChange(EPlayerPress::UNARMED);
	}
	else { 
		NetMulticast_SendUpperPress(EPlayerPress::UNARMED); 
		NetMulticast_SendWeaponeChange(EPlayerPress::UNARMED);
	}
}

// 상호작용(F) 키와 바인딩 함수
void AMultiPlayerBase::PlayerInteraction()
{
	if (LookWeapone) {
		//UE_LOG(LogTemp, Warning, TEXT("PlayerInteraction"));
		WeaponCheck(LookWeapone);
		UpperPress(nullptr);
		if (!HasAuthority()) {
			Server_SendWeaponeCheck(LookWeapone);
			Server_SendUpperPress(EPlayerPress::INTERACTION);
		}
		else {
			NetMulticast_SendWeaponeCheck(LookWeapone);
			NetMulticast_SendUpperPress(EPlayerPress::INTERACTION);
		}
		/*else { 
			NetMulticast_SendWeaponeCheck(lookWeapone);
			NetMulticast_SendUpperPress(EPlayerPress::INTERACTION); 
		}*/
	}
}

// 정밀조준(마우스 오른쪽키) 키를 누를 때 바인딩 함수
void AMultiPlayerBase::PlayerADS()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerADS"));
	//IsADS = true;
	ArmAnim->StopAllMontages(0.1f);
	UpperPress(nullptr);
	
	if (!HasAuthority()) {
		Server_SendUpperPress(EPlayerPress::ADS);
	}
	else { NetMulticast_SendUpperPress(EPlayerPress::ADS); }
}

// 정밀조준(마우스 오른쪽키) 키를 땔 때 바인딩 함수
void AMultiPlayerBase::PlayerUnADS()
{
	if (UpperState->GetEState() == EPlayerUpperState::ADS)
	{
		//IsADS = false;
		// 정밀조준 애니메이션은 몽타주로 플레이 했기 때문에 몽타주를 멈추게 합니다.
		ArmAnim->StopAllMontages(0.1f);

		// 상체 상태를 바꾸게 합니다. (입력에 따라 상태가 바뀝니다.)
		UpperPress(nullptr);

		// 서버와 다른 클라이언트에 복제된 캐릭터도 상태를 바꾸게 하기 위해 RPC로 전달합니다.
		if (!HasAuthority()) 
		{ // 클라이언트 일 때 (데디케이티드 서버)
			Server_SendUpperPress(EPlayerPress::ADS);
		}
		else 
		{ // 서버 일 때 (리슨 서버)
			NetMulticast_SendUpperPress(EPlayerPress::ADS); 
		}
	}
}

/*공격 버튼을 눌렀을 때 반응하는 함수입니다.
*/
void AMultiPlayerBase::PlayerFire()
{
	if (EUpperState == EPlayerUpperState::UNARMED)
	{ // 비무장이면 근접공격을 실행합니다.
		Server_SendMelee(FollowCamera->GetComponentLocation(), FollowCamera->GetForwardVector());
		ArmAnim->PlayMeleeMontage();
	}
	else 
	{ // 무장 상태이며
		if (EquipWeapone->LoadedAmmo > 0)
		{ // 현재 들고있는 총의 탄이 존재하면
			IsFire = true;                           // 사격을 체크
			RecoilReturnRot = FRotator::ZeroRotator; // 반동 되돌아오는 지점 찍기
			CurrentSpreadSize = BaseSpreadSize;
			if (EquipWeapone && EquipWeaponeArm)
			{ // 현재 들고있는 무기가 존재 한다면
				// 총알을 발사합니다.
				UpperState->PlayerFire(this, EquipWeapone, IsFireAuto, ThreeCount
					, EquipWeaponeArm->BodyMesh->GetSocketLocation("Muzzle"), FollowCamera->GetComponentRotation(), BulletFireLoc);
			}
		}
		else 
		{ // 탄이 존재하지 않다면
			EquipWeapone->EmptyCheck.Execute(); // 탄약이 비어있다고 알리기 위한 사운드를 실행하는 함수입니다.
		}
	}
}

/*공격 버튼을 때었을 때 반응하는 함수입니다.
*/
void AMultiPlayerBase::PlayerUnFire()
{
	if (EUpperState != EPlayerUpperState::UNARMED)
	{ // 무장 상태라면
		ArmAnim->StopFireMontage();         // 사격 Anim을 멈추는 함수입니다.
		if (EquipWeaponeArm && EquipWeaponeArm->GetWeaponeLever() != WEAPONLEVER::TRIPLE)
		{
			IsFire = false;                     // 비사격으로 체크
			IsFireAuto = false;                 // 연발사격을 해제
		}
		CurrentSpreadSize = BaseSpreadSize; 
		if (HUD && EquipWeapone) HUD->SetCrossHairSpread(0.0f, false, false);
	}
}

/*장전버튼을 눌렀을때 작동하는 함수입니다.
*/
void AMultiPlayerBase::PlayerReload()
{
	if (EquipWeapone) {
		ArmAnim->PlayReloadMontage();
		EquipWeapone->PlayReloadMontage();
		EquipWeaponeArm->PlayReloadMontage();
	}
	ATwoVersus_PlayerState* state = Cast<ATwoVersus_PlayerState>(GetPlayerState());
	if (state->RifleAmmo <= 0 || EquipWeapone->LoadedAmmo >= EquipWeapone->GetBaseLoadedAmmo()) { return; }
	if (state->RifleAmmo < (EquipWeapone->GetBaseLoadedAmmo() - EquipWeapone->LoadedAmmo)) {
		EquipWeapone->LoadedAmmo = EquipWeapone->LoadedAmmo + state->RifleAmmo;
		state->RifleAmmo = 0;
	}
	else {
		state->RifleAmmo = state->RifleAmmo - (EquipWeapone->GetBaseLoadedAmmo() - EquipWeapone->LoadedAmmo);
		EquipWeapone->LoadedAmmo = EquipWeapone->GetBaseLoadedAmmo();
	}
	state->LoadedAmmo = EquipWeapone->LoadedAmmo;
	state->EquipAmmo = state->RifleAmmo;
	if (HUD) {
		HUD->LoadedAmmoCheck.Execute(EquipWeapone->LoadedAmmo);
		HUD->EquipAmmoCheck.Execute(state->EquipAmmo);
	}
	Server_SendReloadAmmo(state->LoadedAmmo, state->EquipAmmo);
}

// 엎드리기(Z) 키와 바인딩 함수
void AMultiPlayerBase::PlayerProne()
{
	DownPress(nullptr);
	if (!HasAuthority()) {
		Server_SendDownPress(EPlayerPress::PRONE);
	}
	else { NetMulticast_SendDownPress(EPlayerPress::PRONE); }
}

// 테스트용
void AMultiPlayerBase::PlayerSelfDead()
{
	//UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), 5.0f, GetActorForwardVector(), OutHit, nullptr, this, UMelee_DamageType::StaticClass());

}

// 테스트용
void AMultiPlayerBase::PlayerWeaponLever()
{
	if (EquipWeaponeArm && EquipWeapone)
	{
		EquipWeapone->ChangeWeaponeLever();
		EquipWeaponeArm->ChangeWeaponeLever();

		UE_LOG(LogTemp, Warning, TEXT("WeaponeLever: %d"), (int)(EquipWeapone->GetWeaponeLever()));
	}
}

// 전, 후 이동키(W, S) 바인딩 함수
void AMultiPlayerBase::MoveForward(float Value)
{
	if (IsMove)
	{
		InputDirForward = Value;
		if ((Controller != NULL) && (Value != 0.0f))
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
			Direction.Z = 0.0f;
			Direction.Normalize();
			MoveDir += Direction * FMath::Clamp(Value, -1.0f, 1.0f);
			//UE_LOG(LogTemp, Warning, TEXT("InputDirForward: %f"), InputDirForward);
		}
	}
}

// 좌, 우 이동키(A, D) 바인딩 함수
void AMultiPlayerBase::MoveRight(float Value)
{
	if (IsMove) {
		InputDirRight = Value;
		if ((Controller != NULL) && (Value != 0.0f)) {
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			FVector Direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::Y);
			Direction.Z = 0.0f;
			Direction.Normalize();
			MoveDir += Direction * FMath::Clamp(Value, -1.0f, 1.0f);
		}
	}
}

// 좌, 우 시점키(마우스 이동) 바인딩 함수
void AMultiPlayerBase::TurnAtRate(float Rate)
{
	AimDirRight = Rate;
	if (DownState)
	{
		DownState->TurnAtRate(this, AimDirRight);
	}
	
}

// 상, 하 시점키(마우스 이동) 바인딩 함수
void AMultiPlayerBase::LookUpAtRate(float Rate)
{
	AimDirUp = Rate;
	if (DownState)
	{
		DownState->LookUpAtRate(this, Rate);
	}

}

// 데미지를 받으면 어떤 반응을 해야하는지를 나타내는 함수입니다.
float AMultiPlayerBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("TakeDamage"));
	//ApplyPointDamage를 통해 받은 데미지
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	float CurrentHP = 0.0f; // 데미지가 계산된 현재의 체력
	if (!IsDead)
	{ // 죽지 않았다면
		ATwoVersus_PlayerState* State = Cast<ATwoVersus_PlayerState>(GetPlayerState());

		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{ // DamageEvent가 FPointDamageEvent 구조체로 이루어져있는지 확인
			const FPointDamageEvent* Point = (FPointDamageEvent*)&DamageEvent;
			Damage = Point->Damage; // ApplyPointDamage를 통해 받은 데미지
			CurrentHP = State->GetPlyaerHP() - Damage; // 현재의 체력에서 데미지를 빼기
			State->DamageToHP(Damage); // PlayerState에 데미지를 전달하여 현재의 체력을 업데이트합니다.

			UE_LOG(LogTemp, Warning, TEXT("PlayerHP: %f"), CurrentHP);

			// 데미지 타입에 따른 효과표현(사운드, 파티클)
			// 지금은 둘다 동일한 효과로 표현되어있습니다.
			FString DamageName = DamageEvent.DamageTypeClass->GetFName().ToString(); // DamageType의 이름을 가져와 구분합니다,
			if (DamageName == "Bullet_DamageType") // 탄에 의한 데미지
			{
				// 파티클 표시
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitDecal, Point->HitInfo.Location);
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BulletHitHitSound, Point->HitInfo.Location);
			}
			else if (DamageName == "Melee_DamageType") // 근접 공격에 의한 데미지
			{
				// 파티클 표시
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitDecal, Point->HitInfo.Location);
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MeleeHitHitSound, Point->HitInfo.Location);
			}


			AMultiPlayer_HUD* Hud = Cast<AMultiPlayer_HUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

			// 플레이어의 상단에 있는 전체 체력을 업데이트 하는 것 입니다.
			// 첫번째 컨트롤러의 hud = 히트표시 -> 공격의 당사자 , 공격위치 표시 -> 공격을 당하는 플레이어
			if (GetWorld()->GetFirstPlayerController()->GetPawn() && Hud)
			{
				ATwoVersus_PlayerController* Ctrl = Cast<ATwoVersus_PlayerController>(GetWorld()->GetFirstPlayerController());
				if (Ctrl) Ctrl->UpdateMatchHPCheck.Execute();
			}

			// 타격에 성공했을 때 맞은 캐릭터가 죽으면 타격 성공 색이 빨간색이 됩니다.
			// 히트 표시를 할때 로컬 컨트롤을 쓰면 될 것 같다 (플레이어가 아니고 공격의 당사자가 플레이어고 그 플레이어의 hud가 존재한다면)
			if (!IsLocallyControlled() && DamageCauser == GetWorld()->GetFirstPlayerController()->GetPawn() && Hud)
			{
				if (CurrentHP > 0.0f)
				{
					Hud->HitRedCheck.Execute(false);
				}
				else
				{
					Hud->HitRedCheck.Execute(true);
				}
			}

			// 데미지 방향 표시, 타격 물리 표현
			if (IsLocallyControlled() && Hud)
			{ // 플레이어가 상대방에 의해 데미지를 받으면 상대방의 위치에 따른 데미지 방향을 표시합니다.
				Hud->PlyaerHitLocCheck.Execute(DamageCauser->GetActorLocation() + DamageCauser->GetActorForwardVector() * 50.0f);
				/*ATwoVersus_PlayerController* Ctrl = Cast<ATwoVersus_PlayerController>(GetController());
				Ctrl->UpdateMatchHPCheck.Execute();*/
			}
			else
			{ // 타격 물리 표현
				// 타격 물리 표현 끝나는 시간을 초기화합니다.
				if (GetWorld()->GetTimerManager().IsTimerActive(HitReactionTimer)) GetWorld()->GetTimerManager().ClearTimer(HitReactionTimer);
				IsHitReactionEnd = false;
				HitReactionAlpha = 0.0f;
				// 타격 물리 표현 초기화
				BodyMesh->SetAllBodiesPhysicsBlendWeight(0.0f, false);
				BodyMesh->SetAllBodiesSimulatePhysics(false);

				BodyMesh->SetAllBodiesBelowSimulatePhysics("spine_01", true, true);			  // 척추를 기준으로 하위 뼈들 물리 효과를 On합니다.
				BodyMesh->SetAllBodiesBelowPhysicsBlendWeight("spine_01", 0.5f, false, true); // 0.5정도의 물리 효과를 킵니다.
				// 팔을 기준으로 하위 뼈들 물리 효과를 Off합니다.
				BodyMesh->SetAllBodiesBelowSimulatePhysics("lowerarm_l", false, true);
				BodyMesh->SetAllBodiesBelowSimulatePhysics("lowerarm_r", false, true);

				// 데미지가 들어온 방향으로 척추에 힘을 가합니다.
				BodyMesh->AddForceToAllBodiesBelow(DamageCauser->GetActorForwardVector() * 500000.0f, "spine_01"); //500000.0f

				// 타격 물리 표현 끝나는 시간을 정합니다.
				GetWorld()->GetTimerManager().SetTimer(HitReactionTimer, this, &AMultiPlayerBase::HitReactionEnd, 1.0f, false);
			}
			if (HasAuthority()) {
				// 자동 힐 기능(기능은 구현 하지만 지금은 사용 하지 않습니다.)
				//PlayerState->StopHeal();
				//GetWorldTimerManager().ClearTimer(HealTimer);
				//GetWorldTimerManager().SetTimer(HealTimer, this, &AMultiPlayerBase::StratAutoHeal, 4.0f, false);
				// 데미지를 각 클라이언트에 전달합니다.
				NetMulticast_SendPlayerHit(Damage, DamageCauser->GetActorForwardVector(), Point->HitInfo, DamageCauser, DamageEvent.DamageTypeClass);
			}
			// 체력이 일정 수치(40)가 되면 카메라가 회색 빛으로 변합니다. 
			CheckPlayerHP(CurrentHP);
		}
	}	return Damage;
}

// 상체와 관련된 키가 눌렀을 때 상태가 변하는 함수입니다.
// state 매개 변수는 강제로 상태가 지정 될 때 사용이 됩니다.
// 사용을 안할 때는 nullptr을 넣습니다.
void AMultiPlayerBase::UpperPress(UPlayerUpperStateBase* state)
{
	UPlayerUpperStateBase* temp = nullptr;
	if (!state)
	{ // 강제로 변환이 될 상태가 없다면
		// 입력에 따른 상태 값을 불러 옵니다.
		temp = UpperState->HandleInput(this);
		if (temp == nullptr) { return; }
	}
	else 
	{ // 강제로 변환이 될 상태로 변환합니다.
		temp = state;
	}

	// 현재의 상태를 끝냅니다.
	UpperState->StateEnd(this);
	// 그 전 상태 (현재의 상태)를 저장합니다.(이 변수는 전 상태를 나타냅니다.)
	UpperStateBClass = UpperState->GetState();

	if (temp != nullptr) 
	{
		// 현재 상태를 파괴합니다.
		UpperState->DestroyComponent();

		// 입력에 혹은 매개 변수로 가져온 상태로 지정합니다.
		UpperState = temp;

		// 현재 상태를 저장합니다.
		UpperStateNClass = UpperState->GetState(); 
		EUpperState = UpperState->GetEState();
		if (UpperState->GetIsChangeState() == false)
		{ // 만약 바뀌는 과정이 없는 상태라면 상태를 바로 시작합니다.
			UpperState->StateStart(this);
		}
	}
}

// 하체와 관련된 키가 눌렀을 때 상태가 변하는 함수입니다.
void AMultiPlayerBase::DownPress(UPlayerDownStateBase* state)
{
	UPlayerDownStateBase* temp = nullptr;
	float ChangeSpeed = 0.0f;

	if (!state) {
		temp = DownState->HandleInput(this); // 입력된 키에 따른 상태 객체 반환
		if (temp == nullptr) { return; }
	}
	else { temp = state; }

	DownState->StateEnd(this); // 현재 상태 마무리 함수를 작동합니다.
	DownStateBClass = DownState->GetState();

	if (temp != nullptr) {
		if (DownState->GetIsChangeState())
		{ // 바뀌는 과정이 있는 상태이면
			temp->SetIsChangeState(true); // 다른 상태로 바뀔때도 과정이 있으므로 켜져야 한다.
			ChangeSpeed = DownState->GetChangeSpeed(); // 다른 상태로 바뀔때의 속도
		}

		DownState->DestroyComponent(); // 현재 상태를 메모리 해제 합니다.
		DownState = temp; // 현재 상태를 HandleInput에서 반환한 상태 객체를 지정합니다.
		DownStateNClass = DownState->GetState();
		EDownState = DownState->GetEState();

		if (DownState->GetIsChangeState() == false)
		{
			DownState->StateStart(this); // 상태가 바뀌었으므로 바뀐 상태에 대한 시작 함수를 작동 시킵니다.
		}
		else {
			DownState->StateChangeStart(this, ChangeSpeed);
		}
	}
}

/*상태끼리 변하고 있는 상황에서 변하는게 끝나면
* (애니메이션이 끝나면 AnimNotify를 해서 인터페이스로 플레이어에게 변하는 애니메이션이 끝났으니 StateStart를 하라고 알려줍니다.)
* StateStart를 작동하는 함수입니다.
*/
void AMultiPlayerBase::CurrentStateStart_Implementation(bool IsUpper)
{
	UE_LOG(LogTemp, Warning, TEXT("CurrentStateStart"));

	if (IsUpper)
	{
		if (UpperState->GetIsStart() == false) UpperState->StateStart(this);
	}
	else {
		if (DownState->GetIsStart() == false) DownState->StateStart(this);
	}
}

// 죽었는지 체력이 낮은지(체력이 40이하면 화면을 회색을 띄어야한다.)를 판단하는 함수입니다.
void AMultiPlayerBase::CheckPlayerHP(float hp)
{
	if (hp <= 40.0f && hp > 0 && FollowCamera->PostProcessBlendWeight != 1.0f) 
	{ // 채력이 40 이하면 카메라가 회색으로 변합니다.
		UE_LOG(LogTemp, Warning, TEXT("Gray"));
		FollowCamera->PostProcessBlendWeight = 1.0f;
		GetWorldTimerManager().ClearTimer(HealTimer);
		GetWorldTimerManager().SetTimer(HealTimer, this, &AMultiPlayerBase::StratAutoHeal, 4.0f, false);
	}
	else if (hp <= 0.0f)
	{ // 체력이 낮으면 캐릭터를 죽은 상태로 만듭니다.
		UE_LOG(LogTemp, Warning, TEXT("Dead"));
		FollowCamera->PostProcessBlendWeight = 1.0f;
		IsDead = true;
		PlayerDead();
	}
}

// 현재 들고있는 총으로 사격하는 함수입니다.
void AMultiPlayerBase::FireBullet(FVector MuzzleLoc, FRotator MuzzleRot, FVector BulletLoc)
{
	if (EquipWeapone)
	{
		// 발사 애니메이션을 실행합니다.
		ArmAnim->PlayFireMontage();
		EquipWeapone->PlayFireMontage();
		EquipWeaponeArm->PlayFireMontage();

		ATwoVersus_PlayerState* State = Cast<ATwoVersus_PlayerState>(GetPlayerState());
		// 탄 계산 및 PlayerState에 전달합니다.
		if (EquipWeapone->LoadedAmmo > 0)
		{
			EquipWeapone->LoadedAmmo -= 1;
			State->LoadedAmmo -= 1;
		}else 
		{ 
			IsFire = false;
		} // empty 동작

		// UI로 탄을 표시합니다.
		if (HUD) HUD->LoadedAmmoCheck.Execute(EquipWeapone->LoadedAmmo);
		/*if (EquipWeaponInAmmo > 1.0f) {
			EquipWeaponInAmmo -= 1.0f;
		}
		else { equipWeapone->PlayEmptyMontage(); }*/

		// 연속된 발사를 막기 위해 잠시 꺼둡니다.
		// 밑의 타이머에 의해 다시 켜지며 일정 시간 간격으로 탄이 연속적으로 나가게 합니다.
		IsFireAuto = false;

		// 반동에 의해 카메라가 위로 올라가면 처음에 탄이 발사 되었던 지점으로 되돌아오게 합니다.
		// 처음에 탄이 발사 되었던 지점을 여기서 설정합니다.
		RecoilReturnLoc = FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * 2500.0f;
		if (RecoilReturnRot == FRotator::ZeroRotator) {
			RecoilReturnRot = GetControlRotation();
		}

		// 반동이 들어갑니다.
		// 각 총마다 반동의 크기가 다릅니다.
		float AddPitch = EquipWeapone->GetFireRecoilPitch();
		float AddYaw = EquipWeapone->GetFireRecoilYaw();
		AddControllerPitchInput(AddPitch);
		AddControllerYawInput(AddYaw);

		// 결정된 탄 퍼짐에 따라 탄이 나갑니다.
		FRotator BulletFireRot = UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, BulletLoc);
		/*if (UpperState->GetEState() == EPlayerUpperState::ADS)
		{ // 정밀 조준 상태라면 탄이 일직선으로 나가게 합니다.
			BulletFireRot = ArmMesh->GetRightVector().Rotation();
		} // 조준 상태라면 탄 퍼짐에 의해 결정된 위치로 발사되게 합니다.
		else { BulletFireRot = UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, BulletLoc); }*/

		
		if (IsLocallyControlled())
		{
			EquipWeaponeArm->ProjectileFire(MuzzleLoc, MuzzleRot, BulletFireRot);
		}
		else
		{
			EquipWeapone->ProjectileFire(MuzzleLoc, MuzzleRot, BulletFireRot);
		}
		

		if (EquipWeapone->GetWeaponeLever() != WEAPONLEVER::SINGLEFIRE) 
		{ // 레버가 단발이 아니면 자동 발사를 On
			GetWorldTimerManager().SetTimer(FireTimer, this, &AMultiPlayerBase::FireAutoOn, 0.15f, false);
		}

		// 서버와 클라이언트에 전달
		if (IsLocallyControlled())
		{
			if (HasAuthority()) {
				NetMulticast_SendFireBullet(BulletLoc);
			}
			else { Server_SendFireBullet(BulletLoc); }
		}
	}
}


bool AMultiPlayerBase::NetMulticast_SendControllerRot_Validate(FRotator rot)
{
	return true;
}
void AMultiPlayerBase::NetMulticast_SendControllerRot_Implementation(FRotator rot)
{
	ControllerRot = rot;

	FRotator InterpToAngle = (ControllerRot - GetActorRotation()).GetNormalized();
}

bool AMultiPlayerBase::Server_SendDownPress_Validate(EPlayerPress press)
{
	return true;
}
void AMultiPlayerBase::Server_SendDownPress_Implementation(EPlayerPress press)
{
	NetMulticast_SendDownPress(press);
	//UE_LOG(LogTemp, Warning, TEXT("Server_SendDownState"));
}
bool AMultiPlayerBase::NetMulticast_SendDownPress_Validate(EPlayerPress press)
{
	return true;
}
void AMultiPlayerBase::NetMulticast_SendDownPress_Implementation(EPlayerPress press)
{
	if (!IsLocallyControlled()) {
		UPlayerDownStateBase* temp = DownState->SendHandleInput(press);
		float ChangeSpeed = 0.0f;

		DownState->StateEnd(this); // 현재 상태 마무리 함수를 작동합니다.
		DownStateBClass = DownState->GetState();

		if (temp != nullptr) {
			if (DownState->GetIsChangeState())
			{ // 바뀌는 과정이 있는 상태이면
				temp->SetIsChangeState(true); // 다른 상태로 바뀔때도 과정이 있으므로 켜져야 한다.
				ChangeSpeed = DownState->GetChangeSpeed(); // 다른 상태로 바뀔때의 속도
			}

			DownState->DestroyComponent(); // 현재 상태를 메모리 해제 합니다.
			DownState = temp; // 현재 상태를 HandleInput에서 반환한 상태 객체를 지정합니다.
			DownStateNClass = DownState->GetState();
			EDownState = DownState->GetEState();

			if (DownState->GetIsChangeState() == false)
			{
				DownState->StateStart(this); // 상태가 바뀌었으므로 바뀐 상태에 대한 시작 함수를 작동 시킵니다.
			}
			else {
				DownState->StateChangeStart(this, ChangeSpeed);
			}
		}
	}
}

bool AMultiPlayerBase::Server_SendUpperPress_Validate(EPlayerPress press)
{
	return true;
}
void AMultiPlayerBase::Server_SendUpperPress_Implementation(EPlayerPress press)
{
	NetMulticast_SendUpperPress(press);
}
bool AMultiPlayerBase::NetMulticast_SendUpperPress_Validate(EPlayerPress press)
{
	return true;
}
void AMultiPlayerBase::NetMulticast_SendUpperPress_Implementation(EPlayerPress press)
{
	if (!IsLocallyControlled()) {
		UE_LOG(LogTemp, Warning, TEXT("NetMulticast_SendUpperPress_Implementation"));
		UPlayerUpperStateBase* temp = UpperState->SendHandleInput(press);
		if (temp != nullptr) {
			UpperState->StateEnd(this);
			UpperStateBClass = UpperState->GetState();
			UpperState->DestroyComponent();
			UpperState = temp;
			UpperStateNClass = UpperState->GetState();
			EUpperState = UpperState->GetEState();
			if (UpperState->GetIsChangeState() == false)
			{
				UpperState->StateStart(this);
			}
		}
	}
}

bool AMultiPlayerBase::Server_SendWeaponeCheck_Validate(AWeaponeBase* check)
{
	return true;
}
void AMultiPlayerBase::Server_SendWeaponeCheck_Implementation(AWeaponeBase* check)
{
	if (check) {
		//WeaponCheck(check);
		//UE_LOG(LogTemp, Warning, TEXT("Server_SendWeaponeCheck_Implementation"));
		NetMulticast_SendWeaponeCheck(check);
	}
}
bool AMultiPlayerBase::NetMulticast_SendWeaponeCheck_Validate(AWeaponeBase* check)
{
	return true;
}
void AMultiPlayerBase::NetMulticast_SendWeaponeCheck_Implementation(AWeaponeBase* check)
{
	//if (!HasAuthority() && !IsLocallyControlled()) WeaponCheck(check);
	//UE_LOG(LogTemp, Warning, TEXT("NetMulticast_SendWeaponeCheck_Implementation"));
	if (!IsLocallyControlled()) WeaponCheck(check);
}
bool AMultiPlayerBase::Server_SendWeaponeChange_Validate(EPlayerPress press)
{
	return true;
}
void AMultiPlayerBase::Server_SendWeaponeChange_Implementation(EPlayerPress press)
{
	NetMulticast_SendWeaponeChange(press);
}
bool AMultiPlayerBase::NetMulticast_SendWeaponeChange_Validate(EPlayerPress press)
{
	return true;
}
void AMultiPlayerBase::NetMulticast_SendWeaponeChange_Implementation(EPlayerPress press)
{
	if (!IsLocallyControlled()) {
		WeaponeChange(press);
	}
}

bool AMultiPlayerBase::Server_SendIsJumped_Validate(bool jumped)
{
	return  true;
}
void AMultiPlayerBase::Server_SendIsJumped_Implementation(bool jumped)
{
	IsJumped = jumped;
}

bool AMultiPlayerBase::Server_SendValutCheck_Validate()
{
	return true;
}
void AMultiPlayerBase::Server_SendValutCheck_Implementation()
{
	if (WallForwardTracer(WallLoc, WallNomal) && WallHeightTracer(WallLoc, WallNomal, WallHeight) && !IsVault) {
		WallBackHeightTracer(WallLoc, WallBackHeight);
		IsVault = true;
		PlayerVault(WallLoc, WallNomal, WallHeight);
	}
	NetMulticast_SendValutCheck();
}
bool AMultiPlayerBase::NetMulticast_SendValutCheck_Validate()
{
	return true;
}
void AMultiPlayerBase::NetMulticast_SendValutCheck_Implementation()
{
	if (WallForwardTracer(WallLoc, WallNomal) && WallHeightTracer(WallLoc, WallNomal, WallHeight) && !IsVault) {
		WallBackHeightTracer(WallLoc, WallBackHeight);
		IsVault = true;
		PlayerVault(WallLoc, WallNomal, WallHeight);
	}
}

bool AMultiPlayerBase::Server_SendFireBullet_Validate(FVector loc)
{
	return true;
}
void AMultiPlayerBase::Server_SendFireBullet_Implementation(FVector loc)
{
	if (EquipWeapone && !IsLocallyControlled()) {
		EquipWeapone->PlayFireMontage();
		
		IsFireAuto = false;
		FVector MuzzleLoc = EquipWeapone->BodyMesh->GetSocketLocation("Muzzle");
		FRotator BulletFireRot = UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, loc);

		EquipWeapone->ProjectileFire(EquipWeapone->BodyMesh->GetSocketLocation("Muzzle")
			, BulletFireRot, BulletFireRot);
		if (EquipWeapone->GetWeaponeLever() == WEAPONLEVER::FULLAUTO) {
			GetWorldTimerManager().SetTimer(FireTimer, this, &AMultiPlayerBase::FireAutoOn, 0.15f, false);
		}
		NetMulticast_SendFireBullet(loc);
	}
}
bool AMultiPlayerBase::NetMulticast_SendFireBullet_Validate(FVector loc)
{
	return true;
}
void AMultiPlayerBase::NetMulticast_SendFireBullet_Implementation(FVector loc)
{
	if (EquipWeapone && !IsLocallyControlled() && !HasAuthority()) {
		EquipWeapone->PlayFireMontage();

		IsFireAuto = false;
		FVector MuzzleLoc = EquipWeapone->BodyMesh->GetSocketLocation("Muzzle");
		FRotator BulletFireRot = UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, loc);

		EquipWeapone->ProjectileFire(EquipWeapone->BodyMesh->GetSocketLocation("Muzzle")
			, BulletFireRot, BulletFireRot);
		if (EquipWeapone->GetWeaponeLever() == WEAPONLEVER::FULLAUTO) {
			GetWorldTimerManager().SetTimer(FireTimer, this, &AMultiPlayerBase::FireAutoOn, 0.15f, false);
		}
	}
}

bool AMultiPlayerBase::Server_SendMelee_Validate(FVector Loc, FVector Dir)
{
	return true;
}
void AMultiPlayerBase::Server_SendMelee_Implementation(FVector Loc, FVector Dir)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_SendMelee_Implementation"));
		NetMulticast_SendMelee();

		FHitResult OutHit;
		FVector StartTracer = Loc;
		FVector EndTracer = StartTracer + Dir * 90.0f;
		TArray<AActor*> ActorsToIgnore;
		bool hitis = UKismetSystemLibrary::LineTraceSingle(this, StartTracer, EndTracer, ETraceTypeQuery::TraceTypeQuery4, false, ActorsToIgnore
			, EDrawDebugTrace::None, OutHit, true);

		if (hitis && OutHit.Component->GetCollisionProfileName() == "EBodyMesh")
		{
			UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), 5.0f, GetActorForwardVector(), OutHit, nullptr, this, UMelee_DamageType::StaticClass());
			UE_LOG(LogTemp, Warning, TEXT("Melee: Player Hit"));
		}
	}
}
bool AMultiPlayerBase::NetMulticast_SendMelee_Validate()
{
	return true;
}
void AMultiPlayerBase::NetMulticast_SendMelee_Implementation()
{
	if(!IsLocallyControlled()) BodyAnim->PlayMeleeMontage();
}

bool AMultiPlayerBase::NetMulticast_SendPlayerHit_Validate(float Damage, FVector Dir, FHitResult Hit, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType)
{
	return true;
}
void AMultiPlayerBase::NetMulticast_SendPlayerHit_Implementation(float Damage, FVector Dir, FHitResult Hit, AActor* DamageCauser, TSubclassOf<UDamageType> DamageType)
{
	if (IsLocallyControlled()) {
		
	}
	if (!HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("NetMulticast_SendPlayerHit: %f"), Damage);
		float cd = Damage;
		UGameplayStatics::ApplyPointDamage(this, cd, Dir, Hit, nullptr, DamageCauser, DamageType);
	}
	else {
		ATwoVersus_GameState* State = Cast<ATwoVersus_GameState>(UGameplayStatics::GetGameState(this));
		if (State) State->ChangeTeamHPCheck.Execute();
	}
}

bool AMultiPlayerBase::Server_SendReloadAmmo_Validate(int loadAmmo, int EquipAmmo)
{
	return true;
}
void AMultiPlayerBase::Server_SendReloadAmmo_Implementation(int loadAmmo, int EquipAmmo)
{
	ATwoVersus_PlayerState* state = Cast<ATwoVersus_PlayerState>(GetPlayerState());
	state->LoadedAmmo = loadAmmo;
	state->EquipAmmo = EquipAmmo;
}

bool AMultiPlayerBase::Server_SendPlayerStart_Validate()
{
	return true;
}
void AMultiPlayerBase::Server_SendPlayerStart_Implementation()
{
	NetMulticast_SendPlayerStart();
}
bool AMultiPlayerBase::NetMulticast_SendPlayerStart_Validate()
{
	return true;
}
void AMultiPlayerBase::NetMulticast_SendPlayerStart_Implementation()
{
	IsPlayerCameraTurn = true;
	IsMove = true;
}

bool AMultiPlayerBase::Server_SetPlayerStartLoc_Validate()
{
	return true;
}
void AMultiPlayerBase::Server_SetPlayerStartLoc_Implementation()
{
	ATwoVersus_GameState* State = Cast<ATwoVersus_GameState>(UGameplayStatics::GetGameState(this));
	ATwoVersus_GameMode* GameMode = Cast<ATwoVersus_GameMode>(UGameplayStatics::GetGameMode(this));
	ATwoVersus_PlayerController* Ctrl = Cast<ATwoVersus_PlayerController>(GetController());
	ATwoVersus_PlayerState* PState = Cast<ATwoVersus_PlayerState>(GetPlayerState());
	PState->TeamName = Ctrl->GetTeamName();
	for (int i = 0; i < State->AllPlayerStart.Num(); i++) {
		ATwoVersus_PlayerStart* PlayerStart = Cast<ATwoVersus_PlayerStart>(State->AllPlayerStart[i]);
		UE_LOG(LogTemp, Warning, TEXT("TeamName_P: %s"), *(PlayerStart->TeamName));
		UE_LOG(LogTemp, Warning, TEXT("TeamName_C: %s"), *(Ctrl->GetTeamName()));
		if (Ctrl->GetTeamName() == PlayerStart->TeamName && !(PlayerStart->IsUse)) {
			UE_LOG(LogTemp, Warning, TEXT("index: %d"), i);
			PlayerStart->IsUse = true;
			SetActorLocation(PlayerStart->GetActorLocation());
			break;
		}
	}

}

void AMultiPlayerBase::OnRep_TeamNameChange()
{
	if (TeamName == "Blue")
	{
		ArmMesh->SetMaterial(0.0f, BlueMat);
		BodyMesh->SetMaterial(0.0f, BlueMat);
		GetMesh()->SetMaterial(0.0f, BlueMat);
	}
	else
	{
		ArmMesh->SetMaterial(0.0f, RedMat);
		BodyMesh->SetMaterial(0.0f, RedMat);
		GetMesh()->SetMaterial(0.0f, RedMat);
	}
}

void AMultiPlayerBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMultiPlayerBase, IsJumped, COND_SkipOwner);
	DOREPLIFETIME(AMultiPlayerBase, IsCoverLeft);
	DOREPLIFETIME(AMultiPlayerBase, IsCoverRight);
	DOREPLIFETIME(AMultiPlayerBase, CoverAngleLeft);
	DOREPLIFETIME(AMultiPlayerBase, CoverAngleRight);
	DOREPLIFETIME(AMultiPlayerBase, TeamName);
}
