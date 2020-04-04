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

	// Start the receiver and 
	StartUDPReceiver("Game Engine Receiver", "127.0.0.1", RECEIVE_FROM_API_PORT);
	StartUDPSender("Game Engine Sender", "127.0.0.1", SEND_TO_API_PORT);

	// Setup all the modifiable commands
	FString command1 = FString("RegisterCommand;").Append(RAIN_INTENSITY_NAME).Append(";False;Float;0;0;1;");
	SendUDPDatagram(command1);
	FString command2 = FString("RegisterCommand;").Append(DAY_LENGTH_NAME).Append(";False;Float;0.05;1;60;");
	SendUDPDatagram(command2);
	FString command3 = FString("RegisterCommand;").Append(FETCH_QUEST_NAME).Append(";True;Int;1;3;100;");
	SendUDPDatagram(command3);

	// Set default values
	SetRainIntensity(0.f);
	SetDayLength(1.f);
}

// Called every frame
void Aapi_socket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void Aapi_socket::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

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
bool Aapi_socket::StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort)
{
	ScreenMsg("RECEIVER INIT");

	FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);

	// Create socket endpoint
	FIPv4Endpoint Endpoint(Addr, ThePort);

	// Set the default buffer size
	int32 BufferSize = 2 * 1024 * 1024;

	// Build the socket
	ListenSocket = FUdpSocketBuilder(*YourChosenSocketName).AsNonBlocking().AsReusable().BoundToEndpoint(Endpoint).WithReceiveBufferSize(BufferSize);

	// Set up the UDP reciever
	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	UDPReceiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP RECEIVER"));
	UDPReceiver->OnDataReceived().BindUObject(this, &Aapi_socket::Recv);
	UDPReceiver->Start();

	return true;
}

void Aapi_socket::Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	ScreenMsg("Received bytes: ", ArrayReaderPtr->Num());

	// Copy the data to a mutable buffer
	TArray<uint8> rawData;
	rawData.Init(0, ArrayReaderPtr->Num());
	for (int32 i = ArrayReaderPtr->Num() - 1; i >= 0; --i) {
		rawData[i] = ArrayReaderPtr->Pop();
	}

	// Convert the data into a string
	FString parsedMessage = StringFromBinaryArray(rawData);
	ScreenMsg("Message: ", parsedMessage);
	PrintToLog(parsedMessage);

	// Convert the string into a datagram
	UDPDatagarm datagram;
	CreateDatagram(&datagram, parsedMessage);

	// Call function to handle this new message
	ProcessDatagram(datagram);
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
	// Appends every new datagram to a log file
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

void Aapi_socket::CreateDatagram(UDPDatagarm* out, FString& data)
{
	// Split the string on all semiciolons
	TArray<FString> tokenizedString;
	data.ParseIntoArray(tokenizedString, TEXT(";"));

	// Store the metadata
	out->m_commandName = tokenizedString[0];
	
	// Remove the first entry
	tokenizedString.RemoveAt(0, 1, true);
	
	// Store the raw data
	out->m_arguments = tokenizedString;
}

void Aapi_socket::ProcessDatagram(UDPDatagarm& datagram)
{
	if (datagram.m_arguments.Num() < 0) {
		return;
	}

	// Call the correct blueprint function based on the command
	if (datagram.m_commandName == RAIN_INTENSITY_NAME) {
		float val = FCString::Atof(*datagram.m_arguments[0]);
		SetRainIntensity(val);
	} else if (datagram.m_commandName == DAY_LENGTH_NAME) {
		float val = FCString::Atof(*datagram.m_arguments[0]);
		SetDayLength(val);
	} else if (datagram.m_commandName == FETCH_QUEST_NAME) {
		int32 val = FCString::Atoi(*datagram.m_arguments[0]);
		CreateFetchQuest(val);
	}
}


// -------------------------------------
// UDP Sender
// -------------------------------------
bool Aapi_socket::StartUDPSender(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort) {
	//Create Remote Address.
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid;
	RemoteAddr->SetIp(*TheIP, bIsValid);
	RemoteAddr->SetPort(ThePort);

	if (!bIsValid)
	{
		ScreenMsg("Sender IP address not valid: ", TheIP);
		return false;
	}

	SenderSocket = FUdpSocketBuilder(*YourChosenSocketName).AsReusable().WithBroadcast();

	//Set Send Buffer Size
	int32 SendSize = 2 * 1024 * 1024;
	SenderSocket->SetSendBufferSize(SendSize, SendSize);
	SenderSocket->SetReceiveBufferSize(SendSize, SendSize);

	ScreenMsg(TEXT("SENDER INIT"));

	return true;
}



bool Aapi_socket::SendUDPDatagram(FString ToSend)
{
	if (!SenderSocket)
	{
		return false;
	}

	int32 BytesSent = 0;

	// Convert wide characters to utf-8 encoding
	ANSICHAR* utf8Data = TCHAR_TO_UTF8(*ToSend);
	int32 count = ToSend.Len();
	uint8* data = reinterpret_cast<uint8*>(utf8Data);

	// Send the bytes over the socket connection
	SenderSocket->SendTo(data, count, BytesSent, *RemoteAddr);

	// Ensure all bytes were sent
	if (BytesSent <= 0)
	{
		ScreenMsg("Failed to send datagram of size: ", count);
		return false;
	}

	ScreenMsg("Sent bytes: ", BytesSent);

	return true;
}