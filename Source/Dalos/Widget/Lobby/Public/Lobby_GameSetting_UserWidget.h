// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dalos/Game/Public/CustomStructs.h"
#include "Lobby_GameSetting_UserWidget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class GAMEMODESETTING : uint8 {
	TWOVERSUS,
	TEAMDEATHMACH
};

UCLASS()
class DALOS_API ULobby_GameSetting_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetting")
	FGameSetting GameSetting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetting")
	FGameSetting BackupGameSetting;

	UFUNCTION(BlueprintCallable)
	void SetGameSetting(FGameSetting Set);
	UFUNCTION(BlueprintCallable)
	void ClickAcceptButton();
	UFUNCTION(BlueprintCallable)
	void DestroySessionAndLeaveGame();

};
