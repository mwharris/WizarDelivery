#include "DeliveryItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "WizardDelivery/GameModes/DeliveryGameMode.h"

ADeliveryItem::ADeliveryItem()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	SetRootComponent(BoxCollider);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
}

void ADeliveryItem::BeginPlay()
{
	Super::BeginPlay();
}

void ADeliveryItem::CreateCombination(int32 MinGestureNum, int32 MaxGestureNum, UDataTable* GestureDataTable) 
{
	TArray<FName> RowNames = GestureDataTable->GetRowNames();
    int32 NumGestures = FMath::RandRange(MinGestureNum, MaxGestureNum);
    for (int32 i = 0; i < NumGestures; i++) 
    {
        int32 GestureId = FMath::RandRange(1, RowNames.Num());
		FName RowName = RowNames[GestureId - 1];
		static const FString ContextString(TEXT("GestureStruct"));
		FGestureStruct* Gesture = GestureDataTable->FindRow<FGestureStruct>(RowName, ContextString, true);
		if (Gesture != nullptr) 
		{
        	Combination.Add(Gesture);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ADeliveryItem::CreateCombination(): Could not find Gesture!"));
		}
    }
}

void ADeliveryItem::ResolveDelivery(bool Success) 
{
	// TODO: blow up or teleport
}

TArray<FGestureStruct*> ADeliveryItem::GetCombination() const
{
	return Combination;
}

TArray<FString> ADeliveryItem::GetCombinationUI() const
{
	TArray<FString> ReturnArray;
	for (FGestureStruct* Gesture : Combination) 
	{
		ReturnArray.Add(Gesture->UISymbol);
	}
	return ReturnArray;
}