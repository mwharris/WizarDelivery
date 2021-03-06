#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WizardCharacter.generated.h"

class ADeliveryGameMode;
class UAnimMontage;

UCLASS()
class WIZARDDELIVERY_API AWizardCharacter : public ACharacter
{
	GENERATED_BODY()
    DECLARE_DELEGATE_OneParam(FTeleportInputDelegate, int32);
    DECLARE_DELEGATE_OneParam(FGestureInputDelegate, FString);

public:
	AWizardCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	int32 GetLives() const;
	bool HandleLoss();
	void CastTeleport();

protected:
	ADeliveryGameMode* GameModeRef;

	virtual void BeginPlay() override;
	void WarpToCircle(int32 Index);
	void PerformGesture(FString GestureName);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sounds", meta = (AllowPrivateAccess = "true"))
	USoundBase* ChantSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sounds", meta = (AllowPrivateAccess = "true"))
	USoundBase* MoveSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CastMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HurtMontage;

	USkeletalMeshComponent* SkeletalMesh;
	int32 Lives = 3;

};