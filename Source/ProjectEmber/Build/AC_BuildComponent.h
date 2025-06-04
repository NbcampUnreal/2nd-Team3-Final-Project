// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Build/S_Buildable.h"
#include "AC_BuildComponent.generated.h"


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

    //블루프린트에 사용
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

    //블루프린트에서 사용
    UFUNCTION(BlueprintCallable, Category = "Build")
    void SpwanBuild();

    //SpwanBuild도 블루프린트에서 사용
    UFUNCTION(BlueprintCallable, Category = "Build")
    void ChangeMesh();

    UFUNCTION(BlueprintCallable, Category = "Build")
    bool DetectBuildBoxes(FTransform& OutTransform);

public:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UCameraComponent* Camera;

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

    // 핵심: 구조체 배열로 된 Buildables
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FBuildableData> Buildables;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* HitActor;

    // 건축물 datatable
    UPROPERTY(EditAnywhere, Category = "Build")
    UDataTable* BuildData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool Found;

    FTimerHandle BuildDelayHandle;

    UMaterialInterface* GreenMaterial;

    UMaterialInterface* RedMaterial;
};
