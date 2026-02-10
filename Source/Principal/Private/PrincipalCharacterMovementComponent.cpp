#include "Public/PrincipalCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#pragma region Saved Move
void UPrincipalCharacterMovementComponent::FSavedMove_Principal::SetMoveFor(ACharacter* C, float InDeltaTime,
	FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	
	UPrincipalCharacterMovementComponent* CharacterMovement = Cast<UPrincipalCharacterMovementComponent>(C->GetCharacterMovement());
	
	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
	Saved_bPrevWantsToCrouch = CharacterMovement->Safe_bPrevWantsToCrouch;
}

bool UPrincipalCharacterMovementComponent::FSavedMove_Principal::CanCombineWith(const FSavedMovePtr& NewMove,
                                                                                ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Principal* NewPrincipalMove = static_cast<FSavedMove_Principal*>(NewMove.Get());
	
	if (Saved_bWantsToSprint != NewPrincipalMove->Saved_bWantsToSprint)
	{
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

uint8 UPrincipalCharacterMovementComponent::FSavedMove_Principal::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	
	if (Saved_bWantsToSprint) Result |= FLAG_Custom_0;
	
	return Result;
}

void UPrincipalCharacterMovementComponent::FSavedMove_Principal::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);
	
	UPrincipalCharacterMovementComponent* CharacterMovement = Cast<UPrincipalCharacterMovementComponent>(C->GetCharacterMovement());
	
	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
	CharacterMovement->Safe_bPrevWantsToCrouch = Saved_bPrevWantsToCrouch;
}

#pragma endregion

void UPrincipalCharacterMovementComponent::FSavedMove_Principal::Clear()
{
	FSavedMove_Character::Clear();
	
	Saved_bWantsToSprint = 0;
}

UPrincipalCharacterMovementComponent::FNetworkPredictionData_Client_Principal::FNetworkPredictionData_Client_Principal(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

#pragma region Client Network Predication Data

FSavedMovePtr UPrincipalCharacterMovementComponent::FNetworkPredictionData_Client_Principal::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Principal());
}

#pragma endregion

#pragma region CMC

UPrincipalCharacterMovementComponent::UPrincipalCharacterMovementComponent()
{
	NavAgentProps.bCanCrouch = true;	
}

void UPrincipalCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	PrincipalCharacterOwner = Cast<APrincipalCharacter>(GetOwner());
}

FNetworkPredictionData_Client* UPrincipalCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);
	
	if (ClientPredictionData == nullptr)
	{
		UPrincipalCharacterMovementComponent* MutableThis = const_cast<UPrincipalCharacterMovementComponent*>(this);
		
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Principal(*this);
		MutableThis->ClientPredictionData->MaxClientSmoothingDeltaTime = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	return ClientPredictionData;
}

void UPrincipalCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	
	Safe_bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

void UPrincipalCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	
	if (MovementMode == MOVE_Walking)
	{
		if (Safe_bWantsToSprint)
		{
			MaxWalkSpeed = Sprint_MaxWalkSpeed;
		}
		else
		{
			MaxWalkSpeed = Walk_MaxWalkSpeed;
		}
	}
	
	Safe_bPrevWantsToCrouch = bWantsToCrouch;
}

bool UPrincipalCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide);
}

bool UPrincipalCharacterMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

void UPrincipalCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (MovementMode == MOVE_Walking && !bWantsToCrouch && Safe_bPrevWantsToCrouch)
	{
		FHitResult PotentialSlideSurface;
		if (Velocity.SizeSquared() > pow(Slide_MinSpeed, 2) && GetSlideSurface(PotentialSlideSurface))
		{
			EnterSlide();
		}
	}
	
	if (IsCustomMovementMode(CMOVE_Slide) && !bWantsToCrouch)
	{
		ExitSlide();
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UPrincipalCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}
}

void UPrincipalCharacterMovementComponent::EnterSlide()
{
	bWantsToCrouch = true;
	Velocity += Velocity.GetSafeNormal2D() * Slide_EmterImpulse;
	SetMovementMode(MOVE_Custom, CMOVE_Slide);
}

void UPrincipalCharacterMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;
	
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
	SetMovementMode(MOVE_Walking);
}

void UPrincipalCharacterMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	
	RestorePreAdditiveRootMotionVelocity();
	
	FHitResult SurfaceHit;
	if (!GetSlideSurface(SurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
	{
		ExitSlide();
		StartNewPhysics(deltaTime, Iterations);
		return;
	}
	
	Velocity += Slide_GravityForce * FVector::DownVector * deltaTime;
	
	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > .5)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}
	
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(deltaTime, Slide_Fraction, true, GetMaxBrakingDeceleration());
	}
	ApplyRootMotionToVelocity(deltaTime);
	
	Iterations++;
	bJustTeleported = false;
	
	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult Hit(1.f);
	FVector Adjusted = Velocity * deltaTime;
	FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, SurfaceHit.Normal).GetSafeNormal();
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir, SurfaceHit.Normal).ToQuat();
	SafeMoveUpdatedComponent(Adjusted, NewRotation, true, Hit);
	
	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}
	
	FHitResult NewSurfaceHit;
	if (!GetSlideSurface(NewSurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
	{
		ExitSlide();
	}
	
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
}

bool UPrincipalCharacterMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	return GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ProfileName, PrincipalCharacterOwner->GetIgnoreCharacterParams());
}
#pragma endregion

#pragma region Input

void UPrincipalCharacterMovementComponent::SprintPressed()
{
	Safe_bWantsToSprint = true;
}

void UPrincipalCharacterMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

void UPrincipalCharacterMovementComponent::CrouchPressed()
{
	bWantsToCrouch = !bWantsToCrouch;
}

bool UPrincipalCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}
#pragma endregion 
