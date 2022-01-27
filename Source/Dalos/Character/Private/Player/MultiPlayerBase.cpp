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

	// �浹ü(ĸ��)
	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->InitCapsuleSize(50.0f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");

	// Mesh(�ٸ�)
	GetMesh()->SetupAttachment(RootComponent);
	GetMesh()->SetIsReplicated(true);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -97.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName("CharacterMesh");

	// Mesh(��)
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BODY"));
	BodyMesh->SetGenerateOverlapEvents(true); // Overlap���� �浹�� �� �����ϰ� �մϴ�.
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -97.0f));
	BodyMesh->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	BodyMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	BodyMesh->SetCollisionProfileName("EBodyMesh");

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>FULLBODY_SKELETALMESH(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (FULLBODY_SKELETALMESH.Succeeded()) 
	{ // Mesh ����
		GetMesh()->SetSkeletalMesh(FULLBODY_SKELETALMESH.Object);
		BodyMesh->SetSkeletalMesh(FULLBODY_SKELETALMESH.Object);
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance>FULLBODY_ANIMBP(TEXT("AnimBlueprint'/Game/Player/Anim/PlayerBodyAnimBP.PlayerBodyAnimBP_C'"));
	if (FULLBODY_ANIMBP.Succeeded()) 
	{ // AnimIns ����
		GetMesh()->SetAnimInstanceClass(FULLBODY_ANIMBP.Class);
		BodyMesh->SetAnimInstanceClass(FULLBODY_ANIMBP.Class);
	}
	

	// SpringArm(ī�޶� �ٸ�)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(25.0f, 0.0f, 65.0f));
	SpringArm->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
	SpringArm->bDoCollisionTest = true;
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bUsePawnControlRotation = true; // ��Ʈ�ѷ��� ���� ȸ�� 
	SpringArm->bInheritPitch = true; // ��Ʈ�ѷ��� ���� Y�� ȸ�� 
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = true;   // ��Ʈ�ѷ��� ���� Z�� ȸ��  


	// ī�޶�
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(SpringArm);
	FollowCamera->SetIsReplicated(true);
	FollowCamera->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	FollowCamera->bUsePawnControlRotation = false; // ��Ʈ�ѷ��� ���� ȸ�� X
	FollowCamera->PostProcessBlendWeight = 0.0f;


	// Mesh(��)
	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	ArmMesh->SetupAttachment(FollowCamera);
	ArmMesh->SetCollisionProfileName("CharacterMesh");
	ArmMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -155.712738));
	ArmMesh->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	ArmMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>ARM_SKELETALMESH(TEXT("SkeletalMesh'/Game/FirstPerson/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));
	if (ARM_SKELETALMESH.Succeeded()) 
	{ // Mesh ����
		ArmMesh->SetSkeletalMesh(ARM_SKELETALMESH.Object); 
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance>ARM_ANIMBP(TEXT("AnimBlueprint'/Game/Player/Anim/PlayerArmAnimBP.PlayerArmAnimBP_C'"));
	if (ARM_ANIMBP.Succeeded()) 
	{ // AnimIns ����
		ArmMesh->SetAnimInstanceClass(ARM_ANIMBP.Class);
	}


	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // �̵����⿡ ���缭 ȸ���� �������ش�.(ȸ���� ���� �ٲٱ� ������ false)	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); // ȸ�� �ӵ� 0.0���� �ʱ�ȭ (�� ����?)
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;  // ��Ʈ�ѷ� �������� ĳ���� ȸ��(�����Ʈ ����)
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	Cast<UCharacterMovementComponent>(GetMovementComponent())->NetworkSmoothingMode = ENetworkSmoothingMode::Linear;


	// ��ƼŬ
	// �÷��̾ ������ ������ �� ȿ��
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BULLETPARTICLE(TEXT("ParticleSystem'/Game/BallisticsVFX/Particles/Destruction/Fractures/Legacy/Metal_fracture.Metal_fracture'"));
	if (BULLETPARTICLE.Succeeded()) {
		HitDecal = BULLETPARTICLE.Object;
	}


	// ����
	// ī�޶� ü�� ���� ���� ȸ�� ȭ��
	static ConstructorHelpers::FObjectFinder<USoundBase>BULLETHIT_SOUND(TEXT("SoundCue'/Game/BallisticsVFX/SFX/Impacts/MetalDull/MetalHit.MetalHit'"));
	if (BULLETHIT_SOUND.Succeeded()) {
		BulletHitHitSound = BULLETHIT_SOUND.Object;
	}

	// ī�޶� ü�� ���� ���� ȸ�� ȭ��
	static ConstructorHelpers::FObjectFinder<USoundBase>MELEEHIT_SOUND(TEXT("SoundCue'/Game/BallisticsVFX/SFX/Impacts/Crack/HitCue.HitCue'"));
	if (MELEEHIT_SOUND.Succeeded()) {
		MeleeHitHitSound = MELEEHIT_SOUND.Object;
	}


	// Material
	// ī�޶� ü�� ���� ���� ȸ�� ȭ��
	static ConstructorHelpers::FObjectFinder<UObject>GRAY_MATERIAL(TEXT("Material'/Game/Player/Grey_Camera.Grey_Camera'"));
	if (GRAY_MATERIAL.Succeeded())
	{
		FollowCamera->AddOrUpdateBlendable(GRAY_MATERIAL.Object, 1.0f);
	}

	// ����� �÷��̾� ��
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>BLUETEAM_MATERIAL(TEXT("Material'/Game/Player/Team/Character_BlueTeam_Mat.Character_BlueTeam_Mat'"));
	if (BLUETEAM_MATERIAL.Succeeded()) 
	{
		BlueMat = BLUETEAM_MATERIAL.Object;
	}

	// ������ �÷��̾� ��
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>REDTEAM_MATERIAL(TEXT("Material'/Game/Player/Team/Character_RedTeam_Mat.Character_RedTeam_Mat'"));
	if (REDTEAM_MATERIAL.Succeeded()) 
	{
		RedMat = REDTEAM_MATERIAL.Object;
	}
	
}

// �÷��̾ �����ǰ� ���� ������ �� �۵��Ǵ� �Լ�
// �þ߹� ������ ����, ���� �ʱ�ȭ, ���� ����
void AMultiPlayerBase::BeginPlay()
{
	Super::BeginPlay();

	// �÷��̾ �����ǰ� ������ ���� ��ġ�� ���϶�� �˸��ϴ�.
	if(IsLocallyControlled()) Server_SetPlayerStartLoc();

	IsPlayerCameraTurn = false; // ī�޶� �������� �� �ϰ� �մϴ�. (ī��Ʈ �ٿ��� ������ �����Դϴ�.)

	HUD = Cast<AMultiPlayer_HUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	// �÷��̾�, �ٸ� �÷��̾�� ���̰ų� ������ �ϴ� �κ��� ���մϴ�.
	GetMesh()->HideBoneByName(FName("spine_02"), PBO_None);
	GetMesh()->SetOnlyOwnerSee(true); // ���� �÷��̾�Ը� ���̰� �մϴ�. (�ٸ�)
	GetMesh()->SetCastShadow(false);  // ��ο��� �׸��ڰ� ������ �ʰ� �մϴ�. (�ٸ�)
	ArmMesh->SetOnlyOwnerSee(true);   // ���� �÷��̾�Ը� ���̰� �մϴ�. (��)
	ArmMesh->SetCastShadow(false);    // ��ο��� �׸��ڰ� ������ �ʰ� �մϴ�. (��)
	//ArmMesh->bCastHiddenShadow = true;

	// ��Ʈ�ѷ��� �������� �ʴٸ� ������ ���Ǵ� �浹ü�� �ǰ� �մϴ�.
	if(!IsLocallyControlled()) BodyMesh->SetCollisionProfileName("EBodyMesh");
	BodyMesh->SetOwnerNoSee(true);      // ���� �÷��̾�Ը� ������ �ʰ� �մϴ�. (��)		
	BodyMesh->bCastHiddenShadow = true; // ��ο��� �׸��ڰ� ���̰� �մϴ�. (��)

	// ���� �ʱ�ȭ
	UpperState = NewObject<UUnArmed_PlayerUpper>(this, UUnArmed_PlayerUpper::StaticClass()); // �� ���� ���·� �ʱ�ȭ
	DownState = NewObject<UStanding_PlayerDown>(this, UStanding_PlayerDown::StaticClass());  // ���ֱ� ���·� �ʱ�ȭ

	// �� ���º��� �����ϱ�
	UpperState->StateStart(this);
	DownState->StateStart(this);
	UpperStateNClass = UpperState->GetState();
	DownStateNClass = DownState->GetState();

	IsMove = false;				// �������� �� �ϰ� �մϴ�. (ī��Ʈ �ٿ��� ������ �����Դϴ�.)

	CharacterHp = 100.0f; // ������� �ʽ��ϴ�.

	// ���� ���� �� �й�
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

// ������Ʈ �ʱ�ȭ ���Ŀ� �۵��Ǵ� �Լ� (AnimIns �ҷ����� �� ��������Ʈ ����)
void AMultiPlayerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// AnimationInstance �ҷ�����
	BodyAnim = Cast<UPlayerBody_AnimInstance>(BodyMesh->GetAnimInstance());
	ArmAnim = Cast<UPlayerArm_AnimInstance>(ArmMesh->GetAnimInstance());
	LegAnim = Cast<UPlayerBody_AnimInstance>(GetMesh()->GetAnimInstance());

	// ��������Ʈ�� �Լ��� ���ε� �մϴ�.
	LegAnim->VaultDelegate.BindLambda([this]()->void 
	{ // �Ѿ�� ���߰��ϱ�
		StopVault();
		});
	BodyAnim->VaultDelegate.BindLambda([this]()->void 
	{ // �Ѿ�� ���߰��ϱ�
		StopVault();
		});
	LegAnim->ClimbDelegate.BindLambda([this]()->void 
	{ // �ö󰡱� ���߰��ϱ�
		StopClimb();
		});
	BodyAnim->ClimbDelegate.BindLambda([this]()->void 
	{ // �ö󰡱� ���߰��ϱ�
		StopClimb();
		});
	CountDownEndCheck.BindLambda([this]()->void 
	{ // ù ī��Ʈ�ٿ��� ������ �÷��̾ �����̰� �մϴ�.
		Server_SendPlayerStart();
		});
}

// ������, ī�޶�, ź �߻�, �ݵ�, ���º� Tick ó��, ���ڼ� ȿ������ ó���մϴ�.
void AMultiPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/*����� ����(������ Ŭ���̾�Ʈ �Ѵ� �۵�)***************************************************************/

	// �÷��̾� ������
	PlayerMove();

	// ���º� Update�۵�
	DownState->StateUpdate(this);
	UpperState->StateUpdate(this);

	// Ư�� ���·� �����Ҷ� //
	// �����̵����� �ɱ� ���·� ���ؾ� �Ҷ�
	if (DownState->GetEState() == EPlayerDownState::SLIDING && GetVelocity().Size() < 80.0f)
	{
		DownPress(DownState->ChangeState(EPlayerDownState::CROUCH));
	}
	// �ٴ� ���¿��� �ӵ��� �ٸ� ���ֱ� ���·� ���ؾ� �Ҷ�
	if (GetVelocity().Size() < 0.3f && DownState->GetEState() == EPlayerDownState::SPLINT)
	{
		DownPress(DownState->ChangeState(EPlayerDownState::STANDING));
	}

	// ������ Fall�� ���� ¢�� ���� ���� �� �Դϴ�.
	if (IsJumped && GetMovementComponent()->IsFalling() == false) IsJumped = false;

	// ��Ʈ ���׼�
	if (IsHitReactionEnd && HitReactionAlpha != 0.0f)
	{
		HitReactionAlpha = FMath::FInterpTo(HitReactionAlpha, 0.0f, DeltaTime, 8.0f);
		BodyMesh->SetAllBodiesBelowPhysicsBlendWeight("spine_01", HitReactionAlpha, false, true);
	}
	if (IsHitReactionEnd && HitReactionAlpha == 0.0f && !IsDead)
	{
		IsHitReactionEnd = false;
		BodyMesh->SetAllBodiesPhysicsBlendWeight(0.0f, false); // Timeline���� �ٲ㼭 �̰� Finish�� �ֱ� ?
		BodyMesh->SetAllBodiesSimulatePhysics(false);
		//UE_LOG(LogTemp, Warning, TEXT("IsHitReactionEnd: End"));
	}

	// ź �߻�
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

	/*��Ʈ�ѷ��� �����Ҷ��� �۵�***************************************************************************/
	if (IsLocallyControlled()) {

		// ī�޶� ��ġ ���� (�� ���¸��� Ȥ�� ����, FootIK�� ���� Ư���� ��Ȳ���� ī�޶���ġ�� �����Ǵ� �κ��Դϴ�.)
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

		// ���ڼ� ���
		CrossHairCheck();
		if (HUD && EquipWeapone && !IsFire) 
		{
			HUD->SetCrossHairSpread(FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 330.0f), FVector2D(0.0f, EquipWeapone->GetWalkSpreadSize()), GetVelocity().Size()), false, IsFire);
		}

		// �ݵ� ���
		RecoilCheck();

		// ��ȣ�ۿ� ��� (��ȣ�ۿ� �������� �˻��մϴ�.)
		InteractionCheck();

		// ü���� ���� �� ī�޶� ȭ�� ��Ӱ� �ϴ� ���
		ATwoVersus_PlayerState* playerstate = Cast<ATwoVersus_PlayerState>(GetPlayerState());
		if (playerstate && playerstate->GetPlyaerHP() > 40.0f) 
		{
			FollowCamera->PostProcessBlendWeight = 0.0f;
		}
	}
	/**************************************************************************************************/

	/*������ Ŭ���̾�Ʈ***********************************************************************************/ 
	if (HasAuthority()) 
	{ // ���������� �۵� /////////////////////////////////////////////////////////////////////////////////
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
	{ // Ŭ���̾�Ʈ������ �۵�  ///////////////////////////////////////////////////////////////////////////
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

// �Է� ���ε��� �ϴ� �Լ�
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

// �� ���Ͱ� ���� �� �۵��Ǵ� �Լ�
void AMultiPlayerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

/*���ڼ��� ���õ� ó���� �ϴ� �Լ��Դϴ�.
* �� �Լ������� ī�޶��� ForwardVector�� �������� Trace�� �߻��Ͽ�
* ���ڼ��� ��Ʈ�Ǵ� Actor(��, ���� ��)�� �Ÿ�, ��ġ���� ���Ͽ�
* ź ������ �󸶳� �̷������ �ϴ��� ���������� �����Ͽ� HUD�� ǥ���ϴ� ����� �����ϰ� �ֽ��ϴ�.
*
* ***���߿��� ��������� �̿��غ���***
*
*/
bool AMultiPlayerBase::CrossHairCheck()
{
	FHitResult OutHit;                             // Trace�� ��Ʈ�� ������� ��� ����ü�Դϴ�.
	TArray<AActor*> ActorsToIgnore;
	FVector StartTracer = FollowCamera->GetComponentLocation();                   // Trace ��������
	FVector EndTracer = StartTracer + FollowCamera->GetForwardVector() * 2500.0f; // Trace ������ ����
	FVector MuzzleLoc = FVector::ZeroVector;       // �ѱ��� ��ġ
	FVector BulletEndLoc = FVector::ZeroVector;    // ź������ ���� �����Ǵ� �������� �Ѿ��� ��ġ
	FVector CrossHairEndLoc = FVector::ZeroVector; // ���ڼ��� ��Ʈ�� ��ġ
	float Distance = 0.0f;                         // ���ڼ��� ��Ʈ�� ��ġ�� �÷��̾�� �Ÿ�

	// Trace
	bool hitis = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartTracer, EndTracer, ETraceTypeQuery::TraceTypeQuery4, false
		, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	// �ѱ��� ��ġ�� ã���ϴ�.
	if (EquipWeapone != nullptr)
	{
		MuzzleLoc = EquipWeapone->BodyMesh->GetSocketLocation("Muzzle");
	}
	else { MuzzleLoc = FollowCamera->GetComponentLocation(); }


	bool RedCheck = false;
	if (hitis && OutHit.GetActor() != this)
	{ // ��Ʈ�� ���Ͱ� �ڱ��ڽ��� �ƴϸ�
		CrossHairEndLoc = OutHit.Location; // ���ڼ��� �������� ��ġ
		Distance = OutHit.Distance;        // �������� �Ÿ�
		if (OutHit.GetComponent()->GetCollisionProfileName() == "EBodyMesh")
		{ // ��Ʈ�� ������Ʈ�� ���̸�
			RedCheck = true; // ���ڼ��� ���������� �ٲٱ�
		}
	}
	else { // ��Ʈ�� �ȵǸ�
		CrossHairEndLoc = EndTracer;
		Distance = 2000.0f;
	}
	//FMath::Atan2()
	/* ź������ ����
	* ��Ʈ�� �Ÿ��� ���� ������ ���մϴ�.
	* ī�޶��� FOV(�þ߰�)�� �̿��ؼ� �þ߰��� ��Ʈ�Ÿ����� ���̸� ���Ͽ� �� ���� ��ŭ ź�� ��Ʈ���ϴ�.*/
	float Radius = (FMath::Tan(FollowCamera->FieldOfView / 2.0f) * Distance) / CurrentSpreadSize;
	float BulletEndLocX = FMath::RandRange((CrossHairEndLoc + (FollowCamera->GetForwardVector() * Radius)).X,
		(CrossHairEndLoc + (-(FollowCamera->GetRightVector()) * Radius)).X);
	float BulletEndLocY = FMath::RandRange((CrossHairEndLoc + (FollowCamera->GetRightVector() * Radius)).Y,
		(CrossHairEndLoc + (-(FollowCamera->GetRightVector()) * Radius)).Y);
	float BulletEndLocZ = FMath::RandRange((CrossHairEndLoc + (FollowCamera->GetUpVector() * Radius)).Z,
		(CrossHairEndLoc + (-(FollowCamera->GetUpVector()) * Radius)).Z);

	// ź������ ���� ������ �������� ź�� ��ġ�Դϴ�.
	BulletFireLoc = FVector(BulletEndLocX, BulletEndLocY, BulletEndLocZ);

	// ���ڼ��� ���� �����մϴ�.
	if (RedCheck != IsCrossHairRed && HUD)
	{ // �׻� �ٲ�� �ϴ� ���� �ƴ� ������ �ִ� ������(IsCrossHairRed)�� �ٸ��� �۵��ǰ� ��������ϴ�.
		IsCrossHairRed = RedCheck;
		HUD->CrossHairRedCheck.Execute(IsCrossHairRed);
	}
	return hitis;
}

/*�ݵ��� ���õ� ó���� �ϴ��Լ��Դϴ�.
* Trace�� �߻��ؼ� ��Ʈ�� �Ÿ��� ���� ź������ ���Ǹ�
* �ݵ� ���Ŀ� ����� ���۵� �������� �ǵ��� ���� ����� �ֽ��ϴ�.
*/
bool AMultiPlayerBase::RecoilCheck()
{
	// Trace �κ��� ���� �۵��Ǵ� ���θ� ��Ÿ���� ���ؼ��� �����ϴ� �����Դϴ�. 
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

	// ���� �Ϲ��������� �ǵ��ƿ;��ϴ����� �˷��ݴϴ�.
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

// ��ȣ�ۿ��� ������ Actor������ Ȯ���ϴ� �Լ��Դϴ�.
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
		{ // �ٶ󺸴� Actor�� ������ 
			LookWeapone = Cast<AWeaponeBase>(outHit.Actor);
			if (LookWeapone->GetWeaponeState() == WEAPONSTATE::DROP && LookWeapone->GetIsEmpty() == false) 
			{  
				// ������ UI�� On�մϴ�. (Press F)
				LookWeapone->Execute_InteractionStart(LookWeapone);
			}
		}
		else { return hitis; }
	}
	else {
		if (LookWeapone != nullptr) 
		{
			// ������ UI�� Off�մϴ�. (Press F)
			LookWeapone->Execute_InteractionEnd(LookWeapone);
			LookWeapone = nullptr;
		}
	}

	return hitis;
}

// ���� ��� �ִ� ������� Ȯ���ؼ� �������� ������ �����մϴ�.
void AMultiPlayerBase::WeaponCheck(AWeaponeBase* check)
{
	if (check != nullptr) {
		if (BackWeapone1 != nullptr && BackWeapone2 != nullptr) 
		{ // ���Ⱑ �ΰ� ���� ��
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
			{ // ���Ⱑ �ϳ� ���� ��
				/*if (EquipWeapone->GetStaticClass() != check->GetStaticClass())
				{
					
				}
				else 
				{ // �ֿ���� ���Ⱑ ���� ���� ź�� �� ���ϱ� 

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
			{ // ���Ⱑ ���� ��
				BackWeapone1 = check;
			}
		}

		// ź�� ����� �մϴ�.
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

// ���⸦ �ٲٴ� �Լ��Դϴ�.
// ��� �ִ� ���⸦ ������ ������ � �ִ� ���⸦ ������ �ű�ϴ�.
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

// ���� ����ִ� ���⸦ �Ű������� �ٲٴ� �Լ��Դϴ�.
void AMultiPlayerBase::EquipGunOnHand(AWeaponeBase* equip)
{
	EquipWeapone = equip;
	EquipWeapone->SetWeaponeState(WEAPONSTATE::EQUIP);
	EquipWeapone->SetOwner(this); // ���� ���� ���� ����
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
		EquipWeaponeArm->SetOwner(this); // ���� ���� ���� ����
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

// ���� �ð����� ���縦 �� �� �ְ� Ÿ�̸Ӹ� �����߰� �� Ÿ�̸Ӱ� ������ �۵��Ǵ� �Լ��Դϴ�.
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
			IsFire = false;                     // �������� üũ
			IsFireAuto = false;                 // ���߻���� ����
		}
	}
}

// �Է¿� ���� ���� ���� ���� ĳ���͸� �̵���Ű�� �Լ��Դϴ�.
void AMultiPlayerBase::PlayerMove()
{
	if (IsMove)
	{
		if (MoveDir.IsZero()) 
		{ // �����̴� ������ ���ٸ�
			return;
		}
		MoveDir.Normalize();

		// Input�� ���� �����̴� �� ����
		float InputForce = 0.0f;
		if (FMath::Abs(InputDirForward) > FMath::Abs(InputDirRight)) 
		{
			InputForce = FMath::Abs(InputDirForward);
		}
		else { InputForce = FMath::Abs(InputDirRight); }

		//UE_LOG(LogTemp, Warning, TEXT("Input: %f"), FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(0.0f, PlayerSpeed), InputDir));
		
		// ����� ���� ���� ������
		AddMovementInput(MoveDir, FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(0.0f, PlayerSpeed), InputForce) * 0.008f);
		// ��ü ���¿� ���� ������ �߰� ���
		DownState->PlayerMove(this, InputForce, InputDirRight);
	}
}

// �÷��̾ �״� ���·� ����ϴ�.
void AMultiPlayerBase::PlayerDead()
{
	IsMove = false; // �������� ���ϰ� �մϴ�.

	GetWorldTimerManager().ClearTimer(HealTimer);	     // ġ�� Ÿ�̸� Off
	GetWorldTimerManager().ClearTimer(HitReactionTimer); // ���� ȿ�� Ÿ�̸� Off

	// ���⸦ �ٴڿ� ����Ʈ����
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

	// �÷��̾� ����
	// �浹ü �� ���ȴ� �� ���̰� �մϴ�.
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("DeadColl"));
	ArmMesh->SetCollisionProfileName(TEXT("DeadNomal"));
	ArmMesh->HideBoneByName(FName("root"), PBO_None);
	GetMesh()->SetCollisionProfileName(TEXT("DeadNomal"));
	GetMesh()->HideBoneByName(FName("root"), PBO_None);
	BodyMesh->SetCollisionProfileName(TEXT("DeadMesh"));
	BodyMesh->SetOnlyOwnerSee(false);
	BodyMesh->SetOwnerNoSee(false);
	BodyMesh->SetSimulatePhysics(true);

	// ��Ʈ�ѷ����� �˷� ������ GameMode���� �����մϴ�.
	UE_LOG(LogTemp, Warning, TEXT("PlayerDead"));
	ATwoVersus_PlayerController* Ctrl = Cast<ATwoVersus_PlayerController>(GetController());
	if (Ctrl && Ctrl->PlayerDeadCheck.IsBound()) Ctrl->PlayerDeadCheck.Execute();
}

// �÷��̾��� �տ� ���� �ִ����� �ν��ϴ� Trace�� ��� �Լ��Դϴ�.
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

// �νĵ� ���� ���̸� �˱� ���� Trace�� ��� �Լ��Դϴ�.
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

// �νĵ� ���� �� ������ ���̸� �˱� ���� Trace�� ��� �Լ��Դϴ�.
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

// ���� ���¿� ���� �Ѿ�� �� �ö󰡱� �۵�
void AMultiPlayerBase::PlayerVault(FVector Loc, FVector Nomal, FVector Height)
{
	if (BodyAnim == nullptr || GetMesh() == nullptr) {
		return;
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;

	// WallNomal�� �̾Ƴ� ���� ������ Z�� ���� 180�� ȸ���Ͽ� �÷��̾ �ٶ� ���� �������� �ٲ㼭 WallNomalXRotator�����Ѵ�.
	FRotator WallNomalXRotator = FRotator(Nomal.Rotation().Pitch, Nomal.Rotation().Yaw - 180.0f, Nomal.Rotation().Roll);;
	SetActorRotation(FRotator(0.0f, WallNomalXRotator.Yaw, 0.0f));

	ArmMesh->SetOwnerNoSee(true);
	BodyMesh->SetOwnerNoSee(false);
	//IsCameraLock = true;
	if (EquipWeaponeArm) EquipWeaponeArm->SetHidden(true);
	// ���� X,Y ��ǥ�� ���� �÷��̾ ��Ȯ�ϰ� ������(�Ѿ����)��ġ�� ã�´�.
	//���� ��ǥ�� �̾Ƴ��� ��Ȯ�� ��ġ�� �ƴ� ���� �߾� ��ġ�� �̾� �����̴�.
	//�׷��� �Ʒ��� ������ ���� �÷��̾� ������ ���� ��ġ�� ���´�.
	FVector WallLocationXY = Loc + (FRotationMatrix::MakeFromX(Nomal).GetUnitAxis(EAxis::X) * (40.0f));
	// �÷��̾ �Ѿ���� ���� ��Ȯ�� ��ǥ
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
	// �÷��̾��� ��ġ�� �����Ѵ�.
	SetActorLocation(WallLocationTracer);

	// ī�޶� ����
	//SpringArm->AttachToComponent(BodyMesh, FAttachmentTransformRules::KeepRelativeTransform, "CameraLoc");
	//SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

// �Ѿ�⸦ ����ϴ�.
void AMultiPlayerBase::StopVault()
{
	// ���̴� �� ����
	ArmMesh->SetOwnerNoSee(false);
	BodyMesh->SetOwnerNoSee(true);
	BodyAnim->StopMontage();
	LegAnim->StopMontage();

	// ī�޶� ����
	//IsCameraLock = false;
	//SpringArm->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	//SpringArm->SetRelativeLocation(FVector(25.0f, 0.0f, 65.0f));

	if (EquipWeaponeArm) EquipWeaponeArm->SetHidden(false);
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	IsVault = false;
	//UE_LOG(LogTemp, Warning, TEXT("Vault"));
}

// �ö󰡱⸦ ����ϴ�.
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

// ��Ʈ ���׼��� ������ �۵��ϴ� �Լ�(����ǥ���� ������� ���ư��� ��)
void AMultiPlayerBase::HitReactionEnd()
{
	IsHitReactionEnd = true;
	HitReactionAlpha = 0.5f;
}

// �ڵ� ü�� ȸ���� �����ϴ� �Լ��Դϴ�.(����� ī�޶� ������ �����ϰ� ���� ���� �����Դϴ�.)
void AMultiPlayerBase::StratAutoHeal()
{
	UE_LOG(LogTemp, Warning, TEXT("StratAutoHeal"));
	FollowCamera->PostProcessBlendWeight = 0.0f;
	/*ATwoVersus_PlayerState* state = Cast<ATwoVersus_PlayerState>(GetPlayerState());
	if (state) {
		state->StartHeal();
	}*/
}

// �ɱ�(C) Ű�� ���ε� �Լ�
void AMultiPlayerBase::PlayerCrouch()
{
	DownPress(nullptr);
	
	if (!HasAuthority()) 
	{
		Server_SendDownPress(EPlayerPress::CROUCH);
	}
	else { NetMulticast_SendDownPress(EPlayerPress::CROUCH); }
}

// ����(Space) Ű�� ���ε� �Լ�
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

// �޸���(Shift) Ű�� ���ε� �Լ�
void AMultiPlayerBase::PlayerSplint()
{
	DownPress(nullptr);
	if (!HasAuthority()) {
		Server_SendDownPress(EPlayerPress::SPLINT);
	}
	else {  NetMulticast_SendDownPress(EPlayerPress::SPLINT); }
}

// 1�� ���� ����(1) Ű�� ���ε� �Լ�
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

// 2�� ���� ����(2) Ű�� ���ε� �Լ�
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

// ����(3) Ű�� ���ε� �Լ�
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

// ��ȣ�ۿ�(F) Ű�� ���ε� �Լ�
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

// ��������(���콺 ������Ű) Ű�� ���� �� ���ε� �Լ�
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

// ��������(���콺 ������Ű) Ű�� �� �� ���ε� �Լ�
void AMultiPlayerBase::PlayerUnADS()
{
	if (UpperState->GetEState() == EPlayerUpperState::ADS)
	{
		//IsADS = false;
		// �������� �ִϸ��̼��� ��Ÿ�ַ� �÷��� �߱� ������ ��Ÿ�ָ� ���߰� �մϴ�.
		ArmAnim->StopAllMontages(0.1f);

		// ��ü ���¸� �ٲٰ� �մϴ�. (�Է¿� ���� ���°� �ٲ�ϴ�.)
		UpperPress(nullptr);

		// ������ �ٸ� Ŭ���̾�Ʈ�� ������ ĳ���͵� ���¸� �ٲٰ� �ϱ� ���� RPC�� �����մϴ�.
		if (!HasAuthority()) 
		{ // Ŭ���̾�Ʈ �� �� (��������Ƽ�� ����)
			Server_SendUpperPress(EPlayerPress::ADS);
		}
		else 
		{ // ���� �� �� (���� ����)
			NetMulticast_SendUpperPress(EPlayerPress::ADS); 
		}
	}
}

/*���� ��ư�� ������ �� �����ϴ� �Լ��Դϴ�.
*/
void AMultiPlayerBase::PlayerFire()
{
	if (EUpperState == EPlayerUpperState::UNARMED)
	{ // �����̸� ���������� �����մϴ�.
		Server_SendMelee(FollowCamera->GetComponentLocation(), FollowCamera->GetForwardVector());
		ArmAnim->PlayMeleeMontage();
	}
	else 
	{ // ���� �����̸�
		if (EquipWeapone->LoadedAmmo > 0)
		{ // ���� ����ִ� ���� ź�� �����ϸ�
			IsFire = true;                           // ����� üũ
			RecoilReturnRot = FRotator::ZeroRotator; // �ݵ� �ǵ��ƿ��� ���� ���
			CurrentSpreadSize = BaseSpreadSize;
			if (EquipWeapone && EquipWeaponeArm)
			{ // ���� ����ִ� ���Ⱑ ���� �Ѵٸ�
				// �Ѿ��� �߻��մϴ�.
				UpperState->PlayerFire(this, EquipWeapone, IsFireAuto, ThreeCount
					, EquipWeaponeArm->BodyMesh->GetSocketLocation("Muzzle"), FollowCamera->GetComponentRotation(), BulletFireLoc);
			}
		}
		else 
		{ // ź�� �������� �ʴٸ�
			EquipWeapone->EmptyCheck.Execute(); // ź���� ����ִٰ� �˸��� ���� ���带 �����ϴ� �Լ��Դϴ�.
		}
	}
}

/*���� ��ư�� ������ �� �����ϴ� �Լ��Դϴ�.
*/
void AMultiPlayerBase::PlayerUnFire()
{
	if (EUpperState != EPlayerUpperState::UNARMED)
	{ // ���� ���¶��
		ArmAnim->StopFireMontage();         // ��� Anim�� ���ߴ� �Լ��Դϴ�.
		if (EquipWeaponeArm && EquipWeaponeArm->GetWeaponeLever() != WEAPONLEVER::TRIPLE)
		{
			IsFire = false;                     // �������� üũ
			IsFireAuto = false;                 // ���߻���� ����
		}
		CurrentSpreadSize = BaseSpreadSize; 
		if (HUD && EquipWeapone) HUD->SetCrossHairSpread(0.0f, false, false);
	}
}

/*������ư�� �������� �۵��ϴ� �Լ��Դϴ�.
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

// ���帮��(Z) Ű�� ���ε� �Լ�
void AMultiPlayerBase::PlayerProne()
{
	DownPress(nullptr);
	if (!HasAuthority()) {
		Server_SendDownPress(EPlayerPress::PRONE);
	}
	else { NetMulticast_SendDownPress(EPlayerPress::PRONE); }
}

// �׽�Ʈ��
void AMultiPlayerBase::PlayerSelfDead()
{
	//UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), 5.0f, GetActorForwardVector(), OutHit, nullptr, this, UMelee_DamageType::StaticClass());

}

// �׽�Ʈ��
void AMultiPlayerBase::PlayerWeaponLever()
{
	if (EquipWeaponeArm && EquipWeapone)
	{
		EquipWeapone->ChangeWeaponeLever();
		EquipWeaponeArm->ChangeWeaponeLever();

		UE_LOG(LogTemp, Warning, TEXT("WeaponeLever: %d"), (int)(EquipWeapone->GetWeaponeLever()));
	}
}

// ��, �� �̵�Ű(W, S) ���ε� �Լ�
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

// ��, �� �̵�Ű(A, D) ���ε� �Լ�
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

// ��, �� ����Ű(���콺 �̵�) ���ε� �Լ�
void AMultiPlayerBase::TurnAtRate(float Rate)
{
	AimDirRight = Rate;
	if (DownState)
	{
		DownState->TurnAtRate(this, AimDirRight);
	}
	
}

// ��, �� ����Ű(���콺 �̵�) ���ε� �Լ�
void AMultiPlayerBase::LookUpAtRate(float Rate)
{
	AimDirUp = Rate;
	if (DownState)
	{
		DownState->LookUpAtRate(this, Rate);
	}

}

// �������� ������ � ������ �ؾ��ϴ����� ��Ÿ���� �Լ��Դϴ�.
float AMultiPlayerBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("TakeDamage"));
	//ApplyPointDamage�� ���� ���� ������
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	float CurrentHP = 0.0f; // �������� ���� ������ ü��
	if (!IsDead)
	{ // ���� �ʾҴٸ�
		ATwoVersus_PlayerState* State = Cast<ATwoVersus_PlayerState>(GetPlayerState());

		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{ // DamageEvent�� FPointDamageEvent ����ü�� �̷�����ִ��� Ȯ��
			const FPointDamageEvent* Point = (FPointDamageEvent*)&DamageEvent;
			Damage = Point->Damage; // ApplyPointDamage�� ���� ���� ������
			CurrentHP = State->GetPlyaerHP() - Damage; // ������ ü�¿��� �������� ����
			State->DamageToHP(Damage); // PlayerState�� �������� �����Ͽ� ������ ü���� ������Ʈ�մϴ�.

			UE_LOG(LogTemp, Warning, TEXT("PlayerHP: %f"), CurrentHP);

			// ������ Ÿ�Կ� ���� ȿ��ǥ��(����, ��ƼŬ)
			// ������ �Ѵ� ������ ȿ���� ǥ���Ǿ��ֽ��ϴ�.
			FString DamageName = DamageEvent.DamageTypeClass->GetFName().ToString(); // DamageType�� �̸��� ������ �����մϴ�,
			if (DamageName == "Bullet_DamageType") // ź�� ���� ������
			{
				// ��ƼŬ ǥ��
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitDecal, Point->HitInfo.Location);
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BulletHitHitSound, Point->HitInfo.Location);
			}
			else if (DamageName == "Melee_DamageType") // ���� ���ݿ� ���� ������
			{
				// ��ƼŬ ǥ��
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitDecal, Point->HitInfo.Location);
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), MeleeHitHitSound, Point->HitInfo.Location);
			}


			AMultiPlayer_HUD* Hud = Cast<AMultiPlayer_HUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

			// �÷��̾��� ��ܿ� �ִ� ��ü ü���� ������Ʈ �ϴ� �� �Դϴ�.
			// ù��° ��Ʈ�ѷ��� hud = ��Ʈǥ�� -> ������ ����� , ������ġ ǥ�� -> ������ ���ϴ� �÷��̾�
			if (GetWorld()->GetFirstPlayerController()->GetPawn() && Hud)
			{
				ATwoVersus_PlayerController* Ctrl = Cast<ATwoVersus_PlayerController>(GetWorld()->GetFirstPlayerController());
				if (Ctrl) Ctrl->UpdateMatchHPCheck.Execute();
			}

			// Ÿ�ݿ� �������� �� ���� ĳ���Ͱ� ������ Ÿ�� ���� ���� �������� �˴ϴ�.
			// ��Ʈ ǥ�ø� �Ҷ� ���� ��Ʈ���� ���� �� �� ���� (�÷��̾ �ƴϰ� ������ ����ڰ� �÷��̾�� �� �÷��̾��� hud�� �����Ѵٸ�)
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

			// ������ ���� ǥ��, Ÿ�� ���� ǥ��
			if (IsLocallyControlled() && Hud)
			{ // �÷��̾ ���濡 ���� �������� ������ ������ ��ġ�� ���� ������ ������ ǥ���մϴ�.
				Hud->PlyaerHitLocCheck.Execute(DamageCauser->GetActorLocation() + DamageCauser->GetActorForwardVector() * 50.0f);
				/*ATwoVersus_PlayerController* Ctrl = Cast<ATwoVersus_PlayerController>(GetController());
				Ctrl->UpdateMatchHPCheck.Execute();*/
			}
			else
			{ // Ÿ�� ���� ǥ��
				// Ÿ�� ���� ǥ�� ������ �ð��� �ʱ�ȭ�մϴ�.
				if (GetWorld()->GetTimerManager().IsTimerActive(HitReactionTimer)) GetWorld()->GetTimerManager().ClearTimer(HitReactionTimer);
				IsHitReactionEnd = false;
				HitReactionAlpha = 0.0f;
				// Ÿ�� ���� ǥ�� �ʱ�ȭ
				BodyMesh->SetAllBodiesPhysicsBlendWeight(0.0f, false);
				BodyMesh->SetAllBodiesSimulatePhysics(false);

				BodyMesh->SetAllBodiesBelowSimulatePhysics("spine_01", true, true);			  // ô�߸� �������� ���� ���� ���� ȿ���� On�մϴ�.
				BodyMesh->SetAllBodiesBelowPhysicsBlendWeight("spine_01", 0.5f, false, true); // 0.5������ ���� ȿ���� ŵ�ϴ�.
				// ���� �������� ���� ���� ���� ȿ���� Off�մϴ�.
				BodyMesh->SetAllBodiesBelowSimulatePhysics("lowerarm_l", false, true);
				BodyMesh->SetAllBodiesBelowSimulatePhysics("lowerarm_r", false, true);

				// �������� ���� �������� ô�߿� ���� ���մϴ�.
				BodyMesh->AddForceToAllBodiesBelow(DamageCauser->GetActorForwardVector() * 500000.0f, "spine_01"); //500000.0f

				// Ÿ�� ���� ǥ�� ������ �ð��� ���մϴ�.
				GetWorld()->GetTimerManager().SetTimer(HitReactionTimer, this, &AMultiPlayerBase::HitReactionEnd, 1.0f, false);
			}
			if (HasAuthority()) {
				// �ڵ� �� ���(����� ���� ������ ������ ��� ���� �ʽ��ϴ�.)
				//PlayerState->StopHeal();
				//GetWorldTimerManager().ClearTimer(HealTimer);
				//GetWorldTimerManager().SetTimer(HealTimer, this, &AMultiPlayerBase::StratAutoHeal, 4.0f, false);
				// �������� �� Ŭ���̾�Ʈ�� �����մϴ�.
				NetMulticast_SendPlayerHit(Damage, DamageCauser->GetActorForwardVector(), Point->HitInfo, DamageCauser, DamageEvent.DamageTypeClass);
			}
			// ü���� ���� ��ġ(40)�� �Ǹ� ī�޶� ȸ�� ������ ���մϴ�. 
			CheckPlayerHP(CurrentHP);
		}
	}	return Damage;
}

