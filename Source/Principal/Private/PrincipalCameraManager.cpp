// Fill out your copyright notice in the Description page of Project Settings.


#include "PrincipalCameraManager.h"

#include "PrincipalCharacter.h"
#include "PrincipalCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

APrincipalCameraManager::APrincipalCameraManager()
{
}

void APrincipalCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
	
	if (APrincipalCharacter* PrincipalCharacter = Cast<APrincipalCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		UPrincipalCharacterMovementComponent* PMC = PrincipalCharacter->GetPrincipalCharacterMovement();
		FVector TargetCrouchOffset = FVector(0, 0, 
			PMC->GetCrouchedHalfHeight() - PrincipalCharacter->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	
		FVector Offset = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset, FMath::Clamp(CrouchBlendTime / CrouchBlendDuration, 0.f, 1.f));
		
		if (PMC->IsCrouching())
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.f, CrouchBlendDuration);
			Offset -= TargetCrouchOffset;
		}
		else
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime - DeltaTime, 0.f, CrouchBlendDuration);
		}
		
		if (PMC->IsMovingOnGround())
		{
			OutVT.POV.Location += Offset;
		}
	}
}
