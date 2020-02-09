// Fill out your copyright notice in the Description page of Project Settings.


#include "api_socket.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

// Sets default values
Aapi_socket::Aapi_socket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void Aapi_socket::BeginPlay()
{
	Super::BeginPlay();

	// Start the reciever and 
	StartUDPReceiver("Game Engine Receiver", "127.0.0.1", RECEIVE_FROM_API_PORT);
	StartUDPSender("Game Engine Sender", "127.0.0.1", SEND_TO_API_PORT);

	SendUDPDatagram(TEXT("RegisterCommand;Rain Intensity;False;Float;0;0;1;"));

	// Set default values
	SetRainIntensity(0.f);
	SetDayLength(1.f);
	CreateFetchQuest(3);
	CreateFetchQuest(2);
}

// Called every frame
void Aapi_socket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void Aapi_socket::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//~~~~~~~~~~~~~~~~

	delete UDPReceiver;

	// Clear all sockets!
	// makes sure repeat plays in Editor dont hold on to old sockets!
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
	if (SenderSocket)
	{
		SenderSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
	}
}



// -------------------------------------
// UDP Receiver
// -------------------------------------
bool Aapi_socket::StartUDPReceiver(
	const FString& YourChosenSocketName,
	const FString& TheIP,
	const int32 ThePort
) {

	ScreenMsg("RECEIVER INIT");

	//~~~

	FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);

	//Create Socket
	FIPv4Endpoint Endpoint(Addr, ThePort);

	//BUFFER SIZE
	int32 BufferSize = 2 * 1024 * 1024;

	ListenSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);

	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	UDPReceiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP RECEIVER"));
	UDPReceiver->OnDataReceived().BindUObject(this, &Aapi_socket::Recv);
	UDPReceiver->Start();

	return true;
}

void Aapi_socket::Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	ScreenMsg("Received bytes", ArrayReaderPtr->Num());

	// Copy the data to a mutable buffer
	TArray<uint8> rawData;
	rawData.Init(0, ArrayReaderPtr->Num());
	for (int32 i = ArrayReaderPtr->Num() - 1; i >= 0; --i) {
		rawData[i] = ArrayReaderPtr->Pop();
	}

	// Convert the data into a string
	FString parsedMessage = StringFromBinaryArray(rawData);
	ScreenMsg("Message= ", parsedMessage);
	PrintToLog(parsedMessage);

	// Convert the string into a packet
	UDPPacket packet;
	CreatePacket(&packet, parsedMessage);
	ScreenMsg(packet);

	// Call function to handle this new message
	ProcessPacket(packet);
}

//Rama's String From Binary Array
FString Aapi_socket::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	BinaryArray.Add(0); // Add 0 termination. Even if the string is already 0-terminated, it doesn't change the results.
	// Create a string from a byte array. The string is expected to be 0 terminated (i.e. a byte set to 0).
	// Use UTF8_TO_TCHAR if needed.
	// If you happen to know the data is UTF-16 (USC2) formatted, you do not need any conversion to begin with.
	// Otherwise you might have to write your own conversion algorithm to convert between multilingual UTF-16 planes.
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}

void Aapi_socket::PrintToLog(FString toPrint)
{
	toPrint.Append("\r\n");
	FString SaveDirectory = FPaths::ProjectLogDir();
	FString FileName = FString("UDPLog.txt");


	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (PlatformFile.CreateDirectoryTree(*SaveDirectory))
	{
		// Get absolute file path
		FString AbsoluteFilePath = SaveDirectory + "/" + FileName;
		FFileHelper::SaveStringToFile(toPrint, *AbsoluteFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	}
}

void Aapi_socket::CreatePacket(UDPPacket* out, FString& data)
{
	// Split the string on all semiciolons
	TArray<FString> tokenizedString;
	data.ParseIntoArray(tokenizedString, TEXT(";"));

	// Store the metadata
	out->m_commandName = tokenizedString[0];
	
	// Remove these first 5 entries
	tokenizedString.RemoveAt(0, 1, true);
	
	// Store the raw data
	out->m_arguments = tokenizedString;
}

void Aapi_socket::ProcessPacket(UDPPacket& packet)
{
	if (packet.m_commandName == "RainIntensity") {
		if (packet.m_arguments.Num() > 0) {
			float val = FCString::Atof(*packet.m_arguments[0]);
			SetRainIntensity(val);
		}
	}
}


// -------------------------------------
// UDP Sender
// -------------------------------------
bool Aapi_socket::StartUDPSender(
	const FString& YourChosenSocketName,
	const FString& TheIP,
	const int32 ThePort
) {
	//Create Remote Address.
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid;
	RemoteAddr->SetIp(*TheIP, bIsValid);
	RemoteAddr->SetPort(ThePort);

	if (!bIsValid)
	{
		ScreenMsg("Rama UDP Sender>> IP address was not valid!", TheIP);
		return false;
	}

	SenderSocket = FUdpSocketBuilder(*YourChosenSocketName).AsReusable().WithBroadcast();

	//Set Send Buffer Size
	int32 SendSize = 2 * 1024 * 1024;
	SenderSocket->SetSendBufferSize(SendSize, SendSize);
	SenderSocket->SetReceiveBufferSize(SendSize, SendSize);

	ScreenMsg(TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	ScreenMsg(TEXT("Rama ****UDP**** Sender Initialized Successfully!!!"));
	ScreenMsg(TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));

	return true;
}



bool Aapi_socket::SendUDPDatagram(FString ToSend)
{
	if (!SenderSocket)
	{
		ScreenMsg("No sender socket");
		return false;
	}
	//~~~~~~~~~~~~~~~~

	int32 BytesSent = 0;

	/*FAnyCustomData NewData;
	NewData.Scale = FMath::FRandRange(0, 1000);
	NewData.Count = FMath::RandRange(0, 100);
	NewData.Color = FLinearColor(FMath::FRandRange(0, 1), FMath::FRandRange(0, 1), FMath::FRandRange(0, 1), 1);

	FArrayWriter Writer;

	Writer << NewData; //Serializing our custom data, thank you UE4!*/

	ANSICHAR* utf8Data = TCHAR_TO_UTF8(*ToSend);
	int32 count = ToSend.Len();
	/*uint8* data = new uint8[count];
	StringToBytes(utf8Data, data, count);*/
	uint8* data = reinterpret_cast<uint8*>(utf8Data);

	SenderSocket->SendTo(data, count, BytesSent, *RemoteAddr);

	if (BytesSent <= 0)
	{
		const FString Str = "Socket is valid but the receiver received 0 bytes, make sure it is listening properly!";
		UE_LOG(LogTemp, Error, TEXT("%s"), *Str);
		ScreenMsg(Str);
		return false;
	}

	ScreenMsg("UDP~ Send Succcess! Bytes Sent = ", BytesSent);

	return true;
}