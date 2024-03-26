// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "BulletBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "GenericPlatform/GenericPlatformCrashContext.h"
// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp= CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	
	MuzzleName = "Muzzle_01";
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller))
	{
		if(TObjectPtr<UEnhancedInputLocalPlayerSubsystem> SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(DefaultMappingContext,0);
		}
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMouseVector();
	DebugLocationFace();
	// //ToDo: 캐릭터가 바라보는 방향과 컨트롤러의 방향 디버그
	// const float DrawScale = 1000.f;
	// const float Thickness = 10.f;
	// FVector LineStart  = GetActorLocation();
	// LineStart += GetActorRightVector()*100.f;
	//
	// FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector()*100.f);
	// DrawDebugDirectionalArrow(GetWorld(),LineStart,ActorDirection_LineEnd, DrawScale,FColor::Yellow,false,0.f,0,Thickness);
	//
	// FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector()*200.f);
	// DrawDebugDirectionalArrow(GetWorld(),LineStart,ControllerDirection_LineEnd,DrawScale,FColor::Green,false,0.f,0,Thickness);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if(TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ASCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction,ETriggerEvent::Triggered,this,&ASCharacter::Look);
		EnhancedInputComponent->BindAction(FireAction,ETriggerEvent::Triggered,this,&ASCharacter::PrimaryAttack);
	}
}

void ASCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVec = Value.Get<FVector2D>();
	if(IsValid(Controller))
	{
		const FRotator Rot = Controller->GetControlRotation();
		const FRotator YawRot = FRotator(0.f,Rot.Yaw,0.f);

		const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDir,MovementVec.Y);
		AddMovementInput(RightDir,MovementVec.X);
	}
}

void ASCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVec = Value.Get<FVector2D>();

	if(IsValid(Controller))
	{
		AddControllerYawInput(LookAxisVec.X);
	}
}

void ASCharacter::PrimaryAttack(const FInputActionValue& Value)
{
	if(FireAnim)
	{
		PlayAnimMontage(FireAnim);	
	}

	//Todo: 총알 제작
	FVector MuzzleLoc = GetMesh()->GetSocketLocation(MuzzleName);
	FRotator MuzzleRot = GetMesh()->GetSocketRotation(MuzzleName);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;
	if(ensure(BulletClass))
	{
		DrawDebugSphere(GetWorld(),MuzzleLoc,5.f,10.f,FColor::Green,true);
		GetWorld()->SpawnActor<AActor>(BulletClass,MuzzleLoc,MuzzleRot,SpawnParams);
	}
	
	
}
//캐릭터의 위와 정면 값으로 외적 구하기.
void ASCharacter::UpdateMouseVector()
{
	LastForward = GetActorForwardVector();
	

	StrafeVector = FVector::CrossProduct(GetActorUpVector(),LastForward);
	
	const float DrawScale = 1000.f;
	const float Thickness = 10.f;
	FVector LineStart  = GetActorLocation();
	LineStart += GetActorRightVector()*100.f;
	
	FVector ActorDirection_LineEnd = LineStart + (LastForward*100.f);
	DrawDebugDirectionalArrow(GetWorld(),LineStart,ActorDirection_LineEnd, DrawScale,FColor::Red,false,0.f,0,Thickness);
	FVector ActorUpDirection_LineEnd = LineStart+(GetActorUpVector()*100.f);
	DrawDebugDirectionalArrow(GetWorld(),LineStart,ActorUpDirection_LineEnd, DrawScale,FColor::Blue,false,0.f,0,Thickness);
	
	FVector CrossVec_LineEnd = LineStart + (StrafeVector*100.f);
	DrawDebugDirectionalArrow(GetWorld(),LineStart,CrossVec_LineEnd,DrawScale,FColor::Purple,false,0.f,0,Thickness);
}

void ASCharacter::SmoothStrafeVector()
{
	LerpStrafeVector *=-1.0f;
	float SquaredLength = (StrafeVector + LerpStrafeVector).Length();
	if(SquaredLength>1.0f)
	{
		LerpStrafeVector = FMath::Lerp(StrafeVector,LerpStrafeVector,GetWorld()->DeltaTimeSeconds*StrafeLerpSpeed);
	}
	else if(StrafeVector!=LerpStrafeVector)
	{
		StrafeVector = LerpStrafeVector;
	}
}

void ASCharacter::DebugLocationFace()
{
	FHitResult HitResult;
	bool bHit;
	FCollisionParameters TraceParams;

	FVector Loc = GetActorLocation();
	
	bHit = GetWorld()->LineTraceSingleByChannel(HitResult,Loc,Loc+ (StrafeVector*22222.0f), ECC_Visibility);
	if(bHit)
	{
		DrawDebugLine(GetWorld(),Loc,Loc+ (StrafeVector*22222.0f),FColor::Green,false,5.f,0,1.f);
	}
	bHit = GetWorld()->LineTraceSingleByChannel(HitResult,Loc,Loc+(StrafeVector*-22222.0f),ECC_Visibility);
	if(bHit)
	{
		DrawDebugLine(GetWorld(),Loc,Loc+ (StrafeVector*-22222.0f),FColor::Green,false,5.f,0,1.f);
	}
	bHit = GetWorld()->LineTraceSingleByChannel(HitResult,Loc,Loc+(GetActorForwardVector()*22222.0f),ECC_Visibility);
	if(bHit)
	{
		DrawDebugLine(GetWorld(),Loc,Loc+(GetActorForwardVector()*22222.0f),FColor::Blue,false,5.f,0,1.f);
	}
}

FVector ASCharacter::CursorHitLocation()
{
	FHitResult HitResult;
	if(TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(Controller))
	{
		PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::,true,HitResult);
	}
	
}

