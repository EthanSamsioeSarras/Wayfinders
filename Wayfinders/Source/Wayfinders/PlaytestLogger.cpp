// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytestLogger.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformTime.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/Guid.h"


void UPlaytestLogger::StartSession()
{
	
	// Leta upp förra sessionID så vi inte får dubbla ID med samma value
	const FString FilePath = FPaths::ProjectSavedDir() / TEXT("Playtestlog.json");
	
	FString ExistingJson;
	int32 HighestSessionNumber = 0;
	
	if (FFileHelper::LoadFileToString(ExistingJson, *FilePath))
	{
		TArray<TSharedPtr<FJsonValue>> Events;
		
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ExistingJson);
		
		if (FJsonSerializer::Deserialize(Reader, Events))
		{
			for (const TSharedPtr<FJsonValue>& Value : Events)
			{
				if (!Value.IsValid() || Value->Type != EJson::Object)
				{
					continue;
				}
				
				const TSharedPtr<FJsonObject> Object = Value->AsObject();
				
				FString ExistingSessionID;
				
				if (Object->TryGetStringField(TEXT("session_id"), ExistingSessionID))
				{
					if (ExistingSessionID.StartsWith(TEXT("Session ")))
					{
						const FString NumberString = ExistingSessionID.RightChop(8);
						
						HighestSessionNumber = FMath::Max(HighestSessionNumber, FCString::Atoi(*NumberString));
					}
				}
			}
		}
	}
	
	//Sätt session number till rätt session
	SessionNumber = HighestSessionNumber + 1;
	
	SessionID = FString::Printf(TEXT("Session %d"), SessionNumber);
	LastCheckpointTime = -1.0;
	
	SaveEvent(TEXT("SessionStarted"));
}

void UPlaytestLogger::LogEvent(FString EventName)
{
		SaveEvent(EventName);
}

void UPlaytestLogger::LogCheckpoint(FString CheckpointName)
{
	const double CurrentTime = FPlatformTime::Seconds();
	
	TSharedPtr<FJsonObject> ExtraData = MakeShared<FJsonObject>();
	
	ExtraData->SetStringField(TEXT("checkpoint"), CheckpointName);
	
	if (LastCheckpointTime >= 0.0)
	{
		ExtraData->SetNumberField(TEXT("time_since_previous_checkpoint"), CurrentTime - LastCheckpointTime);
	}
	
	LastCheckpointTime = CurrentTime;
	
	SaveEvent(TEXT("CheckpointReached"), ExtraData);
}

void UPlaytestLogger::LogDeath()
{
	SaveEvent(TEXT("PlayerDeath"));
}

void UPlaytestLogger::LogFireflyUsed()
{
	SaveEvent(TEXT("FireflyUsed"));
}

void UPlaytestLogger::SaveEvent(const FString& EventName, const TSharedPtr<FJsonObject>& ExtraData)
{
	const FString FilePath = FPaths::ProjectSavedDir() / TEXT("Playtestlog.json");
	
	TArray<TSharedPtr<FJsonValue>> Events;
	FString ExistingJson;
	
	if (FFileHelper::LoadFileToString(ExistingJson, *FilePath))
	{
		
		TSharedPtr<FJsonValue> RootValue;
		
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ExistingJson);
		
		if (FJsonSerializer::Deserialize(Reader, RootValue) && RootValue.IsValid() && RootValue->Type == EJson::Array)
		{
			Events = RootValue->AsArray();
		}
	}
	
	if (SessionID.IsEmpty())
	{
		SessionID = FGuid::NewGuid().ToString(EGuidFormats::Digits);
	}
	
	TSharedPtr<FJsonObject> EventObject = MakeShared<FJsonObject>();
	
	const FDateTime Now = FDateTime::Now();

	EventObject->SetStringField(TEXT("session_id"), SessionID);
	
	EventObject->SetStringField(TEXT("event"), EventName);
	
	EventObject->SetStringField(
		TEXT("date"),
		Now.ToString(TEXT("%Y-%m-%d")));

	EventObject->SetStringField(
		TEXT("time"),
		Now.ToString(TEXT("%H:%M:%S")));

	if (ExtraData.IsValid())
	{
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : ExtraData->Values)
		{
			EventObject->SetField(Pair.Key, Pair.Value);
		}
	}
	
	Events.Add(MakeShared<FJsonValueObject>(EventObject));

	FString OutputJson;

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputJson);

	if (FJsonSerializer::Serialize(Events, Writer))
	{
		FFileHelper::SaveStringToFile(OutputJson, *FilePath);
	}
}
