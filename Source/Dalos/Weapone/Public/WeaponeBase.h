// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "Dalos/Interface/Public/Interaction_Interface.h"
#include "WeaponeBase.generated.h"

DECLARE_DELEGATE(FWeaponCheckDelegate)

UENUM(BlueprintType)
enum class WEAPONSTATE : uint8 {
	EQUIP,
	BACKEQUIP,
	DROP,
	THROW
};

UENUM(BlueprintType)
enum class WEAPONLEVER : uint8 {
	FULLAUTO,
	TRIPLE,
	SINGLEFIRE
};

UENUM(BlueprintType)
enum class WEAPONTYPE : uint8 {
	RIFLE,
	PISTOL
};

UCLASS()
class DALOS_API AWeaponeBase : public AActor, public IInteraction_Interface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponeBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Base)
	class USkeletalMeshComponent* BodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* Sound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* InteractionUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int LoadedAmmo = 0;

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	WEAPONSTATE WeaponeState;
	WEAPONLEVER WeaponeLever;
	WEAPONTYPE WeaponType;
	bool IsEmpty = false;
	bool IsReload = false;
	float fireRecoilPitch = 0.0f;
	float fireRecoilYaw = 0.0f;
	UAnimMontage* fireMontage;
	UAnimMontage* reloadMontage;
	float walkSpreadSize = 0.0f;
	float fireStartSpreadSize = 0.0f;
	float fireEndSpreadSize = 0.0f;
	float standSpreadSize = 0.0f;
	int baseKeepAmmo = 0;
	int baseLoadedAmmo = 0;
	int limitAmmo = 0;
	FName WeaponName = "";

	TArray<bool> WeaponeLeverCheck;

	class USoundBase* EmptySound;

	virtual void StateStart(WEAPONSTATE state);

public:	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interation")
	void InteractionStart();
	virtual void InteractionStart_Implementation() override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interation")
	void InteractionUpdate();
	virtual void InteractionUpdate_Implementation() override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interation")
	void InteractionEnd();
	virtual void InteractionEnd_Implementation() override;

	WEAPONSTATE GetWeaponeState() { return WeaponeState; }; void SetWeaponeState(WEAPONSTATE set);
	WEAPONLEVER GetWeaponeLever() { return WeaponeLever; }; void SetWeaponeLever(WEAPONLEVER set) { WeaponeLever = set; };
	void ChangeWeaponeLever();
	WEAPONTYPE GetWeaponType() { return WeaponType; }; void SetWeaponType(WEAPONTYPE set) { WeaponType = set; };
	bool GetIsEmpty() { return IsEmpty; }; void SetIsEmpty(bool set) { IsEmpty = set; };
	bool GetIsReload() { return IsReload; }; void SetIsReload(bool set) { IsReload = set; };
	float GetWalkSpreadSize() { return walkSpreadSize; }
	float GetFireStartSpreadSize() { return fireStartSpreadSize; }
	float GetFireEndSpreadSize() { return fireEndSpreadSize; }
	float GetStandSpreadSize() { return standSpreadSize; }
	virtual float GetFireRecoilPitch(); 
	virtual float GetFireRecoilYaw();
	virtual UClass* GetStaticClass();
	virtual AWeaponeBase* SpawnToHand(APawn* owner, FVector loc, FRotator rot);
	int GetBaseKeepAmmo() { return baseKeepAmmo; }
	int GetBaseLoadedAmmo() { return baseLoadedAmmo; }
	FName GetWeaponName() { return WeaponName; }
	int GetLimitAmmo() { return limitAmmo; }

	FWeaponCheckDelegate EmptyCheck;

	virtual void ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot);
	virtual void PlayFireMontage();
	virtual void StopFireMontage();
	virtual void PlayReloadMontage();
	virtual void StopReloadMontage();

};
