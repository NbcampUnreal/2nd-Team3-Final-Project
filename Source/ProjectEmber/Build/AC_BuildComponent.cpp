// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/AC_BuildComponent.h"
#include "BuildInterface.h"
#include "Character/EmberCharacter.h"

// Sets default values for this component's properties
UAC_BuildComponent::UAC_BuildComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
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
		static const FString ContextString(TEXT("BuildDataContext")); //혹시 모를 오류 검출
		FBuildableData* Row = BuildData->FindRow<FBuildableData>(RowName, ContextString);

		if (Row)
		{
			Buildables.Add(*Row);
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

	// BuildGhost 컴포넌트 생성
	BuildGhost = NewObject<UStaticMeshComponent>(PlayerReference);
	if (!BuildGhost)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create BuildGhost component"));
		return;
	}

	BuildGhost->RegisterComponent(); // 월드에 등록
	BuildGhost->AttachToComponent(PlayerReference->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	BuildGhost->SetRelativeTransform(BuildTransform);

	// Static Mesh 설정
	const FBuildableData& Data = Buildables[BuildID];
	BuildGhost->SetStaticMesh(Data.Mesh);

	// 충돌 비활성화
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

	// 0.01초 후 BuildCycle 호출
	GetWorld()->GetTimerManager().SetTimer(BuildDelayHandle, this, &UAC_BuildComponent::BuildCycle, 0.01f, false);
}

void UAC_BuildComponent::BuildCycle()
{
	if (!Camera || Buildables.Num() <= BuildID) return;

	// 1. 카메라 위치 및 방향
	FVector Location = Camera->GetComponentLocation();
	FVector Forward = Camera->GetForwardVector();

	// 2. 라인트레이스 시작/끝 지점
	FVector Start = Location + Forward * 350.f;
	FVector End = Location + Forward * 1000.f;

	// 3. 라인트레이스
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner()); // 자신 무시
	Params.bTraceComplex = false;

	// Buildables에서 TraceChannel 추출
	ECollisionChannel TraceChannel = ECC_Visibility; // 기본 채널
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
		BuildTransform.SetRotation(HitResult.ImpactNormal.ToOrientationQuat());
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
		BuildTransform.SetRotation(Forward.ToOrientationQuat());
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
}

void UAC_BuildComponent::GiveBuildColor(bool bIsGreen)
{
	bCanBuild = bIsGreen;

	if (!BuildGhost) return;

	// 2. BuildGhost의 머티리얼 수 확인
	int32 MaterialCount = BuildGhost->GetNumMaterials();

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GreenMatObj(TEXT("Game/BuildingSystem/Build/Material/M_Green"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RedMatObj(TEXT("Game/BuildingSystem/Build/Material/M_Red"));
	if (GreenMatObj.Succeeded())
	{
		GreenMaterial = GreenMatObj.Object;
	}
	if (RedMatObj.Succeeded())
	{
		RedMaterial = RedMatObj.Object;
	}
	// 3. 머티리얼 인덱스 순회하며 색 변경
	for (int32 i = 0; i < MaterialCount; ++i)
	{
		if (bIsGreen)
		{
			BuildGhost->SetMaterial(i, GreenMaterial); // GreenMaterial은 UMaterialInterface*
		}
		else
		{
			BuildGhost->SetMaterial(i, RedMaterial);   // RedMaterial은 UMaterialInterface*
		}
	}

	// 4. 위치 적용
	BuildGhost->SetWorldTransform(BuildTransform, false, nullptr, ETeleportType::None);
}
void UAC_BuildComponent::SpwanBuild()
{
	// 유효성 검사
	if (!Buildables.IsValidIndex(BuildID))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID: %d"), BuildID);
		return;
	}

	// 해당 인덱스의 데이터 가져오기
	const FBuildableData& Data = Buildables[BuildID];

	if (!Data.Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Buildables[%d] has no Actor class"), BuildID);
		return;
	}

	// Actor 스폰
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

	// 인터페이스로 캐스팅 후 SetMesh 호출
	if (SpawnedActor->GetClass()->ImplementsInterface(UBuildInterface::StaticClass()))
	{
		IBuildInterface::Execute_SetMesh(SpawnedActor, Data.Mesh);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned actor does not implement BuildInterface"));
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
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No mesh found for BuildID: %d"), BuildID);
	}
}

bool UAC_BuildComponent::DetectBuildBoxes(FTransform& OutTransform)
{
	// 1. 유효성 검사
	if (!HitActor || !HitComponent)
	{
		return false;
	}

	// 2. 인터페이스 구현 여부 확인
	if (!HitActor->GetClass()->ImplementsInterface(UBuildInterface::StaticClass()))
	{
		return false;
	}

	// 3. BuildInterface의 ReturnBoxes 호출
	TArray<UBoxComponent*> BuildBoxes = IBuildInterface::Execute_ReturnBoxes(HitActor);

	// 4. 반복문을 통해 HitComponent가 포함되어 있는지 확인
	for (UBoxComponent* Box : BuildBoxes)
	{
		if (Box == Cast<UBoxComponent>(HitComponent))
		{
			OutTransform = HitComponent->GetComponentTransform();
			return true; // LocalFound = true
		}
	}

	// 5. 못 찾은 경우
	return false; // LocalFound = false
}