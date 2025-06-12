// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Build/BuildableData.h"
#include "AC_BuildComponent.generated.h"


class UAlsCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UAC_BuildComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_BuildComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;



public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    //�������Ʈ�� ���
    UFUNCTION(BlueprintCallable, Category = "Build")
    void LaunchBuildMode();

    UFUNCTION(BlueprintCallable, Category = "Build")
    void BuildDelay();

    UFUNCTION(BlueprintCallable, Category = "Build")
    void BuildCycle();

    UFUNCTION(BlueprintCallable, Category = "Build")
    void StopBuildMode();

    UFUNCTION(BlueprintCallable, Category = "Build")
    void SpawnBuildGoust();

    UFUNCTION(BlueprintCallable, Category = "Build")
    void GiveBuildColor(bool bIsGreen);

    //�������Ʈ���� ���
    UFUNCTION(BlueprintCallable, Category = "Build")
    void SpwanBuild();

    //SpwanBuild�� �������Ʈ���� ���
    UFUNCTION(BlueprintCallable, Category = "Build")
    void ChangeMesh();

    UFUNCTION(BlueprintCallable, Category = "Build")
    bool DetectBuildBoxes(FTransform& OutTransform);

public:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAlsCameraComponent> Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UStaticMeshComponent* BuildGhost;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UPrimitiveComponent* HitComponent;

    // Variables
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsBuildModeOn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanBuild;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform BuildTransform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACharacter* PlayerReference;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BuildID;

    // �ٽ�: ����ü �迭�� �� Buildables
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FBuildableData> Buildables;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* HitActor;

    // ���๰ datatable
    UPROPERTY(EditAnywhere, Category = "Build")
    UDataTable* BuildData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool Found;

    FTimerHandle BuildDelayHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> GreenMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> RedMaterial;
};
