// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Public/Player/PlayerState/ChangeState_AnimNotify.h"
#include "Interface/Public/ChangeState_Interface.h"

UChangeState_AnimNotify::UChangeState_AnimNotify()
{

}

void UChangeState_AnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	UClass* ac = MeshComp->GetOwner()->GetClass();
	if (ac->ImplementsInterface(UChangeState_Interface::StaticClass())) {
		//UE_LOG(LogTemp, Warning, TEXT("UChangeState_AnimNotify"));
		IChangeState_Interface* Character = Cast<IChangeState_Interface>(MeshComp->GetOwner());
		if(Character) Character->Execute_CurrentStateStart(MeshComp->GetOwner(), IsUpper);
	}
}
