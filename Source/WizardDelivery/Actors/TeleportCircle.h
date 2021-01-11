#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportCircle.generated.h"

class ADeliveryItem;
class UBoxComponent;
class UStaticMeshComponent;
class USpotLightComponent;
class UTextRenderComponent;

UCLASS()
class WIZARDDELIVERY_API ATeleportCircle : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* DeliveryPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* PlayerPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* TextRenderer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpotLightComponent* SpotLight;

	UFUNCTION(BlueprintCallable, Category="Delivery")
	ADeliveryItem* GetDelivery() const;

	ATeleportCircle();
	bool IsEmpty() const;
	int32 GetCircleNum() const;
	void SetDelivery(ADeliveryItem* DeliveryItem);
	void SetLightActive(bool IsActive);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", meta=(AllowPrivateAccess="true"))
	int32 CircleNum = 0;

	ADeliveryItem* CurrDelivery;

};
