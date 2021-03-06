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
	void CreateCombination(int32 MinGestureNum, int32 MaxGestureNum, UDataTable* GestureDataTable);
	TArray<FGestureStruct*> GetCombination() const;
	void ResolveDelivery(bool Success);
	float GetMaxExpireTime() const;
	float GetCurrExpireTime() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(AllowPrivateAccess = "true"))
	void SpawnTeleportEffect();

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Expire Timer", meta=(AllowPrivateAccess="true"))
	float MaxExpireTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Expire Timer", meta=(AllowPrivateAccess="true"))
	float ExpireTickFrequency = 0.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Expire Timer", meta=(AllowPrivateAccess="true"))
	float CurrExpireTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bob/Float Properties", meta=(AllowPrivateAccess="true"))
	float BobTickFrequency = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bob/Float Properties", meta=(AllowPrivateAccess="true"))
	float BobFloatSpeed = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bob/Float Properties", meta=(AllowPrivateAccess="true"))
	float BobRotationSpeed = 20.f;
	
	UPROPERTY(EditAnywhere, Category="Effects")
	USoundBase* TeleportSound;
	UPROPERTY(EditAnywhere, Category="Effects")
	UParticleSystem* SpawnParticles;
	UPROPERTY(EditAnywhere, Category="Effects")
	UParticleSystem* ExplosionParticles;
	UPROPERTY(EditAnywhere, Category="Effects")
	USoundBase* ExplodeSound;

	int32 Score = 0;
	TArray<FGestureStruct*> Combination;
	FTimerHandle ExpireTimerHandle;
	FTimerHandle BobTimerHandle;
	FTimerHandle DestroyTimerHandle;
	ADeliveryGameMode* GameModeRef;

	void ExpireTick();
	void BobTick();
	void KillSelf();

};