// ��ü�� ���õ� Ű�� ������ �� ���°� ���ϴ� �Լ��Դϴ�.
// state �Ű� ������ ������ ���°� ���� �� �� ����� �˴ϴ�.
// ����� ���� ���� nullptr�� �ֽ��ϴ�.
void AMultiPlayerBase::UpperPress(UPlayerUpperStateBase* state)
{
	UPlayerUpperStateBase* temp = nullptr;
	if (!state)
	{ // ������ ��ȯ�� �� ���°� ���ٸ�
		// �Է¿� ���� ���� ���� �ҷ� �ɴϴ�.
		temp = UpperState->HandleInput(this);
		if (temp == nullptr) { return; }
	}
	else 
	{ // ������ ��ȯ�� �� ���·� ��ȯ�մϴ�.
		temp = state;
	}

	// ������ ���¸� �����ϴ�.
	UpperState->StateEnd(this);
	// �� �� ���� (������ ����)�� �����մϴ�.(�� ������ �� ���¸� ��Ÿ���ϴ�.)
	UpperStateBClass = UpperState->GetState();

	if (temp != nullptr) 
	{
		// ���� ���¸� �ı��մϴ�.
		UpperState->DestroyComponent();

		// �Է¿� Ȥ�� �Ű� ������ ������ ���·� �����մϴ�.
		UpperState = temp;

		// ���� ���¸� �����մϴ�.
		UpperStateNClass = UpperState->GetState(); 
		EUpperState = UpperState->GetEState();
		if (UpperState->GetIsChangeState() == false)
		{ // ���� �ٲ�� ������ ���� ���¶�� ���¸� �ٷ� �����մϴ�.
			UpperState->StateStart(this);
		}
	}
}

