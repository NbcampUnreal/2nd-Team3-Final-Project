// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/AC_BuildComponent.h"
#include "BuildInterface.h"
#include "Character/EmberCharacter.h"
#include "ALSCamera/Public/AlsCameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "EmberLog/EmberLog.h"

// Sets default values for this component's properties
UAC_BuildComponent::UAC_BuildComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Script/Engine.DataTable'/Game/_Data/BuildingSystem/DT_Buildable.DT_Buildable'"));
	if (DataTableRef.Succeeded())
	{
		BuildData = DataTableRef.Object;

	}
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/_Data/BuildingSystem/BuildUI/UI_BuildPreview.UI_BuildPreview_C")); // 확장자 _C 붙이기 필수
	if (WidgetClassFinder.Succeeded())
	{
		BuildPreviewWidgetClass = WidgetClassFinder.Class;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("BuildPreviewWidgetClass not found!"));
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder1(TEXT("/Game/_Data/BuildingSystem/BuildUI/UI_BuildHealth.UI_BuildHealth_C"));
	if (WidgetClassFinder1.Succeeded())
	{
		BuildHealthWidgetClass = WidgetClassFinder1.Class;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("BuildHealthWidgetClass not found!"));
	}
}


// Called when the game starts
void UAC_BuildComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		PlayerReference = Cast<AEmberCharacter>(Owner);
	}

	if (!BuildData)
	{
		return;
	}
	TArray<FName> RowNames = BuildData->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		static const FString ContextString(TEXT("BuildDataContext")); //Ȥ�� �� ���� ����
		FBuildableData* Row = BuildData->FindRow<FBuildableData>(RowName, ContextString);

		if (Row)
		{
			Buildables.Add(*Row);
		}
	}

	if (BuildPreviewWidgetClass)
	{
		BuildPreviewWidget = CreateWidget<UUserWidget>(GetWorld(), BuildPreviewWidgetClass);
		if (BuildPreviewWidget)
		{
			BuildPreviewWidget->AddToViewport();
			BuildPreviewWidget->SetVisibility(ESlateVisibility::Hidden); // 기본은 숨김
		}
	}
	if (BuildHealthWidgetClass)
	{
		BuildHealthWidget = CreateWidget<UUserWidget>(GetWorld(), BuildHealthWidgetClass);
		if (BuildHealthWidget)
		{
			BuildHealthWidget->AddToViewport();
			BuildHealthWidget->SetVisibility(ESlateVisibility::Hidden); // 기본은 숨김
		}
	}
}


// Called every frame
void UAC_BuildComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAC_BuildComponent::SpawnBuildGoust()
{
	if (!PlayerReference || !Buildables.IsValidIndex(BuildID))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid PlayerReference or BuildID"));
		return;
	}

	// BuildGhost ������Ʈ ����
	BuildGhost = NewObject<UStaticMeshComponent>(PlayerReference);
	if (!BuildGhost)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create BuildGhost component"));
		return;
	}

	BuildGhost->RegisterComponent(); // ���忡 ���
	BuildGhost->AttachToComponent(PlayerReference->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	BuildGhost->SetRelativeTransform(BuildTransform);

	// Static Mesh ����
	const FBuildableData& Data = Buildables[BuildID];
	BuildGhost->SetStaticMesh(Data.Mesh);

	// �浹 ��Ȱ��ȭ
	BuildGhost->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UAC_BuildComponent::LaunchBuildMode()
{
	if (bIsBuildModeOn)
	{
		StopBuildMode();
		bIsBuildModeOn = false;
		return;
	}

	bIsBuildModeOn = true;

	BuildCycle();
}

void UAC_BuildComponent::BuildDelay()
{
	if (!bIsBuildModeOn)
	{
		StopBuildMode();
		bIsBuildModeOn = false;
		return;
	}

	// 0.01�� �� BuildCycle ȣ��
	GetWorld()->GetTimerManager().SetTimer(BuildDelayHandle, this, &UAC_BuildComponent::BuildCycle, 0.01f, false);
}

void UAC_BuildComponent::BuildCycle()
{
	if (!Camera || Buildables.Num() <= BuildID) return;
	
	FVector WorldLocation;
	FVector WorldDirection;
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		int32 ViewportSizeX, ViewportSizeY;
		PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
		
		FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);
		
		PC->DeprojectScreenPositionToWorld(
			ScreenCenter.X,
			ScreenCenter.Y,
			WorldLocation,
			WorldDirection
		);
	}
	
	// 2. ����Ʈ���̽� ����/�� ����
	FVector Start = WorldLocation + WorldDirection * 350.f;
	FVector End   = WorldLocation + WorldDirection * 1000.f;

	// 3. ����Ʈ���̽�
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner()); // �ڽ� ����
	Params.bTraceComplex = false;

	// Buildables���� TraceChannel ����
	ECollisionChannel TraceChannel = ECC_Visibility; // �⺻ ä��
	if (Buildables.IsValidIndex(BuildID))
	{

		TraceChannel = UEngineTypes::ConvertToCollisionChannel(Buildables[BuildID].TraceChannel);
	}

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		TraceChannel,
		Params
	);
	if (bHit)
	{
		BuildTransform.SetLocation(HitResult.ImpactPoint);
		// ✅ 수정된 회전 처리 (카메라가 바라보는 방향의 Yaw만 유지)
		FRotator LookRotation = WorldDirection.Rotation();
		LookRotation.Pitch = 0.f;
		LookRotation.Roll = 0.f;
		BuildTransform.SetRotation(FQuat(LookRotation));

		HitActor = HitResult.GetActor();
		HitComponent = HitResult.GetComponent();
		if (IsValid(BuildGhost))
		{
			FTransform TempTransform;
			Found = DetectBuildBoxes(TempTransform);
			if (Found)
			{
				BuildTransform = TempTransform;
			}
			GiveBuildColor(true);
		}
		else
		{
			SpawnBuildGoust();
		}
	}
	else
	{
		BuildTransform.SetLocation(End);
		BuildTransform.SetRotation(WorldDirection.ToOrientationQuat());
		HitActor = nullptr;
		HitComponent = nullptr;
		if (IsValid(BuildGhost))
		{
			GiveBuildColor(false);
		}
		else
		{
			SpawnBuildGoust();
		}
	}

	UpdateBuildPreviewUI();

	BuildDelay();
}

