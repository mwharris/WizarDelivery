#include "DeliveryGameMode.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "WizardDelivery/Actors/DeliveryItem.h"
#include "WizardDelivery/Actors/TeleportCircle.h"
#include "WizardDelivery/Characters/WizardCharacter.h"

void ADeliveryGameMode::BeginPlay() 
{
	Super::BeginPlay();
    // Get a reference to the player
    PlayerRef = Cast<AWizardCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerRef == nullptr) 
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode: Could not find Player!"));
    }
    else 
    {
        Init();
    }
}

void ADeliveryGameMode::Init() 
{
    // Store our number of gestures
    NumGestures = GestureDataTable->GetRowNames().Num();
    // Gather all of the TeleportCircles
	TArray<AActor*> TCActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeleportCircle::StaticClass(), TCActors);
    if (TCActors.Num() > 0) 
    {
        // Convert all Actors to TeleportCircles
        for (AActor* TCActor : TCActors) 
        {
            if (ATeleportCircle* TCircle = Cast<ATeleportCircle>(TCActor))
            {
                SetupTeleportCircle(TCircle);
                NotifyHUDDeliveryCreated(TCircle->GetCircleNum());
            }
        }
        // Sort the array by CircleNum
        TeleportCircles.Sort([](const ATeleportCircle& a, const ATeleportCircle& b) { return a.GetCircleNum() < b.GetCircleNum(); });
        // Pick a random circle to spawn our character
        int32 Index = FMath::RandRange(0, TeleportCircles.Num() - 1);
        WarpPlayerToCircle(Index);
    }
}

void ADeliveryGameMode::SetupTeleportCircle(ATeleportCircle* TCircle) 
{
    // Spawn a delivery on this circle
    FActorSpawnParameters params;
    FVector Location = TCircle->DeliveryPoint->GetComponentLocation();
    ADeliveryItem* NewDelivery = GetWorld()->SpawnActor<ADeliveryItem>(DeliveryClass, Location, FRotator::ZeroRotator, params);
    // Create a random combination for the delivery
    NewDelivery->CreateCombination(MinGestureNum, MaxGestureNum, GestureDataTable);
    // Setup circle, delivery, and circle array
    TCircle->SetDelivery(NewDelivery);
    TeleportCircles.Add(TCircle);
}

void ADeliveryGameMode::WarpPlayerToCircle(int32 CircleIndex) 
{
    if (PlayerIndex == CircleIndex) { return; }
    if (TeleportCircles.Num() <= 0 || CircleIndex < 0 || CircleIndex >= TeleportCircles.Num()) 
    { 
        UE_LOG(LogTemp, Error, TEXT("WarpPlayerToCircle: No circles OR Index is incorrect!"));
        return; 
    }
    // Load the circle at the given index
    ATeleportCircle* TCircle = TeleportCircles[CircleIndex];
    if (TCircle != nullptr) 
    {
        // Move the player to that circle's player location
        FVector Location = TCircle->PlayerPoint->GetComponentTransform().GetLocation();
        PlayerRef->SetActorLocation(Location);
        PlayerIndex = TCircle->GetCircleNum();
        // Get a reference to the delivery on the player's current circle
        ActiveDelivery = TCircle->GetDelivery();
        // Reset our Combination Index
        CombinationIndex = 0;
    }
}

void ADeliveryGameMode::ProcessGesture(FString GestureName) 
{
    // Get the Gesture associated with the input (if one exists)
    static const FString ContextString(TEXT("GestureStruct"));
    FGestureStruct* InputGesture = GestureDataTable->FindRow<FGestureStruct>(FName(GestureName), ContextString, true);
    if (InputGesture != nullptr) 
    {
        // Get the Gesture combination from the Delivery
        TArray<FGestureStruct*> DeliveryCombination = ActiveDelivery->GetCombination();
        FGestureStruct* CombinationGesture = DeliveryCombination[CombinationIndex];
        // Verify that both GestureIds are equal
        if (CombinationGesture != nullptr && CombinationGesture->GestureId == InputGesture->GestureId) 
        {
            UE_LOG(LogTemp, Warning, TEXT("Correct: %s = %s"), *InputGesture->GestureName, *CombinationGesture->GestureName);
            UE_LOG(LogTemp, Warning, TEXT("Circle: %d, Index: %d"), PlayerIndex, CombinationIndex);
            NotifyHUDInputProcessed(PlayerIndex, CombinationIndex, true);
            CombinationIndex++;
            if (CombinationIndex == DeliveryCombination.Num()) 
            {
                // TODO: Teleport this package away
                CombinationIndex = 0;
            }
        }
        else 
        {
            UE_LOG(LogTemp, Warning, TEXT("Incorrect: %s = %s"), *InputGesture->GestureName, *CombinationGesture->GestureName);
            NotifyHUDInputProcessed(PlayerIndex, CombinationIndex, false);
            // TODO: Blow up this package, reset Combinataion Index
        }
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode::ProcessGesture(): Could not find gesture!"));
    }
}

ADeliveryItem* ADeliveryGameMode::GetActiveDelivery() const
{
    return ActiveDelivery;
}