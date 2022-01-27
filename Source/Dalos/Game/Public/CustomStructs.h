// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomStructs.generated.h"

/**
 * 
 */
class DALOS_API CustomStructs
{
public:
	CustomStructs();
	~CustomStructs();
};

USTRUCT(Atomic, BlueprintType)
struct FPlayerInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int settingID = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString playerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* playerImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ACharacter* playerCharacter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* playerCharacterImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString playerStatus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString playerReadyStatus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString playerTeamStatus;

	//FPlayerInfo(const FPlayerInfo& Copy) : settingID(Copy.settingID), playerName(Copy.playerName), playerImage(Copy.playerImage), playerCharacter(Copy.playerCharacter), playerCharacterImage(Copy.playerCharacterImage), playerStatus(Copy.playerStatus), playerReadyStatus(Copy.playerReadyStatus), playerTeamStatus(Copy.playerTeamStatus)
	/*FPlayerInfo& operator=(const FPlayerInfo& p)
	{
		this->settingID = p.settingID;
		this->playerName = p.playerName;
		this->playerImage = p.playerImage;
		this->playerCharacter = p.playerCharacter;
		this->playerCharacterImage = p.playerCharacterImage;
		this->playerStatus = p.playerStatus;
		this->playerReadyStatus = p.playerReadyStatus;
		this->playerTeamStatus = p.playerTeamStatus;
	}*/
	bool operator==(const FPlayerInfo& p) const
	{
		if (this->settingID == p.settingID && this->playerName == p.playerName && this->playerImage == p.playerImage
			&& this->playerCharacter == p.playerCharacter && this->playerCharacterImage == p.playerCharacterImage
			&& this->playerStatus == p.playerStatus && this->playerReadyStatus == p.playerReadyStatus && this->playerTeamStatus == p.playerTeamStatus)
		{
			return true;
		}
		else {
			return false;
		}
	}
};

USTRUCT(Atomic, BlueprintType)
struct FGameSetting
{
	GENERATED_USTRUCT_BODY();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MapName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GameModeName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MatchCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MatchTime = 0;
	
};

USTRUCT(Atomic, BlueprintType)
struct FPlayerTeamInfo
{
	GENERATED_USTRUCT_BODY()

	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TeamName;
};

USTRUCT(Atomic, BlueprintType)
struct FVelocityBlend
{
	GENERATED_USTRUCT_BODY()

	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float front;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float back;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float right;
};
