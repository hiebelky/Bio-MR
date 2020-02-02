// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "api_socket.generated.h"

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

	// Functions to modify the scene, implemented in Blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void SetRainIntensity(float rain);

	UFUNCTION(BlueprintImplementableEvent)
	void SetDayLength(float minutes);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFetchQuest(int items);


	// Structure for storing a single UDP packet from iMotions
	struct UDPPacket {
		int32 m_sequenceNumber;
		FString m_eventSource;
		FString m_sampleName;
		int32 m_timeStamp; // ms since start of slideshow
		int32 m_mediaTime; // position of current video in ms, -1 indicates no video
		TArray<FString> m_rawData;
	};
	
	// Objects used for recieving UDP packets
	FSocket* ListenSocket = nullptr;
	FUdpSocketReceiver* UDPReceiver = nullptr;

	// Functions used for recieving UDP packets
	bool StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort);
	void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);
	
	// Functions for processing UDP packets
	FString StringFromBinaryArray(TArray<uint8> BinaryArray);
	void CreatePacket(UDPPacket* out, FString& data);



	bool ShowOnScreenDebugMessages = true;
	float OnScreenDebugMessageTimeout = 100.f;

	//ScreenMsg
	FORCEINLINE void ScreenMsg(const FString& Msg)
	{
		if (!ShowOnScreenDebugMessages) return;
		GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, *Msg, false, FVector2D(2.f, 2.f));
	}
	FORCEINLINE void ScreenMsg(const FString& Msg, const float Value)
	{
		if (!ShowOnScreenDebugMessages) return;
		GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, FString::Printf(TEXT("%s %f"), *Msg, Value), false, FVector2D(2.f, 2.f));
	}
	FORCEINLINE void ScreenMsg(const FString& Msg, const FString& Msg2)
	{
		if (!ShowOnScreenDebugMessages) return;
		GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, FString::Printf(TEXT("%s %s"), *Msg, *Msg2), false, FVector2D(2.f, 2.f));
	}
	void ScreenMsg(const UDPPacket& packet)
	{
		if (!ShowOnScreenDebugMessages) return;
		GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, FString::Printf(TEXT("Sequence Num: %d"), packet.m_sequenceNumber), false, FVector2D(2.f, 2.f));
		GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, FString::Printf(TEXT("Event Source: %s"), *packet.m_eventSource), false, FVector2D(2.f, 2.f));
		GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, FString::Printf(TEXT("Sample Name: %s"), *packet.m_sampleName), false, FVector2D(2.f, 2.f));
		GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, FString::Printf(TEXT("Timestamp: %d"), packet.m_timeStamp), false, FVector2D(2.f, 2.f));
		GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, FString::Printf(TEXT("Media Time: %d"), packet.m_mediaTime), false, FVector2D(2.f, 2.f));
		for (int i = 0; i < packet.m_rawData.Num(); ++i) {
			GEngine->AddOnScreenDebugMessage(-1, OnScreenDebugMessageTimeout, FColor::Red, FString::Printf(TEXT("Data[%d]: %s"), i, *packet.m_rawData[i]), false, FVector2D(2.f, 2.f));
		}
	}


	// Called whenever this actor is being removed from a level 
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