void UAC_BuildComponent::StopBuildMode()
{
	bIsBuildModeOn = false;
	bCanBuild = false;

	if (IsValid(BuildGhost) && PlayerReference)
	{
		BuildGhost->DestroyComponent();
		BuildGhost = nullptr;
	}

	if (BuildPreviewWidget)
	{
		BuildPreviewWidget->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UAC_BuildComponent::GiveBuildColor(bool bIsGreen)
{
	bCanBuild = bIsGreen;

	if (!BuildGhost) return;

	// 2. BuildGhost�� ��Ƽ���� �� Ȯ��
	int32 MaterialCount = BuildGhost->GetNumMaterials();
	
	/*
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> GreenMatObj(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/_Art/_EasyBuilding/Materials/Instances/Dummy/MI_Can_Build.MI_Can_Build'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> RedMatObj(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/_Art/_EasyBuilding/Materials/Instances/Dummy/MI_CanNot_Build.MI_CanNot_Build'"));
	if (GreenMatObj.Succeeded())
	{
		GreenMaterial = GreenMatObj.Object;
	}
	if (RedMatObj.Succeeded())
	{
		RedMaterial = RedMatObj.Object;
	}*/
	
	// 3. ��Ƽ���� �ε��� ��ȸ�ϸ� �� ����
	for (int32 i = 0; i < MaterialCount; ++i)
	{
		if (bIsGreen)
		{
			BuildGhost->SetMaterial(i, GreenMaterial); // GreenMaterial�� UMaterialInterface*
		}
		else
		{
			BuildGhost->SetMaterial(i, RedMaterial);   // RedMaterial�� UMaterialInterface*
		}
	}

	// 4. ��ġ ����
	BuildGhost->SetWorldTransform(BuildTransform, false, nullptr, ETeleportType::None);
}
void UAC_BuildComponent::SpwanBuild()
{
	if (!bCanBuild)
	{
		return;
	}
	
	// ��ȿ�� �˻�
	if (!Buildables.IsValidIndex(BuildID))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID: %d"), BuildID);
		return;
	}

	// �ش� �ε����� ������ ��������
	const FBuildableData& Data = Buildables[BuildID];

	if (!Data.Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Buildables[%d] has no Actor class"), BuildID);
		return;
	}

	// Actor ����
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		Data.Actor,
		BuildTransform,
		SpawnParams
	);

	if (!SpawnedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn actor from Buildables[%d]"), BuildID);
		return;
	}

	// �������̽��� ĳ���� �� SetMesh ȣ��
	if (SpawnedActor->GetClass()->ImplementsInterface(UBuildInterface::StaticClass()))
	{
		IBuildInterface::Execute_SetMesh(SpawnedActor, Data.Mesh);
		IBuildInterface::Execute_SetHealth(SpawnedActor, Data.Health);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned actor does not implement BuildInterface"));
	}
	if (SpawnedActor)
	{

		// 저장용 구조체 구성
		FBuildSaveData SaveData;
		SaveData.ActorClassPath = Data.Actor->GetClass()->GetPathName();  // 경로 저장
		SaveData.Location = SpawnedActor->GetActorLocation();
		SaveData.Rotation = SpawnedActor->GetActorRotation();
		SaveData.Scale = SpawnedActor->GetActorScale3D();

		SavedBuildDataArray.Add(SaveData);
	}
}
void UAC_BuildComponent::ChangeMesh()
{
	if (!BuildGhost || !Buildables.IsValidIndex(BuildID))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid BuildGhost or PlayerReference or BuildID"));
		return;
	}
	const FBuildableData& Data = Buildables[BuildID];
	if (Data.Mesh)
	{
		BuildGhost->SetStaticMesh(Data.Mesh);
		UpdateBuildPreviewUI();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No mesh found for BuildID: %d"), BuildID);
	}
}

