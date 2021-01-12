#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WizardCharacter.generated.h"

class ADeliveryGameMode;

UCLASS()
class WIZARDDELIVERY_API AWizardCharacter : public ACharacter
{
	GENERATED_BODY()
    DECLARE_DELEGATE_OneParam(FTeleportInputDelegate, int32);
    DECLARE_DELEGATE_OneParam(FGestureInputDelegate, FString);

public:
	AWizardCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	ADeliveryGameMode* GameModeRef;

	virtual void BeginPlay() override;
	void WarpToCircle(int32 Index);
	void PerformGesture(FString GestureName);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sounds", meta = (AllowPrivateAccess = "true"))
	USoundBase* ChantSound;

};