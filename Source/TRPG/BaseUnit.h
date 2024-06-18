// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "BaseUnit.generated.h"


// Enums
UENUM()
enum class EUnitType : uint8 {
	None,
	Bug,
	Dark,
	Dragon,
	Electric,
	Fairy,
	Fighting,
	Fire,
	Flying,
	Ghost,
	Grass,
	Ground,
	Normal,
	Ice,
	Posion,
	Psychic,
	Rock,
	Steel,
	Water,

	MAX
};

UENUM()
enum class EUnitState : uint8 {
	Idle,
	ReadyToMove,
	Moving,
	ReadyToCombat,
	Combating
};

UENUM()
enum class EUnitStats : uint8 {
	None,
	MaxHealth,
	NaturalArmor,
	Strenght,
	Dexterity,
	Special,
	Velocity,
	Mobility,
	Accuracy,
	CriticChance,
	EvadeChance,

	MAX
};

UENUM()
enum class EDamageModifier : uint8 {
	None,
	Strenght,
	Special,
	Dexterity
};

UENUM()
enum class ECombatActionType : uint8 {
	None,
	Attack,
	Skill
};

UENUM()
enum class EPassiveType : uint8 {
	None,
	Start,
	OnAttack,
	OnDefend,
	OnHitEnemy,
	OnHitByEnemy
};

UENUM()
enum class EStatusEffects : uint8 {
	None,
	Burn,
	Poison,
	Freeze,
	Sleep,
	Paralysis,
	Confuse,

	MAX
};

UENUM()
enum class EEffectTypes : uint8 {
	None,
	Status,
	Stats
};

UENUM()
enum class EObjectiveType : uint8 {
	None,
	User,
	Ally,
	Allies,
	Enemy,
	Enemies,
	Zone
};

// Structs
// Struct to create a Data Table that contains all the unit damage types relations
USTRUCT(BlueprintType)
struct FDamageTypeModifiers : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Bug = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Dark = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Dragon = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Electric = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Fairy = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Fighting = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Fire = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Flying = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Ghost = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Grass = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Ground = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Normal = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Ice = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Posion = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Psychic = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Rock = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Steel = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Water = 1;
};

// Struct that defines the effects used inside the CombatActions and Pasives data tables
USTRUCT()
struct FEffects : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectTypes Type = EEffectTypes::None;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitStats NameStat = EUnitStats::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatusEffects NameStatus = EStatusEffects::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Chance = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EObjectiveType Objective = EObjectiveType::None;
};

// Struct that saves all the combat action of all units
USTRUCT(BlueprintType)
struct FCombatActions : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECombatActionType Type = ECombatActionType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitType DamageType = EUnitType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Power = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageModifier Modifier = EDamageModifier::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Accuracy = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EObjectiveType ObjectiveType = EObjectiveType::None;
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
	EPassiveType Type;
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
	EUnitType Type = EUnitType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitType SubType = EUnitType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NaturalArmor = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Strenght = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Dexterity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Special = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Velocity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Movility = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> Model3D;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UAnimBlueprint> AnimationBP;
};

UCLASS()
class TRPG_API ABaseUnit : public AActor
{
	GENERATED_BODY()

	static bool bDamageTypeModifiersTableLoaded;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when unit is clicked by mouse
	UFUNCTION()
	void OnUnitClicked(AActor* ClickedActor, FKey ButtonPressed);
	
	// Called in tick to move unit
	void Move(float DeltaTime);

	// Unit data
	FName Name = TEXT("None");
	FName Archetype = TEXT("None");
	EUnitType Type = EUnitType::None;
	EUnitType SubType = EUnitType::None;
	EUnitState CurrentState = EUnitState::Idle;
	
	TArray<int32> BaseStats;				// List of unit base stats derived from the archetype
	int32 Health = 0;
	int32 Armor = 0;
	
	TArray<int32> StatsEffects;				// List of active effects that modifier the unit stats
	TArray<int32> StatusEffects;			// List of active effects that add a status to the unit

	// Unit actions
	TArray<FName> KnownCombatActions;		// Name of combat actions known by the units. A FName is saved so it can be search in a Data Table
	TArray<int32> EquippedCombatActions;	// Saves an index to the KnownCombatActions array
	FCombatActions* CurrentCombatAction;	// Saves the data of the current combat action in use, to make all calculations needed to apply damage and effects

	// Unit passives
	TArray<FName> KnownPassives;	// Name of combat actions known by the units. A FName is saved so it can be search in a Data Table

	static TArray<TArray<float>> DamageTypeModifiers;

	// Units Textures
	UPROPERTY()
	UTexture2D* Icon;
	UPROPERTY()
	USkeletalMesh* Model3D;
	UPROPERTY()
	USkeletalMeshComponent* Model3DComponent;
	UPROPERTY()
	UAnimInstance* Animation;
	UPROPERTY()
	UAnimBlueprint* AnimationBP;

	// Data Table references
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* UnitsTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* CombatActionsTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* PassivesTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* DamageTypeModifiersTable;

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

	// Called when this unit turn starts
	void TurnStarts();

	// Called when this unit turn ends
	void TurnEnds();

	// Called by PlayerController to make this unit move to a destination
	void MoveUnit(FVector FinalDestination);
	
	// Called by PlayerState to make this unit move to a destination following a preseted path
	void MoveUnit(TArray<FVector>& Path);

	// Set the CurrentCombatAction and return false if the action selected doesn't exist
	void SetCombatAction(int32 ActionId);

	// The unit uses one of its equipped actions
	void UseCurrentAction(ABaseUnit* Objective);

	// Apply a direct damage to the unit and verify if it dies because of it
	void ApplyDamage(int32 Damage, EUnitType DamageType);

	// Apply an effect to a unit
	void ApplyEffect(const FEffects& Effect);

	// Set the Unit State and send a message to all object that need to know about it
	void SetUnitState(EUnitState NewState);

	bool HasActionEquipped(int32 CombatActionSlot) const;

	const EUnitState GetUnitState() const { return CurrentState; }
	const int32 GetUnitStat(EUnitStats Stat) const;
	const float GetDamageTypeModifier(EUnitType DamageType);
	const int32 GetMovement() const { return Movement; }
	const int32 GetCombatActionRangeMin() const { return CurrentCombatAction->MinRange; }
	const int32 GetCombatActionRangeMax() const { return CurrentCombatAction->MaxRange; }
	const FName GetName() const { return Name; }
	const FName GetArchetype() const { return Archetype; }
	UTexture2D* GetIcon() const { return Icon; };

	UFUNCTION(BlueprintCallable)
	const bool IsMoving() const { return CurrentState == EUnitState::Moving; }

	UFUNCTION(BlueprintCallable)
	const bool IsAttacking() const { return CurrentState == EUnitState::Combating; }
};
