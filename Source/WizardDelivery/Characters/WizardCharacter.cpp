#include "WizardCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "WizardDelivery/GameModes/DeliveryGameMode.h"

AWizardCharacter::AWizardCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWizardCharacter::BeginPlay()
{
	Super::BeginPlay();
	GameModeRef = Cast<ADeliveryGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameModeRef == nullptr) 
	{
		UE_LOG(LogTemp, Error, TEXT("Wizard: Could not find Game Mode!"));
	}
	Lives = 3;
}

void AWizardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Teleport input actions
	PlayerInputComponent->BindAction<FTeleportInputDelegate>("One",   EInputEvent::IE_Pressed, this, &AWizardCharacter::WarpToCircle, 1);
	PlayerInputComponent->BindAction<FTeleportInputDelegate>("Two",   EInputEvent::IE_Pressed, this, &AWizardCharacter::WarpToCircle, 2);
	PlayerInputComponent->BindAction<FTeleportInputDelegate>("Three", EInputEvent::IE_Pressed, this, &AWizardCharacter::WarpToCircle, 3);
	// Gesture input actions
	PlayerInputComponent->BindAction<FGestureInputDelegate>("Up",    EInputEvent::IE_Pressed, this, &AWizardCharacter::PerformGesture, FString(TEXT("Up")));
	PlayerInputComponent->BindAction<FGestureInputDelegate>("Down",  EInputEvent::IE_Pressed, this, &AWizardCharacter::PerformGesture, FString(TEXT("Down")));
	PlayerInputComponent->BindAction<FGestureInputDelegate>("Left",  EInputEvent::IE_Pressed, this, &AWizardCharacter::PerformGesture, FString(TEXT("Left")));
	PlayerInputComponent->BindAction<FGestureInputDelegate>("Right", EInputEvent::IE_Pressed, this, &AWizardCharacter::PerformGesture, FString(TEXT("Right")));
	PlayerInputComponent->BindAction<FGestureInputDelegate>("Cast",  EInputEvent::IE_Pressed, this, &AWizardCharacter::PerformGesture, FString(TEXT("Cast")));
}

void AWizardCharacter::WarpToCircle(int32 Index) 
{
	if (GameModeRef->IsGameOver()) { return; }
	GameModeRef->WarpPlayerToCircle(Index - 1);
}

void AWizardCharacter::PerformGesture(FString GestureName) 
{
	if (GameModeRef->IsGameOver()) { return; } 
	if (ChantSound != nullptr) 
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ChantSound);
	}
	GameModeRef->ProcessGesture(GestureName);
}

bool AWizardCharacter::HandleLoss()
{
	Lives--;
	return Lives <= 0;	
}

int32 AWizardCharacter::GetLives() const
{
	return Lives;	
}