bool UAC_BuildComponent::DetectBuildBoxes(FTransform& OutTransform)
{
	// 1. ��ȿ�� �˻�
	if (!HitActor || !HitComponent)
	{
		return false;
	}

	// 2. �������̽� ���� ���� Ȯ��
	if (!HitActor->GetClass()->ImplementsInterface(UBuildInterface::StaticClass()))
	{
		return false;
	}

	// 3. BuildInterface�� ReturnBoxes ȣ��
	TArray<UBoxComponent*> BuildBoxes = IBuildInterface::Execute_ReturnBoxes(HitActor);

	// 4. �ݺ����� ���� HitComponent�� ���ԵǾ� �ִ��� Ȯ��
	for (UBoxComponent* Box : BuildBoxes)
	{
		if (Box == Cast<UBoxComponent>(HitComponent))
		{
			OutTransform = HitComponent->GetComponentTransform();
			return true; // LocalFound = true
		}
	}

	// 5. �� ã�� ���
	return false; // LocalFound = false
}

void UAC_BuildComponent::SaveBuilds()
{
	FString SaveSlot = TEXT("BuildSlot");

	// SaveGame 오브젝트 생성
	UBuildSaveGame* SaveObject = Cast<UBuildSaveGame>(
		UEMSFunctionLibrary::GetCustomSave(this, UBuildSaveGame::StaticClass(), SaveSlot, TEXT("BuildData"))
	);

	if (SaveObject)
	{
		SaveObject->SavedBuilds = SavedBuildDataArray; // 현재 저장된 건물 배열 복사
		UEMSFunctionLibrary::SaveCustom(this, SaveObject); // 실제 저장 수행
	}
}

void UAC_BuildComponent::LoadBuilds()
{
	FString SaveSlot = TEXT("BuildSlot");

	UBuildSaveGame* SaveObject = Cast<UBuildSaveGame>(
		UEMSFunctionLibrary::GetCustomSave(this, UBuildSaveGame::StaticClass(), SaveSlot, TEXT("BuildData"))
	);

	if (SaveObject)
	{
		SavedBuildDataArray = SaveObject->SavedBuilds;

		// 배열을 바탕으로 액터 재배치
		for (const FBuildSaveData& Data : SavedBuildDataArray)
		{
			FActorSpawnParameters Params;
			UClass* BuildClass = LoadClass<AActor>(nullptr, *Data.ActorClassPath);
			if (BuildClass)
			{
				AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
					BuildClass,
					Data.Location,
					Data.Rotation,
					Params
				);

				if (SpawnedActor)
				{
					SpawnedActor->SetActorScale3D(Data.Scale);
				}
			}
		}
	}
}

