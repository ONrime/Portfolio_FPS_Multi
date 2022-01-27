// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Stage/Lobby/Public/StartPoint_Actor.h"

// Sets default values
AStartPoint_Actor::AStartPoint_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AStartPoint_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStartPoint_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

