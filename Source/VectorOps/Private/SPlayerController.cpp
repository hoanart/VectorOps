// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"


ASPlayerController::ASPlayerController()
{
	RotateSpeed = 180.0f;
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TObjectPtr<ASCharacter> PlayerCharacter = Cast<ASCharacter>(GetCharacter());

	if(IsValid(PlayerCharacter))
	{
		FRotator ToMouse = PlayerCharacter->LookupTarget(CursorHitLocation());
		PlayerCharacter->SetActorRotation(FMath::RInterpConstantTo(PlayerCharacter->GetActorRotation(),ToMouse,DeltaSeconds,RotateSpeed));
	}
	
}
FVector ASPlayerController::CursorHitLocation()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility,true,HitResult);
	return HitResult.Location;
}