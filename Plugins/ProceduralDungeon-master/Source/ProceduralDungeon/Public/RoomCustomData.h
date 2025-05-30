/*
 * MIT License
 *
 * Copyright (c) 2023-2025 Benoit Pelletier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "ReplicableObject.h"
#include "Interfaces/DungeonCustomSerialization.h"
#include "Interfaces/DungeonSaveInterface.h"
#include "Templates/SubclassOf.h"
#include "RoomCustomData.generated.h"

class UActorComponent;

// Base class for user custom data embedded in room instances
UCLASS(Abstract, BlueprintType, Blueprintable)
class PROCEDURALDUNGEON_API URoomCustomData : public UReplicableObject, public IDungeonCustomSerialization, public IDungeonSaveInterface
{
	GENERATED_BODY()

public:
	void CreateLevelComponent(class ARoomLevel* LevelActor);

	//~ Begin IDungeonCustomSerialization Interface
	virtual bool SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading) override;
	//~ End IDungeonCustomSerialization Interface

	//~ Begin IDungeonSaveInterface Interface
	virtual void PreSaveDungeon_Implementation() override;
	virtual void PostLoadDungeon_Implementation() override;
	//~ End IDungeonSaveInterface Interface

private:
	// Component to create and attach on the Level Script Actor of the instanced room.
	UPROPERTY(EditAnywhere, Category = "Dungeon Rules", meta = (AllowAbstract = false, AllowPrivateAccess = true))
	TSubclassOf<UActorComponent> LevelComponent {nullptr};

	// Keep a reference to the created component instance
	UPROPERTY(Transient)
	TWeakObjectPtr<UActorComponent> LevelComponentInstance {nullptr};

private:
	struct FSaveData
	{
		TArray<uint8> ComponentData;
	};

	TUniquePtr<FSaveData> SavedData;
};
