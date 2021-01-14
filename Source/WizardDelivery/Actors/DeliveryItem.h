#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeliveryItem.generated.h"

class ADeliveryGameMode;
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
	virtual void Tick(float DeltaTime) override;
	void CreateCombination(int32 MinGestureNum, int32 MaxGestureNum, UDataTable* GestureDataTable);
	TArray<FGestureStruct*> GetCombination() const;
	void ResolveDelivery(bool Success);
	float GetMaxExpireTime() const;
	float GetCurrExpireTime() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Expire Timer", meta=(AllowPrivateAccess="true"))
	float MaxExpireTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Expire Timer", meta=(AllowPrivateAccess="true"))
	float ExpireTickFrequency = 0.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Expire Timer", meta=(AllowPrivateAccess="true"))
	float CurrExpireTime = 0.f;
	
	UPROPERTY(EditAnywhere, Category="Effects")
	UParticleSystem* ExplosionParticles;
	UPROPERTY(EditAnywhere, Category="Effects")
	USoundBase* ExplodeSound;

	int32 Score = 0;
	TArray<FGestureStruct*> Combination;
	FTimerHandle ExpireTimerHandle;
	ADeliveryGameMode* GameModeRef;

	void ExpireTick();

};
