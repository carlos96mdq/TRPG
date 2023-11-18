// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "BaseUnit.generated.h"


// Enums
UENUM()
enum class UnitState : uint8 {
	Idle,
	ReadyToMove,
	Moving,
	ReadyToCombat,
	Combating
};

UENUM()
enum class UnitStats : uint8 {
	None,
	Health,
	Energy,
	Armor,
	StrenghtPower,
	DexterityPower,
	DivinePower,
	Velocity,
	Accuracy,
	CriticChance,
	DoggingChance
};

UENUM()
enum class UnitType : uint8 {
	None,
};

UENUM()
enum class UnitStatus : uint8 {
	None,
	Bleeding,
	Poisoned,
	Stunned,
};

UENUM()
enum class UnitResistances : uint8 {
	None,
	Slashing,
	Bludgeoning,
	Piercing,
	Fire,
	Poison,
	Lightning
};

UENUM()
enum class CombatActionType : uint8 {
	None,
	Attack,
	Skill
};

UENUM()
enum class PassiveType : uint8 {
	None,
	Start,
	OnAttack,
	OnDefend,
	OnHitEnemy,
	OnHitByEnemy
};

UENUM()
enum class EffectType : uint8 {
	None,
	Status,
	Stats,
	Resistances
};

UENUM()
enum class ObjectiveType : uint8 {
	None,
	User,
	Ally,
	Allies,
	Enemy,
	Enemies,
	Zone
};

// Structs
// Struct that contains all resistance types
USTRUCT()
struct FResistances : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	int32 Slashing = 0;
	int32 Bludgeoning = 0;
	int32 Piercing = 0;
	int32 Fire = 0;
	int32 Poison = 0;
	int32 Lightning = 0;
};

// Struct that defines the effects used inside the CombatActions and Pasives data tables
USTRUCT()
struct FEffects : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EffectType Type = EffectType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 SubType = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsValuePercent = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Chance = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ObjectiveType Objective = ObjectiveType::None;
};

// Struct that saves all the combat action of all units
USTRUCT(BlueprintType)
struct FCombatActionsStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	CombatActionType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UnitResistances DamageType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Power = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StrenghtMod = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DexterityMod = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpecialMod = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Accuracy = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ObjectiveType ObjectiveType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinRange = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxRange = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEffects> Effects;
};

// Struct that saves all the passives of all units
USTRUCT(BlueprintType)
struct FPassives : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PassiveType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEffects> Effects;
};

// Struct to initialize a BaseUnit in a data driven way
USTRUCT(BlueprintType)
struct FUnits : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UnitType Type = UnitType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UnitType SubType = UnitType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxEnergy = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Armor = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StrenghtPower = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DexterityPower = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DivinePower = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Velocity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Accuracy = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CriticChance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DoggingChance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FResistances Resistances;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> Model3D;
};

UCLASS()
class TRPG_API ABaseUnit : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when unit is clicked by mouse
	UFUNCTION()
	void OnUnitClicked(AActor* ClickedActor, FKey ButtonPressed);
	
	// Called in tick to move unit
	void Move(float DeltaTime);

	// Called to add the resistance values added by the type or subtyp
	void AddTypeModifiers(UnitType NewType);

	// Unit information
	FName Name = TEXT("None");
	FName Archetype = TEXT("None");
	UnitType Type = UnitType::None;
	UnitType SubType = UnitType::None;
	UnitState CurrentState = UnitState::Idle;
	
	// Unit stats
	FUnits* BaseStats;
	int32 Health = 0;
	int32 Energy = 0;
	
	// Unit actions
	TArray<FName> KnownCombatActions;			// Name of combat actions known by the units. A FName is saved so it can be search in a Data Table
	TArray<int32> EquippedCombatActions;		// Saves an index to the KnownCombatActions array
	FCombatActionsStruct* CurrentCombatAction;	// Saves the data of the current combat action in use, to make all calculations needed to apply damage and effects

	// Unit passives
	TArray<FName> KnownPassives;	// Name of combat actions known by the units. A FName is saved so it can be search in a Data Table

	// Unit buffs


	// Units Textures
	UPROPERTY()
	UTexture2D* Icon;
	UPROPERTY()
	USkeletalMesh* Model3D;
	UPROPERTY()
	USkeletalMeshComponent* Model3DComponent;

	// Data Table references
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* UnitsTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* CombatActionsTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* PassivesTable;

	// Movement
	int32 MaxMovement = 0;
	int32 Movement = 5;
	TQueue<FVector> QueueDestinations;
	FVector Destination = FVector::ZeroVector;

public:	
	// Sets default values for this actor's properties
	ABaseUnit();

	// Initialize the unit with some parameters needed to get the unit data from the data tables
	void Init(FName NewArchetype);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called by PlayerController to make this unit move to a destination
	void MoveUnit(FVector FinalDestination);
	
	// Called by PlayerState to make this unit move to a destination following a preseted path
	void MoveUnit(TArray<FVector>& Path);

	// Set the CurrentCombatAction and return false if the action selected doesn't exist
	void SetCombatAction(int32 ActionId);

	// The unit uses one of its equipped actions
	void UseCurrentAction(ABaseUnit* Objective);

	// Apply a direct damage to the unit and verify if it dies because of it
	void ApplyDamage(int32 Damage);

	// Set the Unit State and send a message to all object that need to know about it
	void SetUnitState(UnitState NewState);

	bool HasActionEquipped(int32 CombatActionSlot) const;

	const UnitState GetUnitState() const { return CurrentState; }
	const int32 GetUnitStat(UnitStats Stat);
	const int32 GetUnitResistance(UnitResistances DamageType);
	const int32 GetMovement() const { return Movement; }
	const int32 GetCombatActionRangeMin() const { return CurrentCombatAction->MinRange; }
	const int32 GetCombatActionRangeMax() const { return CurrentCombatAction->MaxRange; }
	const FName GetName() const { return Name; }
	const FName GetArchetype() const { return Archetype; }
	const int32 GetStatArmor() const { return BaseStats.Armor + ModifierStats.Armor; }
	const int32 GetStatResistance(UnitType UnitResistances);
	const int32 GetStatStrenghtPower() const { return BaseStats.StrenghtPower + ModifierStats.StrenghtPower; }
	const int32 GetStatDexterityPower() const { return BaseStats.DexterityPower + ModifierStats.DexterityPower; }
	const int32 GetStatDivinePower() const { return BaseStats.DivinePower + ModifierStats.DivinePower; }
	const int32 GetStatVelocity() const { return BaseStats.Velocity + ModifierStats.Velocity; }
	const int32 GetStatAccuracy() const { return BaseStats.Accuracy + ModifierStats.Accuracy; }
	const int32 GetStatCriticChance() const { return BaseStats.CriticChance + ModifierStats.CriticChance; }
	const int32 GetStatDoggingChance() const { return BaseStats.DoggingChance + ModifierStats.DoggingChance; }
	UTexture2D* GetIcon() const { return Icon; };
};
