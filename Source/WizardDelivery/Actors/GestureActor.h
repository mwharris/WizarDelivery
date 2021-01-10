#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "GestureActor.generated.h"

USTRUCT(BlueprintType)
struct FGesture : public FTableRowBase
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
class WIZARDDELIVERY_API AGestureActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGestureActor();
	FGesture GestureInfo;

	FString GetGestureName() const;
	void SetGestureName(FString GName);

	FString GetUISymbol() const;
	void SetUISymbol(FString Symbol);

	int32 GetGestureId() const;
	void SetGestureId(int32 GId);

};
