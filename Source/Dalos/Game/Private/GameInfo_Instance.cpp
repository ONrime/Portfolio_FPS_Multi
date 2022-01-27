// Fill out your copyright notice in the Description page of Project Settings.


#include "Dalos/Game/Public/GameInfo_Instance.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Components/SlateWrapperTypes.h"
#include "Dalos/Widget/Public/OptionMenu_UserWidget.h"
#include "Dalos/Widget/Public/FindServer_UserWidget.h"


UGameInfo_Instance::UGameInfo_Instance()
{
	//bReplicates = true;
	static ConstructorHelpers::FClassFinder<UUserWidget> MAINMENU_WIDGET(TEXT("WidgetBlueprint'/Game/UI/MainMenu/MainMenu.MainMenu_C'"));
	if (MAINMENU_WIDGET.Succeeded()) MainMenu_Class = MAINMENU_WIDGET.Class;
	static ConstructorHelpers::FClassFinder<UUserWidget> HOSTMENU_WIDGET(TEXT("WidgetBlueprint'/Game/UI/MainMenu/HostMenu1.HostMenu1_C'"));
	if (HOSTMENU_WIDGET.Succeeded()) HostMenu_Class = HOSTMENU_WIDGET.Class;
	static ConstructorHelpers::FClassFinder<UFindServer_UserWidget> SERVERMENU_WIDGET(TEXT("WidgetBlueprint'/Game/UI/MainMenu/ServerMenu2.ServerMenu2_C'"));
	if (SERVERMENU_WIDGET.Succeeded()) ServerMenu_Class = SERVERMENU_WIDGET.Class;
	static ConstructorHelpers::FClassFinder<UOptionMenu_UserWidget> OPTIONMENU_WIDGET(TEXT("WidgetBlueprint'/Game/UI/MainMenu/OptionMenus.OptionMenus_C'"));
	if (OPTIONMENU_WIDGET.Succeeded()) OptionMenu_Class = OPTIONMENU_WIDGET.Class;
	static ConstructorHelpers::FClassFinder<UUserWidget> LOADINGSCREEN_WIDGET(TEXT("WidgetBlueprint'/Game/UI/Levels/LoadingScreen.LoadingScreen_C'"));
	if (LOADINGSCREEN_WIDGET.Succeeded()) LodingScreen_Class = LOADINGSCREEN_WIDGET.Class;

}

void UGameInfo_Instance::Init()
{
	if (IOnlineSubsystem* subSystem = IOnlineSubsystem::Get()) {
		sessionInterface = subSystem->GetSessionInterface();
		if (sessionInterface.IsValid()) {
			//SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UGameInfo_Instance::OnCreateSessionComplete);
			sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UGameInfo_Instance::OnCreateSessionComplete_Lobby);
			sessionInterface->OnUpdateSessionCompleteDelegates.AddUObject(this, &UGameInfo_Instance::OnUpdateSessionComplete);
			sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UGameInfo_Instance::OnJoinSessionComplete);
			sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UGameInfo_Instance::OnFindSessionsComplete);
			sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UGameInfo_Instance::OnDestroySessionComplete);
		}
	}
	sessionSearch.Reset();
	//GetEngine()->OnNetworkFailure().AddUObject(this, &UGameInfo_Instance::HandleNetworkError);
}

void UGameInfo_Instance::OnCreateSessionComplete(FName server_Name, bool succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete: Succeeded %d"), succeeded);
	if (succeeded) {
		GetWorld()->ServerTravel("/Game/Maps/LobyMap.LobyMap?listen");
	}

}

void UGameInfo_Instance::OnCreateSessionComplete_Lobby(FName server_Name, bool succeeded)
{
	if (succeeded) {
		UE_LOG(LogTemp, Warning, TEXT("Lobby: OnCreateSessionComplete Succeeded"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("OnCreateSession: Succeeded"));
		//GetWorld()->ServerTravel("/Game/Map/LobyMap.LobyMap?listen");

		FOnlineSessionSettings UpdateSettings;
		//UpdateSettings.bIsDedicated = true;
		//UpdateSettings.bIsLANMatch = false;
		UpdateSettings.bUsesPresence = false;
		UpdateSettings.NumPublicConnections = 5;

		FOnlineSessionSetting AddSessionName;
		AddSessionName.Data = FString("TestServer");
		AddSessionName.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
		UpdateSettings.Settings.Add(FName("SESSION_NAME"), AddSessionName);
		FOnlineSessionSetting AddGameModeName;
		AddGameModeName.Data = FString("TwoVersus");
		AddGameModeName.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
		UpdateSettings.Settings.Add(FName("GAMEMODE_NAME"), AddGameModeName);
		sessionInterface->UpdateSession(server_Name, UpdateSettings);

		//UGameplayStatics::OpenLevel(GetWorld(), lobbyName, true, "listen");
		UGameplayStatics::OpenLevel(GetWorld(), lobbyName, true);
	}
	else {  // 세션을 만드는데 실패했을 때
		UE_LOG(LogTemp, Warning, TEXT("Lobby: OnCreateSessionComplete Failed"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("OnCreateSession: Failed"));
	}
}

/**/
void UGameInfo_Instance::OnUpdateSessionComplete(FName ServerName, bool Succeeded)
{
	if(Succeeded) // 세션 업데이트에 성공 했다면
	{
		UE_LOG(LogTemp, Warning, TEXT("Lobby: OnUpdateSessionComplete Succeeded"));
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Lobby: OnUpdateSessionComplete Failed"));
	}
}

void UGameInfo_Instance::OnJoinSessionComplete(FName server_Name, EOnJoinSessionCompleteResult::Type type)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("OnJoinSessionComplete"));
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete: Succeeded %d"), type);
	if (type == EOnJoinSessionCompleteResult::Success) {
		UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete: Succeeded"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete: Failed"));
	}
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		FString JoinAddress = "";
		JoinServerName = server_Name;
		sessionInterface->GetResolvedConnectString(server_Name, JoinAddress);
		PC->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
	}
}