// ��ü�� ���õ� Ű�� ������ �� ���°� ���ϴ� �Լ��Դϴ�.
void AMultiPlayerBase::DownPress(UPlayerDownStateBase* state)
{
	UPlayerDownStateBase* temp = nullptr;
	float ChangeSpeed = 0.0f;

	if (!state) {
		temp = DownState->HandleInput(this); // �Էµ� Ű�� ���� ���� ��ü ��ȯ
		if (temp == nullptr) { return; }
	}
	else { temp = state; }

	DownState->StateEnd(this); // ���� ���� ������ �Լ��� �۵��մϴ�.
	DownStateBClass = DownState->GetState();

	if (temp != nullptr) {
		if (DownState->GetIsChangeState())
		{ // �ٲ�� ������ �ִ� �����̸�
			temp->SetIsChangeState(true); // �ٸ� ���·� �ٲ𶧵� ������ �����Ƿ� ������ �Ѵ�.
			ChangeSpeed = DownState->GetChangeSpeed(); // �ٸ� ���·� �ٲ��� �ӵ�
		}

		DownState->DestroyComponent(); // ���� ���¸� �޸� ���� �մϴ�.
		DownState = temp; // ���� ���¸� HandleInput���� ��ȯ�� ���� ��ü�� �����մϴ�.
		DownStateNClass = DownState->GetState();
		EDownState = DownState->GetEState();

		if (DownState->GetIsChangeState() == false)
		{
			DownState->StateStart(this); // ���°� �ٲ�����Ƿ� �ٲ� ���¿� ���� ���� �Լ��� �۵� ��ŵ�ϴ�.
		}
		else {
			DownState->StateChangeStart(this, ChangeSpeed);
		}
	}
}

