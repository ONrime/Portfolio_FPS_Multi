// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Weapone/Public/Rifle/AK_Weapone.h"
#include "Dalos/Projectile/Public/Rifle_ProjectileBase.h"

AAK_Weapone::AAK_Weapone()
{
	PrimaryActorTick.bCanEverTick = false;
	BodyMesh->SetIsReplicated(true);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	BodyMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	BodyMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//BodyMesh->SetCollisionProfileName(TEXT("Item"));

	static ConstructorHelpers::FClassFinder<UAnimInstance>GUN_ANIM(TEXT("AnimBlueprint'/Game/FPS_Weapon_Bundle/Animation/KA74U/KA74U_X_AnimBP.KA74U_X_AnimBP_C'"));
	if (GUN_ANIM.Succeeded()) {
		BodyMesh->SetAnimInstanceClass(GUN_ANIM.Class);
	}
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SKELETALGUN(TEXT("SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/KA74U/SK_KA74U_X.SK_KA74U_X'"));
	if (SKELETALGUN.Succeeded()) {
		BodyMesh->SetSkeletalMesh(SKELETALGUN.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage>FIRE_MONTAGE(TEXT("AnimMontage'/Game/FPS_Weapon_Bundle/Animation/KA74U/KA74U_X_Fire_Montage.KA74U_X_Fire_Montage'"));
	if (FIRE_MONTAGE.Succeeded()) {
		fireMontage = FIRE_MONTAGE.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage>RELOAD_MONTAGE(TEXT("AnimMontage'/Game/FPS_Weapon_Bundle/Animation/KA74U/KA74U_X_Reload_Montage.KA74U_X_Reload_Montage'"));
	if (RELOAD_MONTAGE.Succeeded()) {
		reloadMontage = RELOAD_MONTAGE.Object;
	}
	/*static ConstructorHelpers::FObjectFinder<UAnimMontage>EMPTY_MONTAGE(TEXT("AnimMontage'/Game/IBWP3DPolicePistol/Animations/Weapons/Pistol_Empty_Montage.Pistol_Empty_Montage'"));
	if (EMPTY_MONTAGE.Succeeded()) {
		EmptyMontage = EMPTY_MONTAGE.Object;
	}*/
	SetWeaponeState(WEAPONSTATE::DROP);
	SetWeaponeLever(WEAPONLEVER::FULLAUTO);
	/*CrossHairRadius = 1.0f;
	//InAmmo = FMath::FRandRange(2.0f, 5.0f);
	MaxInAmmo = 7.0f;
	InAmmo = MaxInAmmo;*/
	walkSpreadSize = 250.0f;
	fireStartSpreadSize = 240.0f;
	fireEndSpreadSize = 270.0f;
	standSpreadSize = 0.0f;

	WeaponName = "AK";
	baseKeepAmmo = 60;
	baseLoadedAmmo = 30;
	LoadedAmmo = baseLoadedAmmo;
	limitAmmo = 90;

	WeaponType = WEAPONTYPE::RIFLE;
	for (int i = 0; i < WeaponeLeverCheck.Num(); i++) WeaponeLeverCheck[i] = true;
}

void AAK_Weapone::BeginPlay()
{
	Super::BeginPlay();
	SetWeaponeState(WEAPONSTATE::DROP);
	StateStart(GetWeaponeState());
}

UClass* AAK_Weapone::GetStaticClass()
{
	return AAK_Weapone::StaticClass();
}

AWeaponeBase* AAK_Weapone::SpawnToHand(APawn* owner, FVector loc, FRotator rot)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnToHand"));
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = owner;
	spawnParameters.Instigator = GetInstigator();

	AAK_Weapone* weapone = GetWorld()->SpawnActor<AAK_Weapone>(AAK_Weapone::StaticClass(), loc, rot, spawnParameters);
	return weapone;
}

void AAK_Weapone::ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot)
{
	FActorSpawnParameters spawnParameter;
	spawnParameter.Owner = GetOwner();
	//spawnParameter.Instigator = GetInstigator();

	auto projectile = GetWorld()->SpawnActor<ARifle_ProjectileBase>(ARifle_ProjectileBase::StaticClass(), loc, rot, spawnParameter);
	if (projectile) {
		projectile->SetProjectileVelocity(8000.0f);
		projectile->ProjectileFire(bulletRot.Vector(), GetOwner());
	}
}

float AAK_Weapone::GetFireRecoilPitch()
{
	return FMath::RandRange(-0.2f, -0.5f);
}
float AAK_Weapone::GetFireRecoilYaw()
{
	return FMath::RandRange(-0.2f, 0.2f);
}

void AAK_Weapone::PlayFireMontage()
{
	BodyMesh->GetAnimInstance()->Montage_Play(fireMontage);
}

void AAK_Weapone::StopFireMontage()
{
	BodyMesh->GetAnimInstance()->Montage_Stop(0.1f);
}

void AAK_Weapone::PlayReloadMontage()
{
	BodyMesh->GetAnimInstance()->Montage_Play(reloadMontage);
}

void AAK_Weapone::StopReloadMontage()
{
	BodyMesh->GetAnimInstance()->Montage_Stop(0.1f);
}
