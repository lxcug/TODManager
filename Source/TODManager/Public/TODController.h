#pragma once

#include "CoreMinimal.h"
#include "Components/PostProcessComponent.h"
#include "TODController.generated.h"


UCLASS(Blueprintable)
class ATODController : public AActor
{
	GENERATED_BODY()

public:
	ATODController();

	void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void TickTime(float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
	void UpdateHours(int Hours);

	UFUNCTION(BlueprintCallable)
	void UpdateMinutes(float Minutes);

	void UpdateTime(float Hours, bool ResetMinutes = false);

	void UpdateTime(float Hours, float Minutes);

	/*
	 * 0.f for 0:00, 1.f for 24:00
	 */
	void UpdateTimeRelative(float RelativeValue);

protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="LightingComponent")
	TObjectPtr<UDirectionalLightComponent> SunLight;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="LightingComponent")
	TObjectPtr<UDirectionalLightComponent> MoonLight;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="LightingComponent")
	TObjectPtr<USkyLightComponent> SkyLight;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="LightingComponent")
	TObjectPtr<USkyAtmosphereComponent> SkyAtmosphere;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="PostProcessComponent")
	TObjectPtr<UPostProcessComponent> PostProcess;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Root")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="TimeControl")
	uint8 CurrentHours = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="TimeControl")
	float CurrentMinutes = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="TimeControl")
	bool bTimeFlow = true;

	float GameDayDurationInSeconds = GameDayDurationInMinutes * 60;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="TimeControl")
	float TimeFlowMultiplier = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="TimeControl")
	float SunriseTime = 6;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="TimeControl")
	float SunsetTime = 18;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="TimeControl")
	float MoonriseTime = 18;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="TimeControl")
	float MoonsetTime = 6;

	// Default 1h in game is 1min in reality, 12minutes for a game day
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="TimeControl")
	float GameDayDurationInMinutes = 4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="LightingControl")
	float SunLightIntensity = 2 * PI;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="LightingControl")
	TObjectPtr<UCurveFloat> SunLightIntensityRatioCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="LightingControl")
	TObjectPtr<UCurveLinearColor> SkyColorCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="LightingControl")
	float MoonLightIntensity = 0.1;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSunrise();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSunset();

	UFUNCTION(BlueprintImplementableEvent)
	void OnMoonrise();

	UFUNCTION(BlueprintImplementableEvent)
	void OnMoonset();

	UFUNCTION(BlueprintImplementableEvent)
	void OnNewDay();

private:
	// Flag for record BlueprintImplementableEvent
	bool bSunrise : 1 = false;
	bool bSunset : 1 = false;
	bool bMoonrise : 1 = false;
	bool bMoonset : 1 = false;

	void ResetFlags()
	{
		bSunrise = bSunset = bMoonrise = bMoonset = false;
	}

private:

	void UpdateLighting();

	bool IsSunLightActive() const
	{
		return CurrentHours >= SunriseTime && CurrentHours <= SunsetTime;
	}

	bool IsMoonLightActive() const
	{
		return CurrentHours >= MoonriseTime || CurrentHours <= MoonsetTime;
	}

public:
	UFUNCTION(BlueprintCallable)
	uint8 GetCurrentHour() const
	{
		return CurrentHours;
	}

	UFUNCTION(BlueprintCallable)
	uint8 GetCurrentMinute() const
	{
		return CurrentMinutes;
	}

	UFUNCTION(BlueprintCallable)
	FString GetTimeString() const
	{
		return FString::Printf(TEXT("%02d:%02d"), CurrentHours, static_cast<int>(CurrentMinutes));
	}

	UFUNCTION(BlueprintCallable)
	bool IsTimeFlow() const
	{
		return bTimeFlow;
	}

	UFUNCTION(BlueprintCallable)
	void SetTimeFlow(bool Value)
	{
		bTimeFlow = Value;
	}

	UFUNCTION(BlueprintCallable)
	float GetTimeFlowMultiplier() const
	{
		return TimeFlowMultiplier;
	}

	UFUNCTION(BlueprintCallable)
	void SetTimeFlowMultiplier(float Value)
	{
		TimeFlowMultiplier = Value;
	}
};
