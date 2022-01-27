// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ChangeState_AnimNotify.generated.h"

/**
 * 상태가 변할 때
 * 변하는 일이 끝나면 작동하는 노티파이
 */
UCLASS()
class DALOS_API UChangeState_AnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UChangeState_AnimNotify();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	bool IsUpper = false;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
