// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlaytestLogger.generated.h"

/**
 * 
 */
UCLASS()
class WAYFINDERS_API UPlaytestLogger : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Playtest Logging")
	void LogEvent(FString EventName);
	
	UFUNCTION(BlueprintCallable, Category = "Playtest Logging")
	void StartSession();
	
	UFUNCTION(BlueprintCallable, Category = "Playtest Logging")
	void LogCheckpoint(FString CheckpointName);
	
	UFUNCTION(BlueprintCallable, Category = "Playtest Logging")
	void LogDeath();
	
	UFUNCTION(BlueprintCallable, Category = "Playtest Logging")
	void LogFireflyUsed();
	
private:
	FString SessionID;
	int32 SessionNumber = 0;
	double LastCheckpointTime = -1.0;
	
	void SaveEvent(
		const FString& EventName,
		const TSharedPtr<class FJsonObject>& ExtraData = nullptr);
	
};
