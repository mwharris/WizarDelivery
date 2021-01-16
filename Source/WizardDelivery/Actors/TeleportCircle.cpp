#include "TeleportCircle.h"
#include "Components/BoxComponent.h"
#include "Components/SpotLightComponent.h" 
#include "Components/TextRenderComponent.h"
#include "WizardDelivery/Actors/DeliveryItem.h"

ATeleportCircle::ATeleportCircle()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	SetRootComponent(BoxCollider);

	DeliveryPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Delivery Point"));
	DeliveryPoint->SetupAttachment(RootComponent);

	PlayerPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Player Point"));
	PlayerPoint->SetupAttachment(RootComponent);

	TextRenderer = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Circle Number Renderer"));
	TextRenderer->SetupAttachment(RootComponent);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spot Light"));
	SpotLight->SetupAttachment(RootComponent);
}

void ATeleportCircle::BeginPlay()
{
	Super::BeginPlay();
	TextRenderer->SetText(FText::AsNumber(CircleNum));
}

bool ATeleportCircle::IsEmpty() const
{
	return GetDelivery() == nullptr;
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

void ATeleportCircle::SetLightActive(bool IsActive) 
{
	SpotLight->SetVisibility(IsActive);
}