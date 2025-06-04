#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "AbilitySystemInterface.h"
#include "EMSActorSaveInterface.h"
#include "EmberCharacter.generated.h"

class UAC_BuildComponent;
class UNiagaraSystem;
struct FMeleeTraceInstanceHandle;
class UNiagaraComponent;
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

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
private:
	void SetupEmberInputComponent() const;
	
public: /* Character */
	virtual UMeleeTraceComponent* GetMeleeTraceComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "Glider")
	USkeletalMeshComponent* GetGliderMesh() const;
	
	UPROPERTY(EditAnywhere, Category = "InteractionSystem")
	TObjectPtr<class UInteractionComponent> InteractionComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeleeTraceComponent* MeleeTraceComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> VisualCharacterMesh;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> GliderMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Overlay", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> OverlayStaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Overlay", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> OverlaySkeletalMesh;
	
protected:
	UPROPERTY(EditAnywhere, Category = "HpBar")
	TSubclassOf<class UUserWidget> HpBarWidgetClass;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UEmberWidgetComponent> HpBarWidget;
	
public:/* Build System */
	
protected:
	TObjectPtr<UAC_BuildComponent> BuildComponent;
public: /* VFX */
	UFUNCTION(BlueprintCallable, Category = "Effects")
	virtual UNiagaraComponent*	  GetWeaponTrailComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "Effects")
	virtual UNiagaraComponent*	  GetDualWeaponTrailComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "Effects")
	virtual UNiagaraSystem* GetOverlayHitEffect() const;
	
	UFUNCTION(BlueprintCallable, Category = "Effects")
	void SetOverlayHitEffect(UNiagaraSystem* InHitEffectAsset);
	
	UFUNCTION(BlueprintCallable, Category = "Effects")
	void PlayHitEffectAtLocation(const FVector& Location);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effects", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> WeaponTrailComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effects", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> DualWeaponTrailComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Effects", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> OverlayHitEffect;
	
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
	virtual void NotifyLocomotionModeChanged(const FGameplayTag& PreviousLocomotionMode) override;
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
	virtual void Input_OnGlide();
	virtual void Input_OnRagdoll();
	virtual void Input_OnRoll();
	virtual void Input_OnRotationMode();
	virtual void Input_OnViewMode();
	virtual void Input_OnSwitchShoulder();
	virtual void Input_OnQuickSlot(int32 PressedIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EmberCharacter")
	TObjectPtr<UEmberInputHandlerComponent> InputHandler;

	friend class UEmberInputHandlerComponent;

protected:
	/** 글라이드 시 전방(Forward) 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float GlideForwardSpeed = 800.0f;

	/** 글라이드 시 하강 속도(z축) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float GlideDescendSpeed = 200.0f;

	/** 글라이드 시 중력 스케일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float GlideGravityScale = 0.f;

	/** 기본 낙하 */
	float DefaultGravityScale = 1.0f;

protected:
	//UFUNCTION()
	//void HandleMeleeTraceHit(UMeleeTraceComponent* ThisComponent, AActor* HitActor, const FVector& HitLocation, const FVector& HitNormal, FName HitBoneName, FMeleeTraceInstanceHandle TraceHandle);
	
protected: /* Inventory */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "EmberCharacter")
	TObjectPtr<class UUserItemManger> EmberItemManager;
	
};