/*���³��� ���ϰ� �ִ� ��Ȳ���� ���ϴ°� ������
* (�ִϸ��̼��� ������ AnimNotify�� �ؼ� �������̽��� �÷��̾�� ���ϴ� �ִϸ��̼��� �������� StateStart�� �϶�� �˷��ݴϴ�.)
* StateStart�� �۵��ϴ� �Լ��Դϴ�.
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

// �׾����� ü���� ������(ü���� 40���ϸ� ȭ���� ȸ���� �����Ѵ�.)�� �Ǵ��ϴ� �Լ��Դϴ�.
void AMultiPlayerBase::CheckPlayerHP(float hp)
{
	if (hp <= 40.0f && hp > 0 && FollowCamera->PostProcessBlendWeight != 1.0f) 
	{ // ä���� 40 ���ϸ� ī�޶� ȸ������ ���մϴ�.
		UE_LOG(LogTemp, Warning, TEXT("Gray"));
		FollowCamera->PostProcessBlendWeight = 1.0f;
		GetWorldTimerManager().ClearTimer(HealTimer);
		GetWorldTimerManager().SetTimer(HealTimer, this, &AMultiPlayerBase::StratAutoHeal, 4.0f, false);
	}
	else if (hp <= 0.0f)
	{ // ü���� ������ ĳ���͸� ���� ���·� ����ϴ�.
		UE_LOG(LogTemp, Warning, TEXT("Dead"));
		FollowCamera->PostProcessBlendWeight = 1.0f;
		IsDead = true;
		PlayerDead();
	}
}

// ���� ����ִ� ������ ����ϴ� �Լ��Դϴ�.
void AMultiPlayerBase::FireBullet(FVector MuzzleLoc, FRotator MuzzleRot, FVector BulletLoc)
{
	if (EquipWeapone)
	{
		// �߻� �ִϸ��̼��� �����մϴ�.
		ArmAnim->PlayFireMontage();
		EquipWeapone->PlayFireMontage();
		EquipWeaponeArm->PlayFireMontage();

		ATwoVersus_PlayerState* State = Cast<ATwoVersus_PlayerState>(GetPlayerState());
		// ź ��� �� PlayerState�� �����մϴ�.
		if (EquipWeapone->LoadedAmmo > 0)
		{
			EquipWeapone->LoadedAmmo -= 1;
			State->LoadedAmmo -= 1;
		}else 
		{ 
			IsFire = false;
		} // empty ����

		// UI�� ź�� ǥ���մϴ�.
		if (HUD) HUD->LoadedAmmoCheck.Execute(EquipWeapone->LoadedAmmo);
		/*if (EquipWeaponInAmmo > 1.0f) {
			EquipWeaponInAmmo -= 1.0f;
		}
		else { equipWeapone->PlayEmptyMontage(); }*/

		// ���ӵ� �߻縦 ���� ���� ��� ���Ӵϴ�.
		// ���� Ÿ�̸ӿ� ���� �ٽ� ������ ���� �ð� �������� ź�� ���������� ������ �մϴ�.
		IsFireAuto = false;

		// �ݵ��� ���� ī�޶� ���� �ö󰡸� ó���� ź�� �߻� �Ǿ��� �������� �ǵ��ƿ��� �մϴ�.
		// ó���� ź�� �߻� �Ǿ��� ������ ���⼭ �����մϴ�.
		RecoilReturnLoc = FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * 2500.0f;
		if (RecoilReturnRot == FRotator::ZeroRotator) {
			RecoilReturnRot = GetControlRotation();
		}

		// �ݵ��� ���ϴ�.
		// �� �Ѹ��� �ݵ��� ũ�Ⱑ �ٸ��ϴ�.
		float AddPitch = EquipWeapone->GetFireRecoilPitch();
		float AddYaw = EquipWeapone->GetFireRecoilYaw();
		AddControllerPitchInput(AddPitch);
		AddControllerYawInput(AddYaw);

		// ������ ź ������ ���� ź�� �����ϴ�.
		FRotator BulletFireRot = UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, BulletLoc);
		/*if (UpperState->GetEState() == EPlayerUpperState::ADS)
		{ // ���� ���� ���¶�� ź�� ���������� ������ �մϴ�.
			BulletFireRot = ArmMesh->GetRightVector().Rotation();
		} // ���� ���¶�� ź ������ ���� ������ ��ġ�� �߻�ǰ� �մϴ�.
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
		{ // ������ �ܹ��� �ƴϸ� �ڵ� �߻縦 On
			GetWorldTimerManager().SetTimer(FireTimer, this, &AMultiPlayerBase::FireAutoOn, 0.15f, false);
		}

		// ������ Ŭ���̾�Ʈ�� ����
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

		DownState->StateEnd(this); // ���� ���� ������ �Լ��� �۵��մϴ�.
		DownStateBClass = DownState->GetState();

		if (temp != nullptr) {
			if (DownState->GetIsChangeState())
			{ // �ٲ�� ������ �ִ� �����̸�
				temp->SetIsChangeState(true); // �ٸ� ���·� �ٲ𶧵� ������ �����Ƿ� ������ �Ѵ�.
				ChangeSpeed = DownState->GetChangeSpeed(); // �ٸ� ���·� �ٲ��� �ӵ�
			}

			DownState->DestroyComponent(); // ���� ���¸� �޸� ���� �մϴ�.
			DownState = temp; // ���� ���¸� HandleInput���� ��ȯ�� ���� ��ü�� �����մϴ�.
			DownStateNClass = DownState->GetState();
			EDownState = DownState->GetEState();

			if (DownState->GetIsChangeState() == false)
			{
				DownState->StateStart(this); // ���°� �ٲ�����Ƿ� �ٲ� ���¿� ���� ���� �Լ��� �۵� ��ŵ�ϴ�.
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