void UGameInfo_Instance::OnFindSessionsComplete(bool succeeded)
{

	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionsComplete: Succeeded %d"), succeeded);
	if (succeeded && sessionSearch->SearchResults.Num() > 0) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("OnFindSessionsSucceeded"));
		sessionsNum = sessionSearch->SearchResults.Num();
		IsFindServer = true;
		//Join_Server();
		Results= sessionSearch->SearchResults;
		findSessionSucceeded.Broadcast();
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("OnFindSessionsFailed"));
		IsFindServer = false;
		findSessionFaild.Broadcast();
	}

}

void UGameInfo_Instance::OnDestroySessionComplete(FName ServerName, bool succeeded)
{
	if (succeeded) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("OnDestroySessionComplete"));
	}
	
}


/*void UGameInfo_Instance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	//HandleNetworkError()
}*/

void UGameInfo_Instance::HandleNetworkError(ENetworkFailure::Type FailureType, bool IsServer)
{
	if (!IsServer) {
		UE_LOG(LogTemp, Warning, TEXT("NetError: %s"), *NetErrorToString(FailureType));
	}
}

void UGameInfo_Instance::HandleTravelError(ETravelFailure::Type FailureType)
{
	UE_LOG(LogTemp, Warning, TEXT("TravelError: %s"), *TravelErrorToString(FailureType));
}

void UGameInfo_Instance::CreateServer(int32 PlayerNum, FName ServerName, FName GameModeName, bool LanCheck)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("CreateServer"));
	UE_LOG(LogTemp, Warning, TEXT("CreateServer"));
	FOnlineSessionSettings sessionSettings;
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bIsLANMatch = LanCheck;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bUsesPresence = true;
	//SessionSettings.bUsesPresence = false;
	//SessionSettings.bIsDedicated = true;
	sessionSettings.NumPublicConnections = 5;

	FOnlineSessionSetting AddSessionName;
	AddSessionName.Data = ServerName.ToString();
	AddSessionName.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
	sessionSettings.Settings.Add(FName("SESSION_NAME"), AddSessionName);
	FOnlineSessionSetting AddGameModeName;
	AddGameModeName.Data = GameModeName.ToString();
	AddGameModeName.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
	sessionSettings.Settings.Add(FName("GAMEMODE_NAME"), AddGameModeName);

	sessionInterface->DestroySession(ServerName);
	sessionInterface->CreateSession(PlayerNum, ServerName, sessionSettings);
}



void UGameInfo_Instance::ShowMainMenu()
{
	auto PlayerContoller = GetFirstLocalPlayerController();
	if (MainMenu_WB == nullptr) {
		MainMenu_WB = CreateWidget<UUserWidget>(PlayerContoller, MainMenu_Class);
	}
	MainMenu_WB->AddToViewport();
	PlayerContoller->bShowMouseCursor = true;
}

void UGameInfo_Instance::ShowHostMenu()
{
	auto PlayerContoller = GetFirstLocalPlayerController();
	if (HostMenu_WB == nullptr) {
		HostMenu_WB = CreateWidget<UUserWidget>(PlayerContoller, HostMenu_Class);
	}
	HostMenu_WB->AddToViewport();
	PlayerContoller->bShowMouseCursor = true;
}

void UGameInfo_Instance::ShowServerMenu()
{
	auto PlayerContoller = GetFirstLocalPlayerController();
	ServerMenu_WB = CreateWidget<UFindServer_UserWidget>(PlayerContoller, ServerMenu_Class);
	ServerMenu_WB->AddToViewport();
	PlayerContoller->bShowMouseCursor = true;
}

