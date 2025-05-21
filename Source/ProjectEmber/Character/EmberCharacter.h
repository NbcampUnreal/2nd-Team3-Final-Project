#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "AbilitySystemInterface.h"
#include "EMSActorSaveInterface.h"
#include "EmberCharacter.generated.h"

class UGameMenuWidget;
class UEmberLayerBase;
struct FInputActionInstance;
struct FGameplayAbilitySpec;
class UAlsCameraComponent;
class UEmberInputHandlerComponent;
class UMeleeTraceComponent;
struct FInputActionValue;

UCLASS()
class PROJECTEMBER_API AEmberCharacter : public AAlsCharacter, public IAbilitySystemInterface, public IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	AEmberCharacter();

public: /* Character */
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(EditAnywhere, Category = "InteractionSystem")
	TObjectPtr<class UInteractionComponent> InteractionComponent;

	UPROPERTY(EditAnywhere, Category="Interaction")
	UAnimMontage* InteractMontage;

public:
	virtual UMeleeTraceComponent* GetMeleeTraceComponent() const;

protected:
	void SetupEmberInputComponent() const;

	UPROPERTY(EditAnywhere, Category = "HpBar")
	TSubclassOf<class UUserWidget> HpBarWidgetClass;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UEmberWidgetComponent> HpBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeleeTraceComponent* MeleeTraceComponent;
public: /* AbilitySystem */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UFUNCTION()
	virtual void OnOutOfHealth();

	void AbilityInputPressed(int32 InputID);
	FGameplayAbilitySpec* GetSpecFromOverlayMode(const bool IsRightInput = false) const;
	void TryAbilityFromOnAim(const bool bPressed);
protected:
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartRightInputAbilities;
	
	bool bClientAbility{false};
public: /* Als */
	virtual void NotifyControllerChanged() override; // 컨트롤러 변경 시 매핑 등록/해제
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;

protected:
	virtual bool StartMantlingInAir() override; // 공중 자동 파쿠르막기 (AlsCharacter::Tick 에서 그냥 주석처리하면 될거같은데 수정해도 될지 모르겠음)
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

protected: /* Camera */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "EmberCharacter")
	TObjectPtr<UAlsCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmberCharacter|Input")
	float LookUpMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmberCharacter|Input")
	float LookRightMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmberCharacter|Input")
	float LookUpRate{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmberCharacter|Input")
	float LookRightRate{240.0f};

public: /* Input */
	void BindUIInput(UGameMenuWidget* Layer);
protected:
	virtual void Input_OnLookMouse(const FInputActionValue& ActionValue);
	virtual void Input_OnLook(const FInputActionValue& ActionValue);
	virtual void Input_OnMove(const FInputActionValue& ActionValue);
	virtual void Input_OnSprint(const FInputActionValue& ActionValue);
	virtual void Input_OnWalk();
	virtual void Input_OnCrouch();
	virtual void Input_OnJump(const FInputActionValue& ActionValue);
	virtual void Input_OnAim(const FInputActionValue& ActionValue);
	virtual void Input_OnRagdoll();
	virtual void Input_OnRoll();
	virtual void Input_OnRotationMode();
	virtual void Input_OnViewMode();
	virtual void Input_OnSwitchShoulder();
	virtual void Input_OnQuickSlot(int32 PressedIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EmberCharacter")
	TObjectPtr<UEmberInputHandlerComponent> InputHandler;

	friend class UEmberInputHandlerComponent;

protected: /* Inventory */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "EmberCharacter")
	TObjectPtr<class UUserItemManger> EmberItemManager;
	
};


