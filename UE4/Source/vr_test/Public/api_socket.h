// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "api_socket.generated.h"

// Define constants for networking
const int32 SEND_TO_API_PORT = 60003;
const int32 RECEIVE_FROM_API_PORT = 60002;
const FString RAIN_INTENSITY_NAME = "Rain Intensity";
const FString DAY_LENGTH_NAME = "Day Length (minutes)";
const FString FETCH_QUEST_NAME = "Create Fetch Quest";

UCLASS()
class VR_TEST_API Aapi_socket : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Aapi_socket();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called whenever this actor is being removed from a level 
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	// -------------------------------------
	// Functions to modify the scene, implemented in Blueprint
	// -------------------------------------
	UFUNCTION(BlueprintImplementableEvent)
	void SetRainIntensity(float rain);

	UFUNCTION(BlueprintImplementableEvent)
	void SetDayLength(float minutes);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFetchQuest(int32 items);



	// -------------------------------------
	// Structures
	// -------------------------------------
	struct UDPDatagarm {
		FString m_commandName;
		TArray<FString> m_arguments;
	};

	// -------------------------------------
	// UDP Receiver
	// -------------------------------------
	FSocket* ListenSocket = nullptr;
	FUdpSocketReceiver* UDPReceiver = nullptr;

	// Functions used for recieving UDP datagrams
	bool StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort);
	void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);
	
	// Functions for processing UDP datagrams
	FString StringFromBinaryArray(TArray<uint8> BinaryArray);
	void CreateDatagram(UDPDatagarm* out, FString& data);
	void ProcessDatagram(UDPDatagarm& datagram);


	// -------------------------------------
	// UDP Sender
	// -------------------------------------
	FSocket* SenderSocket = nullptr;
	TSharedPtr<FInternetAddr> RemoteAddr;

	UFUNCTION(BlueprintCallable)
	bool SendUDPDatagram(FString ToSend);

	bool StartUDPSender(const FString& YourChosenSocketName,const FString& TheIP,const int32 ThePort);




	// -------------------------------------
	// Debug messages
	// -------------------------------------
	bool ShowOnScreenDebugMessages = false;
	float OnScreenDebugMessageTimeout = 100.f;

	FORCEINLINE void ScreenMsg(const FString& Msg)
	{
		if (ShowOnScreenDebugMessages)
		{
			GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, *Msg, false, FVector2D(2.f, 2.f));
		}
	}
	FORCEINLINE void ScreenMsg(const FString& Msg, const float Value)
	{
		if (ShowOnScreenDebugMessages)
		{
			GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, FString::Printf(TEXT("%s %f"), *Msg, Value), false, FVector2D(2.f, 2.f));
		}
	}
	FORCEINLINE void ScreenMsg(const FString& Msg, const FString& Msg2)
	{
		if (ShowOnScreenDebugMessages)
		{
			GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, FString::Printf(TEXT("%s %s"), *Msg, *Msg2), false, FVector2D(2.f, 2.f));
		}
	}
	void PrintToLog(FString toPrint);
};
