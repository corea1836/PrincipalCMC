// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PrincipalCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class PRINCIPAL_API APrincipalCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly) float CrouchBlendDuration = .2f;
	float CrouchBlendTime;
public:
	APrincipalCameraManager();
	
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
};
