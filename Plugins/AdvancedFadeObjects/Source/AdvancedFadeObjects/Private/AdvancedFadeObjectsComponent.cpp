// Copyright 2024 BitProtectStudio. All Rights Reserved.

#include "AdvancedFadeObjectsComponent.h"
#include "TimerManager.h"
#include "Engine.h"

#include "Kismet/GameplayStatics.h"


UAdvancedFadeObjectsComponent::UAdvancedFadeObjectsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Add first collision type
	objectTypes.Add(ECC_WorldStatic);
}

// Called when the game starts
void UAdvancedFadeObjectsComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController *plrCtrl_ = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (plrCtrl_->IsLocalController())
	{
		GetOwner()->GetWorld()->GetTimerManager().SetTimer(timerHandle_AddObjectsTimer, this, &UAdvancedFadeObjectsComponent::AddObjectsToHideTimer, addObjectInterval, true);
		GetOwner()->GetWorld()->GetTimerManager().SetTimer(timerHandle_ObjectComputeTimer, this, &UAdvancedFadeObjectsComponent::FadeObjWorkerTimer, calcFadeInterval, true);

		GetOwner()->GetWorld()->GetTimerManager().PauseTimer(timerHandle_ObjectComputeTimer);
		GetOwner()->GetWorld()->GetTimerManager().PauseTimer(timerHandle_AddObjectsTimer);

		SetActivate(bIsActivate);
	}
}

