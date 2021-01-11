#include "DeliveryGameMode.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
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
    if (TCActors.Num() <= 0) { return; } 
    // Spawn 1 delivery at a random circle
    int32 RandCircleIndex = FMath::RandRange(1, TCActors.Num());
    // Convert all Actors to TeleportCircles
    for (AActor* TCActor : TCActors) 
    {
        if (ATeleportCircle* TCircle = Cast<ATeleportCircle>(TCActor))
        {
            // Spawn a delivery if this is our random start point
            if (TCircle->GetCircleNum() == RandCircleIndex) 
            {
                SpawnDeliveryAtCircle(TCircle);
            }
            TeleportCircles.Add(TCircle);
        }
    }
    // Sort the array by CircleNum
    TeleportCircles.Sort([](const ATeleportCircle& a, const ATeleportCircle& b) { return a.GetCircleNum() < b.GetCircleNum(); });
    // Pick a random circle to spawn our character
    int32 Index = FMath::RandRange(0, TeleportCircles.Num() - 1);
    WarpPlayerToCircle(Index);
    // Start our spawn timer
    RampUpCounter = RampUpFrequencySeconds; 
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ADeliveryGameMode::TickSpawnDelivery, SpawnTimerFrequency, false);
}

void ADeliveryGameMode::SpawnDeliveryAtCircle(ATeleportCircle* TCircle) 
{
    // Spawn a delivery on this circle
    FActorSpawnParameters params;
    FVector Location = TCircle->DeliveryPoint->GetComponentLocation();
    ADeliveryItem* NewDelivery = GetWorld()->SpawnActor<ADeliveryItem>(DeliveryClass, Location, FRotator::ZeroRotator, params);
    // Create a random combination for the delivery
    NewDelivery->CreateCombination(MinGestureNum, MaxGestureNum, GestureDataTable);
    // Setup circle, delivery, and circle array
    TCircle->SetDelivery(NewDelivery);
    // Set ActiveDelivery if the Player is currently on this circle while it's spawned
    if (TCircle->GetCircleNum() == PlayerIndex + 1) 
    {
        ActiveDelivery = TCircle->GetDelivery();
    }
    // Tell the HUD to update this circle's combination
    NotifyHUDDeliveryCreated(TCircle->GetCircleNum());
}

void ADeliveryGameMode::TickSpawnDelivery() 
{
    if (GameOver) { return; }
    RampUpCounter -= SpawnTimerFrequency;
    // Get a list of empty Teleport Circles
    TArray<ATeleportCircle*> EmptyCircles;
    for (ATeleportCircle* TCircle : TeleportCircles) 
    {
        if (TCircle->IsEmpty()) 
        {
            EmptyCircles.Add(TCircle);
        }
    }
    // Spawn a delivery at a random empty circle if we found any
    if (EmptyCircles.Num() > 0) 
    {
        int32 RandIndex = EmptyCircles.Num() == 1 ? 0 : FMath::RandRange(0, EmptyCircles.Num() - 1);
        ATeleportCircle* TCircle = EmptyCircles[RandIndex];
        SpawnDeliveryAtCircle(TCircle);
        // Ramp up if our counter expired
        if (RampUpCounter <= 0) 
        {
            SpawnTimerFrequency -= FMath::Clamp(RampUpSpeed, MinSpawnTime, 5.f);
            RampUpCounter = RampUpFrequencySeconds;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Refreshing spawner.  SpawnTimerFrequency: %f, RampUpCounter: %f, RampUpFrequencySeconds: %f"), SpawnTimerFrequency, RampUpCounter, RampUpFrequencySeconds);
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ADeliveryGameMode::TickSpawnDelivery, SpawnTimerFrequency, false);
}

void ADeliveryGameMode::WarpPlayerToCircle(int32 CircleIndex) 
{
    if (PlayerIndex == CircleIndex || GameOver) { return; }
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
        PlayerIndex = CircleIndex;
        // Mark TCircles as active/inactive
        for (ATeleportCircle* Circle : TeleportCircles) 
        {
            Circle->SetLightActive(Circle == TCircle);
        }
        // Get a reference to the delivery on the player's current circle
        ActiveDelivery = TCircle->GetDelivery();
        // Reset our Combination Index
        CombinationIndex = 0;
    }
}

void ADeliveryGameMode::ProcessGesture(FString GestureName) 
{
    if (ActiveDelivery == nullptr || GameOver) { return; }
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
            NotifyHUDInputProcessed(PlayerIndex + 1, CombinationIndex, true);
            CombinationIndex++;
            if (CombinationIndex == DeliveryCombination.Num()) 
            {
                ResolveDelivery(true, DeliveryCombination.Num());
            }
        }
        else 
        {
            NotifyHUDInputProcessed(PlayerIndex + 1, CombinationIndex, false);
            ResolveDelivery(false, DeliveryCombination.Num());
        }
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode::ProcessGesture(): Could not find gesture!"));
    }
}

void ADeliveryGameMode::ResolveDelivery(bool Success, int32 ComboLength, bool DelayDestroy) 
{
    TeleportCircles[PlayerIndex]->SetDelivery(nullptr);
    if (!DelayDestroy)
    {
        ActiveDelivery->ResolveDelivery(Success);
    }
    ActiveDelivery = nullptr;
    CombinationIndex = 0;
    if (Success) 
    {
        Score += ComboLength;
        NotifyHUDUpdateScore(Score, TeleportCircles[PlayerIndex]->GetCircleNum());
    }
    else 
    {
        GameOver = true;
	    NotifyHUDGameOver(Score);
    }
}

bool ADeliveryGameMode::DeliveryExpireTick(ADeliveryItem* DeliveryItem) 
{
    if (DeliveryItem == nullptr) { return false; }
    // Find which circle this delivery is in
    int32 CircleNum = 0;
    for (ATeleportCircle* Circle : TeleportCircles) 
    {
        if (Circle->GetDelivery() ==  DeliveryItem)
        {
            CircleNum = Circle->GetCircleNum();
        }
    }
    // Tick the progress bar on the UI
    float Percent = DeliveryItem->GetCurrExpireTime() / DeliveryItem->GetMaxExpireTime();
    NotifyHUDDeliveryTimer(CircleNum, Percent);
    // Destroy the delivery and punish player if it expired
    if (DeliveryItem->GetCurrExpireTime() <= 0) 
    {
        ResolveDelivery(false, DeliveryItem->GetCombination().Num(), true);
        return true;
    }
    return false;
}

ADeliveryItem* ADeliveryGameMode::GetActiveDelivery() const
{
    return ActiveDelivery;
}

int32 ADeliveryGameMode::GetScore() const
{
    return Score;
}

bool ADeliveryGameMode::IsGameOver() const
{
    return GameOver;
}