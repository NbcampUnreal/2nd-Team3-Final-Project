#include "CustomAICharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h" // 이하 헤더추가
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

//////////////////////////////////////////////////////////////////////////
// ACustomAICharacter

ACustomAICharacter::ACustomAICharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 네비게이션 Invoker 관련 변수 초기화
	NavGenerationRadius = 100.0f;
	NavRemovalRadius = 150.0f;

	// Navigation Invoker 컴포넌트 생성 및 초기값 셋업.
	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	// SetGenerationRadii 함수를 사용하여 생성 반경과 제거 반경 설정
	NavInvoker->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);

	// AI Modifier 테스트 관련 변수 초기화
	bIsSucceeded = false;
	bIsMoving = false;
	AcceptanceRadius = 50.0f; // 블루프린트에서 5.0으로 설정된 것으로 보이지만, 언리얼 단위로 변환

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

	// AI Modifier 테스트 위해 BeginPlay() 추가
void ACustomAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// AI 컨트롤러 찾기
	AIController = Cast<AAIController>(GetController());

	// AI 컨트롤러가 없으면 자동으로 생성하지 않음 (필요시 생성 코드 추가)
	if (AIController)
	{
		// 디버깅 에러 방지를 위해 언비인딩 코드 실행
		AIController->ReceiveMoveCompleted.RemoveDynamic(this, &ACustomAICharacter::OnMoveCompleted);
		// 이동 완료 이벤트 델리게이트 바인딩
		AIController->ReceiveMoveCompleted.AddDynamic(this, &ACustomAICharacter::OnMoveCompleted);

		// 타겟 포인트 찾기
		FindTargetPoints(); 
		StartMoving();
	}
}

//////////////////////////////////////////////////////////////////////////
// AI Modifier Texst 인풋 로직 구현 (Invoker로직에서 수정)

void ACustomAICharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	else // Input Mapping Context가 없는 경우(AI인 경우) 컨트롤러 추가.
	{
		// 컨트롤러가 변경되었고 AI 컨트롤러인 경우
		AIController = Cast<AAIController>(Controller);
		if (AIController)
		{
			AIController->ReceiveMoveCompleted.AddDynamic(this, &ACustomAICharacter::OnMoveCompleted);
		}
	}
}

void ACustomAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACustomAICharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACustomAICharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACustomAICharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACustomAICharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//////////////////////////////////////////////////////////////////////////
// AI Modifier 테스트용 Movement Logic 구현.

void ACustomAICharacter::FindTargetPoints()
{
	// 타겟 포인트가 설정되어 있지 않은 경우 자동으로 찾기
	// "||"는 A OR B로 A 혹은 B가 True일 경우 True를 반환하는 논리연산자입니다. 
	// 여기서는 역논리 연산자가 bool 변수 앞에 붙어있으므로 bool변수가 하나라도 false 일 경우 True로 판정합니다.
	if (!Target || !Target2) 
	{
		TArray<AActor*> FoundTargets;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), FoundTargets);

		if (FoundTargets.Num() >= 2)
		{
			Target = FoundTargets[0];
			Target2 = FoundTargets[1];
			UE_LOG(LogTemplateCharacter, Display, TEXT("Found TargetPoints: %s and %s"),
				*Target->GetName(), *Target2->GetName());
		}
		else
		{
			UE_LOG(LogTemplateCharacter, Warning, TEXT("Not enough TargetPoints found in the level, need at least 2!"));
		}
	}
}

void ACustomAICharacter::StartMoving()
{
	// 타겟 포인트를 찾고 이동 시작
	FindTargetPoints();
	MoveToTarget();
}

void ACustomAICharacter::MoveToTarget()
{
	if (!AIController)
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("AIController is not valid! Make sure the character is possessed by an AIController."));
		return;
	}

	if (bIsMoving)
	{
		// 이미 이동 중이면 중복 호출 방지
		return;
	}

	// IsSucceeded 값에 따라 타겟 선택
	AActor* SelectedTarget = bIsSucceeded ? Target : Target2;

	if (SelectedTarget)
	{
		bIsMoving = true;

		// AI MoveTo 함수 호출
		FVector TargetLocation = SelectedTarget->GetActorLocation();
		EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(
			TargetLocation,
			AcceptanceRadius,
			true,  // 목적지에 오버랩 되면 도착으로 판정할지 여부.
			true,  // 경로 찾기 사용
			false, // 프로젝션 사용 안함
			true   // 네비게이션 데이터 사용
		);

		if (MoveResult == EPathFollowingRequestResult::Failed)
		{
			UE_LOG(LogTemplateCharacter, Warning, TEXT("Failed to start movement to target!"));
			bIsMoving = false;
		}
		else
		{
			UE_LOG(LogTemplateCharacter, Display, TEXT("Moving to %s (IsSucceeded: %s)"),
				*SelectedTarget->GetName(), bIsSucceeded ? TEXT("True") : TEXT("False"));
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("Selected target is not valid! Make sure Target and Target2 are set."));
	}
}

void ACustomAICharacter::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	bIsMoving = false;

	// 이동 결과에 따라 IsSucceeded 값 토글
	if (Result == EPathFollowingResult::Success)
	{
		// 성공적으로 이동 완료됨
		bIsSucceeded = !bIsSucceeded;  // 값 토글
		UE_LOG(LogTemplateCharacter, Display, TEXT("Move completed successfully. IsSucceeded toggled to: %s"),
			bIsSucceeded ? TEXT("True") : TEXT("False"));

		// 지연 후 다음 이동 시작
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ACustomAICharacter::MoveToTarget, 0.5f, false);
	}
	else
	{
		// 이동 실패
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Move failed with result: %d"), static_cast<int32>(Result));

		// 실패 시에도 다시 시도
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ACustomAICharacter::MoveToTarget, 1.0f, false);
	}
}