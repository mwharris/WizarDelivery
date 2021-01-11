#include "DeliveryItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
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
	// Get a reference to the GameMode
	GameModeRef = Cast<ADeliveryGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameModeRef == nullptr) 
	{
		UE_LOG(LogTemp, Error, TEXT("ADeliveryItem::BeginPlay(): Cannot find Game Mode!"));
	}
	// Set up our expire timer
	CurrExpireTime = MaxExpireTime;
	GetWorldTimerManager().SetTimer(ExpireTimerHandle, this, &ADeliveryItem::ExpireTick, ExpireTickFrequency, true);
}

void ADeliveryItem::ExpireTick() 
{
	CurrExpireTime -= ExpireTickFrequency;
	if (CurrExpireTime <= 0) 
	{
		GetWorldTimerManager().ClearTimer(ExpireTimerHandle);
	}
	if (GameModeRef != nullptr) 
	{
		bool KillYourself = GameModeRef->DeliveryExpireTick(this);
		if (KillYourself) 
		{
			Destroy();
		}
	}
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
	if (Success) 
	{
		// Teleport it away
		Destroy();
	}
	else
	{
		// Blow it up
		Destroy();
	}
}

float ADeliveryItem::GetMaxExpireTime() const
{
	return MaxExpireTime;
}

float ADeliveryItem::GetCurrExpireTime() const
{
	return CurrExpireTime;
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

TArray<FGestureStruct*> ADeliveryItem::GetCombination() const
{
	return Combination;
}