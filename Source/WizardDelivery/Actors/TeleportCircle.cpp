#include "TeleportCircle.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "WizardDelivery/Actors/DeliveryItem.h"

ATeleportCircle::ATeleportCircle()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	SetRootComponent(BoxCollider);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	DeliveryPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Delivery Point"));
	DeliveryPoint->SetupAttachment(RootComponent);

	PlayerPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Player Point"));
	PlayerPoint->SetupAttachment(RootComponent);
}

void ATeleportCircle::BeginPlay()
{
	Super::BeginPlay();
}

bool ATeleportCircle::IsEmpty() const
{
	return Empty;
}

int32 ATeleportCircle::GetCircleNum() const
{
	return CircleNum;
}

ADeliveryItem* ATeleportCircle::GetDelivery() const
{
	return CurrDelivery;
}

void ATeleportCircle::SetDelivery(ADeliveryItem* DeliveryItem) 
{
	CurrDelivery = DeliveryItem;
}