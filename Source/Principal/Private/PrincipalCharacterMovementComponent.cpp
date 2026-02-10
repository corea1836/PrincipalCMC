#include "Public/PrincipalCharacterMovementComponent.h"
#include "GameFramework/Character.h"

#pragma region Saved Move
void UPrincipalCharacterMovementComponent::FSavedMove_Principal::SetMoveFor(ACharacter* C, float InDeltaTime,
	FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	
	UPrincipalCharacterMovementComponent* CharacterMovement = Cast<UPrincipalCharacterMovementComponent>(C->GetCharacterMovement());
	
	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
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

#pragma endregion 