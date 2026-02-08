#include "Public/PrincipalCharacterMovementComponent.h"
#include "GameFramework/Character.h"

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

void UPrincipalCharacterMovementComponent::FSavedMove_Principal::Clear()
{
	FSavedMove_Character::Clear();
	
	Saved_bWantsToSprint = 0;
}

UPrincipalCharacterMovementComponent::UPrincipalCharacterMovementComponent()
{
}