void UGameInfo_Instance::ShowOptionMenu()
{
	auto PlayerContoller = GetFirstLocalPlayerController();
	if (OptionMenu_WB == nullptr) {
		OptionMenu_WB = CreateWidget<UOptionMenu_UserWidget>(PlayerContoller, OptionMenu_Class);
	}
	OptionMenu_WB->AddToViewport();
	PlayerContoller->bShowMouseCursor = true;

	if (!IsCreateSaveFile) {
		OptionMenu_WB->WelcomeMessageVis = ESlateVisibility::Visible;
	}
	else {
		OptionMenu_WB->WelcomeMessageVis = ESlateVisibility::Hidden;
	}
}

void UGameInfo_Instance::LaunchLobby(int32 PlayerNum, FName ServerName, FName GameModeName, bool LanCheck, bool IsDedicated)
{
	if (IsDedicated) {
		FindServer(false, IsDedicated);
	}
	else {
		maxPlayer = PlayerNum;
		serverName = ServerName;
		ShowLodingScreen();
		CreateServer(PlayerNum, ServerName, GameModeName, LanCheck);
	}
}

void UGameInfo_Instance::JoinServer(bool DedicatedCheck)
{
	TArray<FOnlineSessionSearchResult> searchResults = sessionSearch->SearchResults;
	FString sessionId = searchResults[0].Session.GetSessionIdStr();
	UE_LOG(LogTemp, Warning, TEXT("SearchResults Count: %s"), *sessionId);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Join Server"));
	//ServerName= SearchResults[0]->
	int num = 0;
	/*if (DedicatedCheck) { // 데디케이티드 서버 고르기
		for (int i = 0; i < searchResults.Num(); i++) {
			if (searchResults[i].Session.SessionSettings.bIsDedicated) {
				num = i;
			}
		}
	}*/
	//FBlueprintSessionResult
	FName sessionName = FName(*(searchResults[num].Session.SessionSettings.Settings.FindRef("SESSION_NAME").Data.ToString()));
	//FName gameModeName = FName(*(searchResults[num].Session.SessionSettings.Settings.FindRef("GAMEMODE_NAME").Data.ToString()));
	sessionInterface->JoinSession(0, FName(*(searchResults[num].Session.OwningUserName)), searchResults[num]);
}

void UGameInfo_Instance::FindServer(bool LanCheck, bool DedicatedCheck)
{
	IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get();

	if (onlineSub)
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr Sessions = onlineSub->GetSessionInterface();

		if (sessionInterface.IsValid())
		{
			sessionSearch = MakeShareable(new FOnlineSessionSearch());
			sessionSearch->bIsLanQuery = LanCheck;
			sessionSearch->SearchState = EOnlineAsyncTaskState::NotStarted;
			sessionSearch->MaxSearchResults = 50;
			sessionSearch->QuerySettings.Set(SEARCH_DEDICATED_ONLY, DedicatedCheck, EOnlineComparisonOp::Equals);
			sessionSearch->QuerySettings.Set(SEARCH_NONEMPTY_SERVERS_ONLY, true, EOnlineComparisonOp::Equals);

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = sessionSearch.ToSharedRef();
			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			sessionInterface->FindSessions(0, SearchSettingsRef);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("FindServer"));
		}
	}
	else
	{
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}

}

void UGameInfo_Instance::ShowLodingScreen()
{
	auto PlayerContoller = GetFirstLocalPlayerController();
	if (LodingScreen_WB == nullptr) {
		LodingScreen_WB = CreateWidget<UUserWidget>(PlayerContoller, LodingScreen_Class);
	}
	LodingScreen_WB->AddToViewport();
}

void UGameInfo_Instance::Destroy_SessionCaller(APlayerController PC)
{
	// DestroySession() 작성하기
}

void UGameInfo_Instance::Check_SaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(playerSettingsSave, 0)) {
		ShowMainMenu();
		IsCreateSaveFile = true;
	}
	else {
		ShowOptionMenu();
		auto PlayerContoller = GetFirstLocalPlayerController();
		PlayerContoller->bShowMouseCursor = true;
	}
}

void UGameInfo_Instance::ChoiseJoinServer(int num)
{
	TArray<FOnlineSessionSearchResult> searchResults = sessionSearch->SearchResults;
	if (searchResults.Num() > 0) {
		FName sessionName = FName(*(searchResults[num].Session.SessionSettings.Settings.FindRef("SESSION_NAME").Data.ToString()));;
		sessionInterface->JoinSession(0, FName(*(searchResults[num].Session.OwningUserName)), searchResults[num]);
	}
}

void UGameInfo_Instance::DestroySessionAndLeaveGame()
{
	if (sessionInterface) {
		sessionInterface->DestroySession(JoinServerName);
	}
}


FString UGameInfo_Instance::NetErrorToString(ENetworkFailure::Type FailureType)
{
	return FString();
}

FString UGameInfo_Instance::TravelErrorToString(ETravelFailure::Type FailureType)
{
	return FString();
}
