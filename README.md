# Intro
Principal CMC 는 언리얼엔진의 CMC(CharacterMovementComponent) 에서 제공하지 않는 움직임을 구현한 Custom CMC 입니다.
새로운 움직임은 Sprint, Crouch, Prone ... 등을 구현하며, 최종 목표는 언리얼 네트워크와 연동입니다.
Custom CMC 의 주된 구현은 새로운 움직임의 물리연산과 구현, 네트워크 전송으로 시작해 서버 검증에 의한 클라이언트 보정까지 이어집니다.

문서에서는 그 원리와 구현 방법, CPP 학습에 대해 자세히 기록합니다.

# Overview
언리얼 CMC  는 클라이언트 예측 이동 -> 서버 검증 -> 클라이언트 보정의 순서로 이루어집니다.
키보드, 조이스틱 입력은  서버 검증 이후 이동을 동기화 하면 플레이어는 지연을 체감하므로, 클라이언트 선이동 후 서버 보정에 의한 잘못된 움직임을 보정합니다.

Custom CMC 는 언리얼엔진의 Tick 흐름에 끼어들어 새롭게 정의한 움직임을 클라이언트와 서버에 적용해야합니다.
다행히 언리얼엔진은 사용자가 커스텀 할 수 있는 부분이 컴포넌트화 되어있어 Tick 사이클에 커스텀 로직을 적용하기 수월합니다.
크게 수정할 부분은 아래와 같습니다.
- ACharacter : 캐릭터 상태를 수정하고 움직임을 최종적으로 적용할 액터
- UCharacterMovementComponent : 움직임 관련 로직(물리연산, 상태 변경, 서버 동기화, 보정 등) 구현

## Custom CMC Setting
ACharacter 는 내부 생성자에서 기본 CMC 를 생성하기 때문에 생성자에서 Custom CMC 를 생성하도록 설정해야합니다.
```c++
// PrincipalCharacter.cpp
APrincipalCharacter::APrincipalCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPrincipalCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)) {
    // ObjectInitializer 를 통해 기본 생성 컴포넌트를 교체합니다. CharacterMovementComponent -> CustomCharacterMovementComponent
    PrincipalCharacterMovementComponent = Cast<UPrincipalCharacterMovementComponent>(GetCharacterMovement());
    ...
}
```

## CMC Lifecycle
