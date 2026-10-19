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
	
};
