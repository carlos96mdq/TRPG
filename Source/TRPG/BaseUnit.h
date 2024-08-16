// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "BaseUnit.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnitStopsMoving, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnitStopsAction, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnitUpdateStats, int32)

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
	MaxEnergy,
	Strenght,
	Dexterity,
	Special,
	Velocity,
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
	int32 Energy = 0;
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
	int32 MaxEnergy = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Strenght = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Dexterity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Special = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Velocity = 0;
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when unit is clicked by mouse
	UFUNCTION()
	void OnUnitClicked(AActor* ClickedActor, FKey ButtonPressed);
	
	// Called in tick to move unit
	void Move(float DeltaTime);

	void AddCombatAction(FName ActionName);

	// General Data
	bool bIsPlayer = false;
	bool bIsAlive = true;
	int32 UnitPlayerIndex = -1;
	FName Name = TEXT("None");
	FName Archetype = TEXT("None");
	EUnitType Type = EUnitType::None;
	EUnitType SubType = EUnitType::None;
	EUnitState CurrentState = EUnitState::Idle;
	
	// Stats and Status
	TArray<int32> BaseStats;
	int32 Health = 0;
	int32 Armor = 0;
	int32 Energy = 0;
	TArray<int32> StatsEffects;				// List of active effects that modifier the unit stats
	TArray<int32> StatusEffects;			// List of active effects that add a status to the unit
	TArray<FCombatActions*> CombatActions;
	FCombatActions* CurrentCombatAction;
	ABaseUnit* CurrentObjective;

	// Unit passives
	TArray<FName> KnownPassives;	// Name of combat actions known by the units. A FName is saved so it can be search in a Data Table

	
	// Graphics Data
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
	TQueue<FVector> QueueDestinations;
	FVector Destination = FVector::ZeroVector;

	// Data Table References
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* UnitsTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* CombatActionsTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* PassivesTable;
	
public:	
	// Sets default values for this actor's properties
	ABaseUnit();

	// Initialize the unit with some parameters needed to get the unit data from the data tables
	void Init(FName NewArchetype, bool IsPlayer=false);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FOnUnitStopsMoving OnUnitStopsMoving;
	FOnUnitStopsAction OnUnitStopsAction;
	FOnUnitUpdateStats OnUnitUpdateStats;

	// Called when this unit turn starts
	virtual void TurnStarts();

	// Called when this unit turn ends
	void TurnEnds();

	// Called by PlayerController to make this unit move to a destination
	void MoveUnit(FVector FinalDestination);
	
	// Called by PlayerState to make this unit move to a destination following a preseted path
	void MoveUnit(TArray<FVector>& Path);

	// Set the CurrentCombatAction
	void SetCombatAction(int32 ActionIdx);

	// The unit uses one of its equipped actions
	bool TryUsingCurrentAction(ABaseUnit* Objective);
	void UseCurrentAction(ABaseUnit* Objective);

	// Apply a direct damage to the unit and verify if it dies because of it
	void ApplyDamage(int32 Damage, EUnitType DamageType);

	// Apply an effect to a unit
	void ApplyEffect(const FEffects& Effect);

	// Set the Unit State and send a message to all object that need to know about it
	void SetUnitState(EUnitState NewState);

	void SetUnitPlayerIndex(int32 NewIdx) { UnitPlayerIndex = NewIdx; }

	bool HasActionEquipped(int32 CombatActionIndex) const;

	EUnitState GetUnitState() const { return CurrentState; }
	int32 GetUnitStat(EUnitStats Stat) const;
	float GetDamageTypeModifier(EUnitType DamageType) const;
	int32 GetCombatActionRangeMin() const { return CurrentCombatAction->MinRange; }
	int32 GetCombatActionRangeMax() const { return CurrentCombatAction->MaxRange; }
	int32 GetCombatActionEnergyCost(int32 ActionIdx=-1) const;
	FName GetName() const { return Name; }
	int32 GetLife() const { return Health; }
	int32 GetArmor() const { return Armor; }
	int32 GetEnergy() const { return Energy; }
	UTexture2D* GetIcon() const { return Icon; }
	bool IsPlayer() const { return bIsPlayer; }
	bool IsAlive() const { return bIsAlive; }
	int32 GetUnitPlayerIndex() const { return UnitPlayerIndex; }

	UFUNCTION(BlueprintCallable)
	bool IsMoving() const { return CurrentState == EUnitState::Moving; }

	UFUNCTION(BlueprintCallable)
	bool IsAttacking() const { return CurrentState == EUnitState::Combating; }
};
