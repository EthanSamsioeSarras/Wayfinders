// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaytestLogger.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"


void UPlaytestLogger::LogEvent(FString EventName)
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
	
	TSharedPtr<FJsonObject> EventObject = MakeShared<FJsonObject>();
	
	const FDateTime Now = FDateTime::Now();

	EventObject->SetStringField(TEXT("event"), EventName);
	
	EventObject->SetStringField(
		TEXT("date"),
		Now.ToString(TEXT("%Y-%m-%d")));

	EventObject->SetStringField(
		TEXT("time"),
		Now.ToString(TEXT("%H:%M:%S")));

	Events.Add(MakeShared<FJsonValueObject>(EventObject));

	FString OutputJson;

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputJson);

	FJsonSerializer::Serialize(Events, Writer);

	FFileHelper::SaveStringToFile(OutputJson, *FilePath);
	
}