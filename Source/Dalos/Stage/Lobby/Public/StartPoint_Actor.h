// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartPoint_Actor.generated.h"

/* �κ�� ��ŸƮ ���� ���� �׽�Ʈ�� �Դϴ�.
*/
UCLASS()
class DALOS_API AStartPoint_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStartPoint_Actor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
