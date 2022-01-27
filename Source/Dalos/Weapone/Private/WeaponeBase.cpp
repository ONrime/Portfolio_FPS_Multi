// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Weapone/Public/WeaponeBase.h"
#include "Components/WidgetComponent.h"
#include "EngineUtils.h"

// Sets default values
AWeaponeBase::AWeaponeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//SetReplicates(true);
	//SetReplicateMovement(true);

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BODY"));
	RootComponent = BodyMesh;
	BodyMesh->SetIsReplicated(true);

	Sound = CreateDefaultSubobject<UAudioComponent>(TEXT("AUDIO"));
	Sound->SetupAttachment(RootComponent);

	InteractionUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("INTERACTION_UI"));
	InteractionUI->SetupAttachment(RootComponent);
	InteractionUI->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	InteractionUI->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionUI->SetDrawAtDesiredSize(true);
	InteractionUI->SetHiddenInGame(true);
	InteractionUI->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FClassFinder<UUserWidget>INTERACTION_HUD(TEXT("WidgetBlueprint'/Game/UI/Item/Item_Interaction.Item_Interaction_C'"));
	if (INTERACTION_HUD.Succeeded()) { InteractionUI->SetWidgetClass(INTERACTION_HUD.Class); }

	// »ç¿îµå
	static ConstructorHelpers::FObjectFinder<USoundBase>EMPTY_SOUND(TEXT("SoundWave'/Game/Sci-FiSoundPack/Mechanical/wav/Device_Toggle_10.Device_Toggle_10'"));
	if (EMPTY_SOUND.Succeeded()) {
		EmptySound = EMPTY_SOUND.Object;
	}

	WeaponeLever = WEAPONLEVER::SINGLEFIRE;
	WeaponeLeverCheck.Init(false, 3);
}

// Called when the game starts or when spawned
void AWeaponeBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	EmptyCheck.BindLambda([this]()->void {
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), EmptySound, GetActorLocation());
	});
}

void AWeaponeBase::StateStart(WEAPONSTATE state)
{
	switch (state)
	{
	case WEAPONSTATE::EQUIP:
		UE_LOG(LogTemp, Warning, TEXT("WEAPONSTATE::EQUIP"));
		BodyMesh->SetOwnerNoSee(true);
		BodyMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		BodyMesh->SetSimulatePhysics(false);
		BodyMesh->SetCollisionProfileName("NoCollision");
		break;
	case WEAPONSTATE::BACKEQUIP:
		UE_LOG(LogTemp, Warning, TEXT("WEAPONSTATE::BACKEQUIP"));
		BodyMesh->SetOwnerNoSee(false);
		BodyMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		BodyMesh->SetSimulatePhysics(false);
		BodyMesh->SetCollisionProfileName("NoCollision");
		break;
	case WEAPONSTATE::DROP:
		UE_LOG(LogTemp, Warning, TEXT("WEAPONSTATE::DROP"));
		BodyMesh->SetOwnerNoSee(false);
		BodyMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		BodyMesh->SetSimulatePhysics(true);
		BodyMesh->SetCollisionProfileName("ItemObject");
		break;
	case WEAPONSTATE::THROW:
		BodyMesh->SetOwnerNoSee(false);
		BodyMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		BodyMesh->SetSimulatePhysics(true);
		BodyMesh->SetCollisionProfileName("ItemObject");
		break;
	default:
		break;
	}
}

void AWeaponeBase::InteractionStart_Implementation()
{
	InteractionUI->SetHiddenInGame(false);
}
void AWeaponeBase::InteractionUpdate_Implementation()
{
}
void AWeaponeBase::InteractionEnd_Implementation()
{
	InteractionUI->SetHiddenInGame(true);
}

float AWeaponeBase::GetFireRecoilPitch()
{
	return 0.0f;
}
float AWeaponeBase::GetFireRecoilYaw()
{
	return 0.0f;
}

UClass* AWeaponeBase::GetStaticClass()
{
	return nullptr;
}

AWeaponeBase* AWeaponeBase::SpawnToHand(APawn* owner, FVector loc, FRotator rot)
{
	return nullptr;
}

void AWeaponeBase::ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot)
{
}

void AWeaponeBase::PlayFireMontage()
{
}

void AWeaponeBase::StopFireMontage()
{
}

void AWeaponeBase::PlayReloadMontage()
{
}

void AWeaponeBase::StopReloadMontage()
{
}

void AWeaponeBase::SetWeaponeState(WEAPONSTATE set) {
	WeaponeState = set;
	StateStart(WeaponeState);
}

void AWeaponeBase::ChangeWeaponeLever()
{
	int Num = (int)WeaponeLever;
	Num > 1 ? Num = 0 : ++Num;

	WeaponeLever = (WEAPONLEVER)Num;
}

