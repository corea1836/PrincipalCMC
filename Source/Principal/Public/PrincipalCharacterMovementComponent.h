// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/Object.h"
#include "PrincipalCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class PRINCIPAL_API UPrincipalCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	class FSavedMove_Principal : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;
		
		// 데이터 저장 변수
		uint8 Saved_bWantsToSprint:1;
		
		// 1. 기록 : 현재 캐릭터 상태를 SavedMove_Principal 에 기록
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
		// 2. 최적화 : 이전 이동 기록과 합칠 수 있으면 합침
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		// 3. 압축 : 서버로 보낼 때 1 Byte(uint8) 로 압축
		virtual uint8 GetCompressedFlags() const override;
		// 4. 복구 : 서버 보정 발생 시, 이 기록(SavedMove_Principal) 을 다시 적용
		virtual void PrepMoveFor(ACharacter* C) override;
		// 5. 초기화 : 사용이 끝난 객체는 삭제
		virtual void Clear() override;		
	};
	
	class FNetworkPredictionData_Client_Principal : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Principal(const UCharacterMovementComponent& ClientMovement);
		
		typedef FNetworkPredictionData_Client_Character Super;
		
		virtual FSavedMovePtr AllocateNewMove() override;
	};
	
	bool Safe_bWantsToSprint;
	
public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
public:
	UPrincipalCharacterMovementComponent();
};
