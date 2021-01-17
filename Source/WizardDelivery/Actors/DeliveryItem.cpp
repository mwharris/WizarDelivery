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
	// Set up our rotation/bob timer
	GetWorldTimerManager().SetTimer(BobTimerHandle, this, &ADeliveryItem::BobTick, BobTickFrequency, true);
	// Spawn a particle
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionParticles, GetActorLocation());
}

void ADeliveryItem::ExpireTick() 
{
	// Don't expire if the game is over
	if (GameModeRef == nullptr || GameModeRef->IsGameOver()) 
	{
		GetWorldTimerManager().ClearTimer(ExpireTimerHandle);
		return;
	}
	// Tick down our expire time
	CurrExpireTime -= ExpireTickFrequency;
	if (CurrExpireTime <= 0) 
	{
		GetWorldTimerManager().ClearTimer(ExpireTimerHandle);
	}
	// Kill ourselves if the GameMode says so
	if (GameModeRef->DeliveryExpireTick(this)) 
	{
		Destroy();
	}
}

void ADeliveryItem::BobTick() 
{
	float DeltaTime = BobTickFrequency;
	FVector NewLocation = GetActorLocation();
	FRotator NewRotation = GetActorRotation();
	float RunningTime = GetGameTimeSinceCreation();
	float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
	NewLocation.Z += DeltaHeight * 20.0f;       //Scale our height by a factor of 20
	float DeltaRotation = DeltaTime * 20.0f;    //Rotate by 20 degrees per second
	NewRotation.Yaw += DeltaRotation;
	SetActorLocationAndRotation(NewLocation, NewRotation);
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
		GetWorldTimerManager().ClearTimer(ExpireTimerHandle);
		GetWorldTimerManager().ClearTimer(BobTimerHandle);
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ADeliveryItem::KillSelf, 2.f, true);
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), TeleportSound, GetActorLocation());
		SpawnTeleportEffect();
	}
	else
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplodeSound, GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionParticles, GetActorLocation());
		Destroy();
	}
}

void ADeliveryItem::KillSelf() 
{
	Destroy();	
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
		if (Gesture != nullptr) 
		{
			ReturnArray.Add(Gesture->UISymbol);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ADeliveryItem::GetCombinationUI(): Gesture was a nullptr!"));
		}
	}
	return ReturnArray;
}

TArray<FGestureStruct*> ADeliveryItem::GetCombination() const
{
	return Combination;
}