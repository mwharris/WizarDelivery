#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeliveryItem.generated.h"

class UBoxComponent;
class UDataTable;
class UStaticMeshComponent;
struct FGestureStruct;

UCLASS()
class WIZARDDELIVERY_API ADeliveryItem : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;

	UFUNCTION(BlueprintCallable, Category="Combination")
	TArray<FString> GetCombinationUI() const;

	ADeliveryItem();
	void CreateCombination(int32 MinGestureNum, int32 MaxGestureNum, UDataTable* GestureDataTable);
	TArray<FGestureStruct*> GetCombination() const;
	void ResolveDelivery(bool Success);

protected:
	virtual void BeginPlay() override;

private:
	int32 Score = 0;
	float ExpireTime = 0.f;
	TArray<FGestureStruct*> Combination;	

};