void UAC_BuildComponent::RepairBuilding()
{
	FoundBuild();
	if (!bIsBuildModeOn || !HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Build Mode OFF or No HitActor"));
		return;
	}

	// BuildInterface 확인
	if (!HitActor->GetClass()->ImplementsInterface(UBuildInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("HitActor does not implement BuildInterface"));
		return;
	}
	// 현재 Actor의 Class Path로 DataTable에서 BuildData 찾기
	FString ClassPath = HitActor->GetClass()->GetPathName();
	FBuildableData* FoundData = nullptr;

	for (FBuildableData& Data : Buildables)
	{
		if (Data.Actor && Data.Actor->GetPathName() == ClassPath)
		{
			FoundData = &Data;
			break;
		}
	}

	if (!FoundData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Matching BuildableData not found"));
		return;
	}

	// 현재 체력 가져오기 (인터페이스로 체력 가져오기 필요)
	float CurrentHealth = IBuildInterface::Execute_GetHealth(HitActor);
	float MaxHealth = FoundData->Health;

	if (CurrentHealth >= MaxHealth)
	{
		UE_LOG(LogTemp, Log, TEXT("Already at full health"));
		UpdateHealth(CurrentHealth, MaxHealth);
		return;
	}

	// 자원 확인 (플레이어 인벤토리 시스템이 필요. 임시로 bool로 가정)
	bool bHasResources = true; // 여기에 인벤토리 체크 로직 삽입
	if (!bHasResources)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough resources to repair"));
		return;
	}

	// 자원 소비 (리소스 시스템과 연동 필요)
	// 예: ConsumeResource(FoundData->RequiredResources, FoundData->RequiredResourceAmounts);

	// 수리량 결정 (간단히 고정값이나 %로 가능)
	float RepairAmount = 20.0f; // 예: 고정 수리량

	float NewHealth = FMath::Clamp(CurrentHealth + RepairAmount, 0.0f, MaxHealth);
	IBuildInterface::Execute_SetHealth(HitActor, NewHealth);

	UE_LOG(LogTemp, Log, TEXT("Repaired %s to %.1f / %.1f"), *HitActor->GetName(), NewHealth, MaxHealth);

	UpdateHealth(NewHealth, MaxHealth);
}

void UAC_BuildComponent::UpdateBuildPreviewUI()
{
	if (!BuildPreviewWidget || !Buildables.IsValidIndex(BuildID))
	{
		return;
	}

	BuildPreviewWidget->SetVisibility(ESlateVisibility::Visible);

	const FBuildableData& Data = Buildables[BuildID];

	UImage* ThumbnailImage = Cast<UImage>(BuildPreviewWidget->GetWidgetFromName(TEXT("BuildThumbnail")));
	UTextBlock* DescText = Cast<UTextBlock>(BuildPreviewWidget->GetWidgetFromName(TEXT("BuildDescriptionText")));

	if (ThumbnailImage && Data.Thumbnail)
	{
		ThumbnailImage->SetBrushFromTexture(Data.Thumbnail);
	}
	if (DescText)
	{
		DescText->SetText(Data.BuildableDescription);
	}
}

void UAC_BuildComponent::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (!BuildHealthWidget)
	{
		return;
	}

	BuildHealthWidget->SetVisibility(ESlateVisibility::Visible);

	const FBuildableData& Data = Buildables[BuildID];

	UProgressBar* HealthBar = Cast<UProgressBar>(BuildHealthWidget->GetWidgetFromName(TEXT("HealthBar")));
	UTextBlock* HealthText = Cast<UTextBlock>(BuildHealthWidget->GetWidgetFromName(TEXT("HealthText")));

	if (HealthBar)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}

	if (HealthText)
	{
		FString Text = FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth);
		HealthText->SetText(FText::FromString(Text));
	}
	// 🔽 2초 후 숨기기
	if (BuildHealthWidget)
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthWidgetHideTimerHandle); // 이전 타이머 제거
		GetWorld()->GetTimerManager().SetTimer(HealthWidgetHideTimerHandle, [this]()
			{
				if (BuildHealthWidget)
				{
					BuildHealthWidget->SetVisibility(ESlateVisibility::Hidden);
				}
			}, 1.0f, false);
	}
	UE_LOG(LogTemp, Warning, TEXT("Complete UI"));
}

void UAC_BuildComponent::FoundBuild()
{

	FVector WorldLocation;
	FVector WorldDirection;
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		int32 ViewportSizeX, ViewportSizeY;
		PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

		FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

		PC->DeprojectScreenPositionToWorld(
			ScreenCenter.X,
			ScreenCenter.Y,
			WorldLocation,
			WorldDirection
		);
	}
	FVector Start = WorldLocation + WorldDirection * 350.f;
	FVector End = WorldLocation + WorldDirection * 1000.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerReference);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility, // 건물 전용 트레이스 채널 사용 추천
		Params
	);

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitResult.GetActor()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LineTrace failed"));
	}

	if (bHit && HitResult.GetActor())
	{
		AActor* HitTarget = HitResult.GetActor();
		if (HitTarget->GetClass()->ImplementsInterface(UBuildInterface::StaticClass()))
		{
			HitActor = HitTarget; // ✅ 수리 대상 설정
		}
	}
	else
	{
		HitActor = nullptr;
	}
}
