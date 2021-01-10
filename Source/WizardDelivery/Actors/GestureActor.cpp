#include "GestureActor.h"

AGestureActor::AGestureActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

FString AGestureActor::GetGestureName() const
{
	return GestureInfo.GestureName;
}

void AGestureActor::SetGestureName(FString GName) 
{
	GestureInfo.GestureName = GName;
}

FString AGestureActor::GetUISymbol() const
{
	return GestureInfo.UISymbol;
}

void AGestureActor::SetUISymbol(FString Symbol) 
{
	GestureInfo.UISymbol = Symbol;
}

int32 AGestureActor::GetGestureId() const
{
	return GestureInfo.GestureId;
}

void AGestureActor::SetGestureId(int32 GId) 
{
	GestureInfo.GestureId = GId;
}