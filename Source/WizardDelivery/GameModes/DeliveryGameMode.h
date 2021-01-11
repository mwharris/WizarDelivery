#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "DeliveryGameMode.generated.h"

class ADeliveryItem;
class ATeleportCircle;
class AWizardCharacter;
class UDataTable;

USTRUCT(BlueprintType)
struct FGestureStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString GestureName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString UISymbol;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 GestureId;

};

UCLASS()
class WIZARDDELIVERY_API ADeliveryGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Scoring")
	int32 GetScore() const;

	void WarpPlayerToCircle(int32 CircleIndex);
	ADeliveryItem* GetActiveDelivery() const;
	void ProcessGesture(FString GestureName);
	bool DeliveryExpireTick(ADeliveryItem* DeliveryItem);

protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(AllowPrivateAccess = "true"))
	void NotifyHUDDeliveryCreated(int32 CircleNum);
	UFUNCTION(BlueprintImplementableEvent, meta=(AllowPrivateAccess = "true"))
	void NotifyHUDInputProcessed(int32 CircleNum, int32 ComboIndex, bool Correct);
	UFUNCTION(BlueprintImplementableEvent, meta=(AllowPrivateAccess = "true"))
	void NotifyHUDUpdateScore(int32 NewScore, int32 CircleNum);
	UFUNCTION(BlueprintImplementableEvent, meta=(AllowPrivateAccess = "true"))
	void NotifyHUDGameOver(int32 FinalScore);
	UFUNCTION(BlueprintImplementableEvent, meta=(AllowPrivateAccess = "true"))
	void NotifyHUDDeliveryTimer(int32 CircleNum, float Percent);

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ADeliveryItem> DeliveryClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	UDataTable* GestureDataTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combinations", meta = (AllowPrivateAccess = "true"))
	int32 MinGestureNum = 4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combinations", meta = (AllowPrivateAccess = "true"))
	int32 MaxGestureNum = 8;

	int32 NumGestures = 0;
	TArray<ATeleportCircle*> TeleportCircles; 
	AWizardCharacter* PlayerRef;
	int32 PlayerIndex = 99;
	ADeliveryItem* ActiveDelivery;
	int32 CombinationIndex = 0;
	int32 Score = 0;
	bool GameOver = false;

	void Init();
	void SetupTeleportCircle(ATeleportCircle* TCircle);
	void ResolveDelivery(bool Success, int32 ComboLength, bool DelayDestroy = false);

};
