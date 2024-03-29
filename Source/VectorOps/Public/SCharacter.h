// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SCharacter.generated.h"


UCLASS()
class VECTOROPS_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FRotator LookupTarget(const FVector& Target);
protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PrimaryAttack(const FInputActionValue& Value);

protected:
	void DebugLocationFace();
	FVector CursorHitLocation();
	
protected:
	//Components
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components",meta = (AllowPrivateAccess = true))
	TObjectPtr<class USpringArmComponent> SpringArmComp;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components",meta = (AllowPrivateAccess = true))
	TObjectPtr<class UCameraComponent> CameraComp;

	
private:
	//Input
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Input",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Input",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Input",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Input",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;
	
private:
	UPROPERTY(EditDefaultsOnly,Category="Animation")
	TObjectPtr<class UAnimMontage> FireAnim;
	
	UPROPERTY(EditDefaultsOnly,Category="Bullet")
	TSubclassOf<class ABulletBase> BulletClass;
	FName MuzzleName;
	
};

