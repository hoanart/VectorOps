// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "BulletBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "SPlayerController.h"
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
		PlayerController->SetShowMouseCursor(true);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DebugLocationFace();
	
	
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if(TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ASCharacter::Move);
		EnhancedInputComponent->BindAction(FireAction,ETriggerEvent::Triggered,this,&ASCharacter::PrimaryAttack);
	}
}

FRotator ASCharacter::LookupTarget(const FVector& Target)
{
	FVector TargetVec2D = FVector(Target.X,Target.Y,0.f);
	FVector ActorVec2D = FVector(GetActorLocation().X,GetActorLocation().Y,0.f);
	//ILookupMouseInterface::Lookup_Implementation();
	FVector Dir = (TargetVec2D-ActorVec2D).GetSafeNormal();
	
	return FRotationMatrix::MakeFromX(Dir).Rotator();
}

void ASCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVec = Value.Get<FVector2D>();
	
	if(IsValid(Controller))
	{
		const FRotator Rot =GetActorRotation();
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
		AddControllerYawInput(LookAxisVec.Y);
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
		GetWorld()->SpawnActor<AActor>(BulletClass,GetActorLocation(),GetActorRotation(),SpawnParams);
	}
	
	
}


void ASCharacter::DebugLocationFace()
{
	FHitResult HitResult;
	bool bHit;
	FCollisionParameters TraceParams;

	FVector Loc = GetActorLocation();

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
		PlayerController->GetHitResultUnderCursor(ECC_Visibility,true,HitResult);
	}
	return HitResult.Location;
}