// Called every frame
void UAdvancedFadeObjectsComponent::TickComponent(const float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAdvancedFadeObjectsComponent::AddObjectsToHideTimer()
{
	 UGameplayStatics::GetAllActorsOfClass(this, playerClass, characterArray);

    for (AActor* currentActor_ : characterArray)
    {
        const FVector traceStart_ = GEngine->GetFirstLocalPlayerController(GetOwner()->GetWorld())->PlayerCameraManager->GetCameraLocation();
        const FVector traceEnd_ = currentActor_->GetActorLocation();
        FVector traceLength_ = traceStart_ - traceEnd_;
        const FQuat acQuat_ = currentActor_->GetActorQuat();

        if (traceLength_.Size() < workDistance)
        {
            FCollisionQueryParams traceParams_(TEXT("FadeObjectsTrace"), bIsTraceComplex, GetOwner());

            traceParams_.AddIgnoredActors(actorsIgnore);
            traceParams_.bReturnPhysicalMaterial = false;
            // Not tracing complex uses the rough collision instead making tiny objects easier to select.
            traceParams_.bTraceComplex = bIsTraceComplex;

            TArray<FHitResult> hitArray_;
            TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes_;

            // Convert ECollisionChannel to ObjectType
            for (int i = 0; i < objectTypes.Num(); ++i)
            {
                traceObjectTypes_.Add(UEngineTypes::ConvertToObjectType(objectTypes[i].GetValue()));
            }

            // Check distance between camera and player for new object to fade, and add this in array
            GetOwner()->GetWorld()->SweepMultiByObjectType(hitArray_, traceStart_, traceEnd_, acQuat_, traceObjectTypes_,
                                                           FCollisionShape::MakeCapsule(capsuleRadius, capsuleHalfHeight), traceParams_);

            for (int iHit_ = 0; iHit_ < hitArray_.Num(); ++iHit_)
            {
                if (hitArray_[iHit_].bBlockingHit && IsValid(hitArray_[iHit_].GetComponent()) && !fadeObjectsHit.Contains(hitArray_[iHit_].GetComponent()))
                {
                    fadeObjectsHit.AddUnique(hitArray_[iHit_].GetComponent());
                }
            }
        }
    }

    // Make fade array after complete GetAllActorsOfClass loop
    for (int iObject_ = 0; iObject_ < fadeObjectsHit.Num(); ++iObject_)
    {
        // If not contains this component in fadeObjectsTemp
        if (!fadeObjectsTemp.Contains(fadeObjectsHit[iObject_]))
        {
            TArray<UMaterialInterface*> lBaseMaterials_;
            TArray<UMaterialInstanceDynamic*> lMidMaterials_;

            lBaseMaterials_.Empty();
            lMidMaterials_.Empty();

            fadeObjectsTemp.AddUnique(fadeObjectsHit[iObject_]);

            // For loop all materials ID in object
            for (int nM = 0; nM < fadeObjectsHit[iObject_]->GetNumMaterials(); ++nM)
            {
                lMidMaterials_.Add(UMaterialInstanceDynamic::Create(fadeMaterial, fadeObjectsHit[iObject_]));
                lBaseMaterials_.Add(fadeObjectsHit[iObject_]->GetMaterial(nM));

                // Set new material on object
                fadeObjectsHit[iObject_]->SetMaterial(nM, lMidMaterials_.Last());
            }
            // Create new fade object in array of objects to fade
            FFadeObjStruct newObject_;
            newObject_.NewElement(fadeObjectsHit[iObject_], lBaseMaterials_, lMidMaterials_, immediatelyFade, true);
        	newObject_.cameraCollision = fadeObjectsHit[iObject_]->GetCollisionResponseToChannel(ECC_Camera);
            // Add object to array
            fadeObjects.Add(newObject_);

            // Set collision on Primitive Component
            fadeObjectsHit[iObject_]->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
        }
    }

    // Set hide to visible true if contains
    for (int fOT = 0; fOT < fadeObjectsTemp.Num(); ++fOT)
    {
        if (!fadeObjectsHit.Contains(fadeObjectsTemp[fOT]))
        {
            fadeObjects[fOT].SetHideOnly(false);
        }
    }

    // Clear array
    fadeObjectsHit.Empty();
}

void UAdvancedFadeObjectsComponent::FadeObjWorkerTimer()
{
	if (fadeObjects.Num() > 0)
	{
		// For loop all fade objects
		for (int iObject_ = 0; iObject_ < fadeObjects.Num(); ++iObject_)
		{
			// Index of iteration 
			int fnID_ = iObject_;

			float adaptiveFade_;

			if (fnID_ == fadeObjects.Num())
			{
				adaptiveFade_ = nearObjectFade;
			}
			else
			{
				adaptiveFade_ = farObjectFade;
			}

			// For loop fadeMID array
			for (int t = 0; t < fadeObjects[iObject_].fadeMID.Num(); ++t)
			{
				float targetF_;

				const float currentF = fadeObjects[iObject_].fadeCurrent;

				if (fadeObjects[iObject_].bToHide)
				{
					targetF_ = adaptiveFade_;
				}
				else
				{
					targetF_ = 1.0f;
				}

				const float newFade_ = FMath::FInterpConstantTo(currentF, targetF_, GetOwner()->GetWorld()->GetDeltaSeconds(), fadeRate);

				fadeObjects[iObject_].fadeMID[t]->SetScalarParameterValue("Fade", newFade_);

				currentFade = newFade_;

				fadeObjects[iObject_].SetFadeAndHide(newFade_, fadeObjects[iObject_].bToHide);
			}
			// remove index in array
			if (currentFade == 1.0f)
			{
				for (int iBaseMat_ = 0; iBaseMat_ < fadeObjects[fnID_].baseMatInterface.Num(); ++iBaseMat_)
				{
					fadeObjects[fnID_].primitiveComp->SetMaterial(iBaseMat_, fadeObjects[fnID_].baseMatInterface[iBaseMat_]);
				}

				fadeObjects[fnID_].primitiveComp->SetCollisionResponseToChannel(ECC_Camera, fadeObjects[fnID_].cameraCollision);
				fadeObjects.RemoveAt(fnID_);
				fadeObjectsTemp.RemoveAt(fnID_);
			}
		}
	}
}

void UAdvancedFadeObjectsComponent::SetActivate(bool setActivate)
{
	bIsActivate = setActivate;

	if (!bIsActivate)
	{
		GetOwner()->GetWorld()->GetTimerManager().PauseTimer(timerHandle_ObjectComputeTimer);
		GetOwner()->GetWorld()->GetTimerManager().PauseTimer(timerHandle_AddObjectsTimer);
	}
	else
	{
		GetOwner()->GetWorld()->GetTimerManager().UnPauseTimer(timerHandle_ObjectComputeTimer);
		GetOwner()->GetWorld()->GetTimerManager().UnPauseTimer(timerHandle_AddObjectsTimer);
	}
}

void UAdvancedFadeObjectsComponent::SetEnable(bool setEnable)
{
	bIsEnabled = setEnable;
}
