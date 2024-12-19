#include "TODController.h"

#include "../../../../../Source/Titan/Titan.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Curves/CurveLinearColor.h"


ATODController::ATODController()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("TODPlacer Root"));
	Root->SetWorldRotation(FRotator(0, 0, 0));

	SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("TODPlacer SunLight"));
	SunLight->SetupAttachment(Root);
	SunLight->SetAtmosphereSunLight(true);
	SunLight->SetAtmosphereSunLightIndex(0);
	SunLight->ForwardShadingPriority = 0;

	MoonLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("TODPlacer MoonLight"));
	MoonLight->SetupAttachment(Root);
	MoonLight->SetLightColor(FLinearColor(0.1, 0.3, 0.7));
	MoonLight->SetAtmosphereSunLight(true);
	MoonLight->SetAtmosphereSunLightIndex(1);
	MoonLight->ForwardShadingPriority = 1;

	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("TODPlacer SkyLight"));
	SkyLight->SetupAttachment(Root);
	SkyLight->SetRealTimeCapture(true);

	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("TODPlacer SkyAtomsphere"));
	SkyAtmosphere->SetupAttachment(Root);

	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("TODPlacer PostProcess"));
	PostProcess->SetupAttachment(Root);

	CurrentHours = SunriseTime;
	if (bTimeFlow)
	{
		UpdateLighting();
	}
}

void ATODController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// if (GEngine)
	// {
	// 	const FString PrintMessage = FString::Printf(
	// 		TEXT("Current time %02d:%02d"), CurrentHours, static_cast<int>(CurrentMinutes));
	// 	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, PrintMessage);
	// }


	TickTime(DeltaSeconds);
}

void ATODController::UpdateTime(float Hours, bool ResetMinutes)
{
	CurrentHours = FMath::Clamp(Hours, 0, 23);
	if (ResetMinutes)
	{
		CurrentMinutes = 0;
	}

	ResetFlags();
	UpdateLighting();
}

void ATODController::UpdateTime(float Hours, float Minutes)
{
	CurrentHours = FMath::Clamp(Hours, 0, 23);
	CurrentMinutes = FMath::Clamp(Minutes, 0, 59);

	ResetFlags();
	UpdateLighting();
}

void ATODController::UpdateTimeRelative(float RelativeValue)
{
	int AccumulatedMinutes = 24 * 60 * RelativeValue;
	CurrentHours = AccumulatedMinutes / 60;
	CurrentMinutes = AccumulatedMinutes % 60;

	ResetFlags();
	UpdateLighting();
}

void ATODController::TickTime(float DeltaSeconds)
{
	if (!bTimeFlow)
	{
		return;
	}

	const float SecondsScale = 24 * 3600 / GameDayDurationInSeconds;
	const float DeltaMinutes = DeltaSeconds * SecondsScale * TimeFlowMultiplier / 60;
	CurrentMinutes += DeltaMinutes;

	if (CurrentMinutes >= 60)
	{
		if (CurrentHours == 23)
		{
			ResetFlags();
			OnNewDay();
		}

		const int Hours = CurrentMinutes / 60;
		CurrentHours += Hours;
		CurrentMinutes -= 60 * Hours;
	}
	if (CurrentHours >= 24)
	{
		CurrentHours %= 24;
	}

	if (!bSunrise && CurrentHours == SunriseTime)
	{
		bSunrise = true;
		OnSunrise();
	}
	if (!bSunset && CurrentHours == SunsetTime + 1)
	{
		bSunset = true;
		OnSunset();
	}
	if (!bMoonrise && CurrentHours == MoonriseTime)
	{
		bMoonrise = true;
		OnMoonrise();
	}
	if (!bMoonset && CurrentHours == MoonsetTime + 1)
	{
		bMoonset = true;
		OnMoonset();
	}

	UpdateLighting();
}

void ATODController::UpdateHours(int Hours)
{
	CurrentHours = FMath::Clamp(Hours, 0, 23);

	ResetFlags();
	UpdateLighting();
}

void ATODController::UpdateMinutes(float Minutes)
{
	CurrentMinutes = FMath::Clamp(Minutes, 0, 59);

	ResetFlags();
	UpdateLighting();
}

void ATODController::UpdateLighting()
{
	Root->SetWorldRotation(FRotator(0, 0, 0));
	GameDayDurationInSeconds = GameDayDurationInMinutes * 60;

	if (SkyColorCurve)
	{
		const float DayTimeRatio = (CurrentHours * 60 + CurrentMinutes) / (24 * 60);
		const FLinearColor NewSkyColor = SkyColorCurve->GetLinearColorValue(DayTimeRatio);
		SkyLight->SetLightColor(NewSkyColor);
	}

	if (IsSunLightActive())
	{
		SunLight->SetHiddenInGame(false);

		const float DayTimeRatio = ((CurrentHours - SunriseTime) * 60 + CurrentMinutes) / ((SunsetTime - SunriseTime +
			1) * 60);

		FVector RotateAxis = FVector(0, -1, 1);
		RotateAxis.Normalize();
		const FQuat TargetQuat = FQuat::Slerp({RotateAxis, PI + UE_KINDA_SMALL_NUMBER}, {RotateAxis, 2 * PI}, DayTimeRatio);
		SunLight->SetRelativeRotation(TargetQuat);

		const float NewIntensity = SunLightIntensityRatioCurve
			                           ? SunLightIntensity * SunLightIntensityRatioCurve->GetFloatValue(DayTimeRatio)
			                           : SunLightIntensity;
		SunLight->SetIntensity(NewIntensity);
	}
	else
	{
		SunLight->SetHiddenInGame(true);
	}

	if (IsMoonLightActive())
	{
		MoonLight->SetHiddenInGame(false);

		const float NightTimeRatio = (CurrentHours >= MoonriseTime
			                              ? (CurrentHours - MoonriseTime) * 60 + CurrentMinutes
			                              : (CurrentHours + (25 - MoonriseTime)) * 60 + CurrentMinutes) / ((26 -
			MoonriseTime + MoonsetTime) * 60);

		FVector RotateAxis = FVector(0, -1, 1);
		RotateAxis.Normalize();
		const FQuat TargetQuat = FQuat::Slerp({RotateAxis, PI + UE_KINDA_SMALL_NUMBER}, {RotateAxis, 2 * PI}, NightTimeRatio);
		MoonLight->SetRelativeRotation(TargetQuat);

		MoonLight->SetIntensity(MoonLightIntensity);
	}
	else
	{
		MoonLight->SetHiddenInGame(true);
	}
}
