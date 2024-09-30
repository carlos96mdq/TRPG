// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnit.h"
#include "TRPGGameStateBase.h"
#include "Animation/AnimInstance.h"

ABaseUnit::ABaseUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Skeleton Component to attach a SkeletonMesh to this unit
	Model3DComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	Model3DComponent->SetupAttachment(RootComponent);

	// Create Animator Component
	//Model3DComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	
	//Model3DComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Initialize the TArrays
	BaseStats.Init(0, (int32)EUnitStats::MAX);
	StatsEffects.Init(0, (int32)EUnitStats::MAX);
	StatusEffects.Init(0, (int32)EStatusEffects::MAX);
}

void ABaseUnit::Init(FName NewArchetype, EUnitControllerOwner NewOwner, int32 NewUnitIndex)
{
	Archetype = NewArchetype;
	ControllerOwner = NewOwner;
	UnitIndex = NewUnitIndex;
}

void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();

	// Get data linked to Unit Name and save it
	FUnits* DTBaseStats = UnitsTable->FindRow<FUnits>(Archetype, "", true);
	check(DTBaseStats);
	
	Name = Archetype;
	Type = DTBaseStats->Type;
	SubType = DTBaseStats->SubType;

	// Load and reference assets needed
	Icon = DTBaseStats->Icon.LoadSynchronous();
	Model3D = DTBaseStats->Model3D.LoadSynchronous();
	AnimationBP = DTBaseStats->AnimationBP.LoadSynchronous();

	// Set unit stats from data
	BaseStats[(int8)EUnitStats::MaxHealth] = DTBaseStats->MaxHealth;
	BaseStats[(int8)EUnitStats::NaturalArmor] = DTBaseStats->NaturalArmor;
	BaseStats[(int8)EUnitStats::MaxEnergy] = DTBaseStats->MaxEnergy;
	BaseStats[(int8)EUnitStats::Strenght] = DTBaseStats->Strenght;
	BaseStats[(int8)EUnitStats::Dexterity] = DTBaseStats->Dexterity;
	BaseStats[(int8)EUnitStats::Special] = DTBaseStats->Special;
	BaseStats[(int8)EUnitStats::Velocity] = DTBaseStats->Velocity;
	Health = GetUnitStat(EUnitStats::MaxHealth);
	Armor = GetUnitStat(EUnitStats::NaturalArmor);
	Energy = GetUnitStat(EUnitStats::MaxEnergy);
	UE_LOG(LogTemp, Display, TEXT("[UNIT STARTS] This unit starts with a Health of %d and an Armor of %d"), Health, Armor);
	//TODO inicializo las pasivas manualmente, esto deberia venir como data de algun lado
	//KnownPassives.Add("Pasiva_test");

	//TODO PLACEHOLDER inicializo la tabla de acciones con algo cargado para pruebas
	AddCombatAction(FName("Ember"));
	AddCombatAction(FName("Growl"));

	// Set SkeletonMesh from datatable
	Model3DComponent->SetSkeletalMesh(Model3D);
	
	// Set Animator from datatable
	//Model3DComponent->SetAnimClass(Animation);
	if (AnimationBP)
	{
		Model3DComponent->SetAnimClass(AnimationBP->GeneratedClass);
	}
	//Model3DComponent->SetAnimClass(AnimationBP->GetClass());
	//Model3DComponent->SetAnimInstance(Animation);
}

void ABaseUnit::TurnStarts()
{
	UE_LOG(LogTemp, Display, TEXT("[UNIT TURN] The unit %s started its turn"), *Name.ToString());
	CurrentState = EUnitState::Idle;

	// TODO	Check if the unit loses its turn

	Armor = GetUnitStat(EUnitStats::NaturalArmor);
	Energy = GetUnitStat(EUnitStats::MaxEnergy);

	OnUnitUpdateStats.Broadcast(UnitIndex);
}

void ABaseUnit::TurnEnds()
{
	// Check for damage coming from Burn or Poison effects and reduce their effects
	for (uint8 i = 0; i < StatusEffects.Num() - 1; i++)
	{
		if ((i == (uint8)EStatusEffects::Burn || i == (uint8)EStatusEffects::Poison) && StatusEffects[i] > 0)
		{
			UE_LOG(LogTemp, Display, TEXT("[UNIT TURN] This unit receives %d damage points from an effect"), StatusEffects[i]);
			ApplyDamage(StatusEffects[i], EUnitType::None);
			StatusEffects[i]--;
		}
	}
}

void ABaseUnit::OnUnitClicked(AActor* ClickedActor, FKey ButtonPressed)
{}

void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EUnitState::Moving)
	{
		Move(DeltaTime);
	}
	else if (CurrentState == EUnitState::Combating)
	{
		UseCurrentAction(CurrentObjective);
	}

	//UE_LOG(LogTemp, Warning, TEXT("La Velocidad de mi unidad es: x: %x, y: %y, z: %z"));
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 300, FColor::Red, false, 2.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorRightVector() * 300, FColor::Green, false, 2.0f, 0, 1.0f);
}

void ABaseUnit::MoveUnit(FVector FinalDestination)
{
	// Create a queue of destinations so the unit can travel only in a vertical and horizontal way
	QueueDestinations.Enqueue(FVector(FinalDestination.X, GetActorLocation().Y, GetActorLocation().Z));
	QueueDestinations.Enqueue(FVector(FinalDestination.X, FinalDestination.Y, GetActorLocation().Z));

	// Set the first step
	QueueDestinations.Dequeue(Destination);
}

void ABaseUnit::MoveUnit(TArray<FVector>& Path)
{
	// Before anything, check and reduce the energy needed to make this movement
	if (Path.Num() <= Energy)
	{
		CurrentState = EUnitState::Moving;
		Energy -= Path.Num();

		for (int32 i = (Path.Num() - 1); i >= 0; i--)
		{
			//FVector CurrentLocation = Path.Pop();
			FVector CurrentLocation = Path[i];
			QueueDestinations.Enqueue(FVector(CurrentLocation.X, CurrentLocation.Y, GetActorLocation().Z));
		}

		// Set the first step
		QueueDestinations.Dequeue(Destination);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UNIT ACTION] This unit is trying to move a distance greater than its energy"));
		CurrentState = EUnitState::Idle;
	}
}

void ABaseUnit::Move(float DeltaTime)
{
	// Move
	FVector NextStep = FMath::VInterpConstantTo(
		GetActorLocation(),
		Destination,
		DeltaTime,
		600
	);

	SetActorLocation(NextStep);

	// Rotate
	FVector MovementDirection = Destination - GetActorLocation();
	MovementDirection = MovementDirection.GetSafeNormal();
	if (MovementDirection != FVector::ZeroVector)
	{
		FRotator NewRotation = MovementDirection.Rotation();
		//FRotator InterpolatedRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, InterpSpeed);
		SetActorRotation(NewRotation);
	}

	// Verify if the unit reached the current destination and if there are new ones to reach
	if (GetActorLocation() == Destination)
	{
		if (!QueueDestinations.Dequeue(Destination))
		{
			CurrentState = EUnitState::Idle;
			OnUnitUpdateStats.Broadcast(UnitIndex);
			OnUnitStopsMoving.Broadcast(UnitIndex);
		}
	}
}

void ABaseUnit::AddCombatAction(FName ActionName)
{
	FCombatActions* CombatAction = CombatActionsTable->FindRow<FCombatActions>(ActionName, "", true);

	if (CombatAction)
		CombatActions.Add(CombatAction);
	else
		UE_LOG(LogTemp, Warning, TEXT("[UNIT STARTS] The Unit is trying to add a Combat Action that doesn't exist in data tables"));
}

void ABaseUnit::SetCombatAction(int32 ActionIdx)
{
	if (ActionIdx >= 0 && ActionIdx < CombatActions.Num())
	{
		CurrentCombatAction = CombatActions[ActionIdx];
		check(CurrentCombatAction);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("[COMBAT] The Unit is trying to use a Combat Action that doesn't have"));
}

bool ABaseUnit::TryUsingCurrentAction(ABaseUnit* Objective)
{
	// TODO Verify if this action can be used in that objective
	// TODO Check for actions effects that are executed before accuracy check
	UE_LOG(LogTemp, Display, TEXT("[COMBAT] The unit %s tries to use the curent action"), *Name.ToString());
	if (Objective->IsAlive() && CurrentCombatAction->Energy <= Energy)
	{
		CurrentObjective = Objective;
		CurrentState = EUnitState::Combating;
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[COMBAT] The unit %s can't use the action (energy insufficient or invalid objective)"), *Name.ToString());
		CurrentState = EUnitState::Idle;
		return false;
	}
}

void ABaseUnit::UseCurrentAction(ABaseUnit* Objective)
{
	CurrentState = EUnitState::Combating;
	Energy -= CurrentCombatAction->Energy;

	// Check if this action pass the accuracy check (if it hits)
	bool IsCritic = false;
	bool IsSuccess = false;
	int32 HitChanceDie = FMath::RandRange(1, 100);
	int32 HitThreshold = GetUnitStat(EUnitStats::Accuracy) + CurrentCombatAction->Accuracy - Objective->GetUnitStat(EUnitStats::EvadeChance);
	int32 CriticThreshold = GetUnitStat(EUnitStats::CriticChance);	//TODO ver el tema del critico

	// If it is a critic it allways hit, don't matter the objective DoggingChance 
	if (HitChanceDie <= CriticThreshold)
		IsCritic = true;
	else if (HitChanceDie <= HitThreshold)
		IsSuccess = true;

	// The action passed the accuracy check, so continue
	if (IsSuccess || IsCritic)
	{
		UE_LOG(LogTemp, Display, TEXT("[COMBAT] The CombatAction launched by %s hit %s"), *Name.ToString(), *(Objective->Name).ToString());
		// First apply the damge only if this Action is an Attack
		if (CurrentCombatAction->Type == ECombatActionType::Attack)
		{
			// Get Attack Damage
			int32 Damage = CurrentCombatAction->Power;

			switch (CurrentCombatAction->Modifier)
			{
			case EDamageModifier::Strenght:
				Damage += GetUnitStat(EUnitStats::Strenght);
				break;
			case EDamageModifier::Dexterity:
				Damage += GetUnitStat(EUnitStats::Dexterity);
				break;
			case EDamageModifier::Special:
				Damage += GetUnitStat(EUnitStats::Special);
				break;
			default:
				break;
			}

			if (IsCritic)
				Damage = FMath::Floor(Damage * 1.5);

			UE_LOG(LogTemp, Display, TEXT("[COMBAT] It was an Attack with a total damage of %d"), Damage);

			// Apply the damage to the Objective
			Objective->ApplyDamage(Damage, CurrentCombatAction->DamageType);
		}

		// Check for action effects (in case of Skills, they only have effects but don't do damage)
		if (CurrentCombatAction->Effects.Num() > 0)
		{
			ABaseUnit* EffectObjective;
			for (const FEffects& Effect : CurrentCombatAction->Effects)
			{
				// Set the effect's objective because it can be different than the attack/skill objective
				switch (Effect.Objective)
				{
				case EObjectiveType::User:
					EffectObjective = this;
					break;
				case EObjectiveType::Enemy:
					EffectObjective = Objective;
					break;
				default:
					EffectObjective = Objective;
					break;
				}

				// Check for chance
				int32 Chance = Effect.Chance;
				int32 ChanceDie = FMath::RandRange(1, 100);

				if (ChanceDie <= Chance)
					EffectObjective->ApplyEffect(Effect);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[COMBAT] The CombatAction missed"));
	}

	CurrentState = EUnitState::Idle;
	OnUnitUpdateStats.Broadcast(UnitIndex);
	OnUnitStopsAction.Broadcast(UnitIndex);
}

void ABaseUnit::ApplyDamage(int32 Damage, EUnitType DamageType)
{
	int32 FinalDamage = Damage;
	UE_LOG(LogTemp, Display, TEXT("[COMBAT] This unit receive a damage to process of: %d, and has a current Armor of: %d"), Damage, Armor);

	FinalDamage -= Armor;
	Armor -= Damage;

	if (Armor < 0)
		Armor = 0;
	if (FinalDamage < 0)
		FinalDamage = 0;
	UE_LOG(LogTemp, Display, TEXT("[COMBAT] After processing the armor, this unit get a Final Damage of: %d, and has an Armor of: %d"), FinalDamage, Armor);

	// Apply only non-negative Damage
	if (FinalDamage > 0)
	{
		FinalDamage *= GetDamageTypeModifier(DamageType);
		FinalDamage = (int32)floorf(FinalDamage);
		if (FinalDamage == 0)
			FinalDamage = 1;
		UE_LOG(LogTemp, Display, TEXT("[COMBAT] After getting the damage type modifier, this unit get a Final Damage of: %d"), FinalDamage);

		Health = Health - FinalDamage;

		UE_LOG(LogTemp, Display, TEXT("[COMBAT] Health after damage: %d"), Health);

		// If this unit Health reaches 0, kill this unit
		if (Health <= 0)
			UnitDies();
	}
	else
		UE_LOG(LogTemp, Display, TEXT("[COMBAT] The Final Damage wasn't enought to damage this unit"));

	// This event is called to update any visual interface that need to change the units stats
	OnUnitUpdateStats.Broadcast(UnitIndex);
}

void ABaseUnit::ApplyEffect(const FEffects& Effect)
{
	UE_LOG(LogTemp, Display, TEXT("[COMBAT] Apply the Combat Action Effect to this objective unit"));
	
	if (Effect.Type == EEffectTypes::Stats)
	{
		check((uint8)Effect.NameStat < StatsEffects.Num());
		StatsEffects[(uint8)Effect.NameStat] += Effect.Value;
	}
	else if (Effect.Type == EEffectTypes::Status)
	{
		check((uint8)Effect.NameStatus < StatusEffects.Num());
		StatusEffects[(uint8)Effect.NameStatus] += Effect.Value;
	}
}

void ABaseUnit::UnitDies()
{
	UE_LOG(LogTemp, Display, TEXT("[COMBAT] This unit just died"));

	// Set unit parameters and visual according to the dead state
	bIsAlive = false;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	// Let know other objects that this unit dies so all changes needed are made
	//OnUnitDies.Broadcast(UnitRegistrationIndex);
}

void ABaseUnit::SetUnitState(EUnitState NewState)
{
	CurrentState = NewState;
}

bool ABaseUnit::HasActionEquipped(int32 CombatActionIndex) const
{
	if (CombatActionIndex >= 0 && CombatActionIndex < CombatActions.Num())
		return true;
	else
		return false;
}

int32 ABaseUnit::GetCombatActionEnergyCost(int32 ActionIdx) const
{
	if (ActionIdx == -1)
	{
		check(CurrentCombatAction);
		return CurrentCombatAction->Energy;
	}
	else
	{
		FCombatActions* CombatAction = CombatActions[ActionIdx];
		check(CombatAction)
		return CombatAction->Energy;
	}
}

int32 ABaseUnit::GetUnitStat(EUnitStats Stat) const
{
	int32 StatValue = 0;
	
	// Add base value coming from unit archetype base stats
	check((uint8)Stat < BaseStats.Num())
	StatValue += BaseStats[(uint8)Stat];

	// Add modifier value from stats effects
	check((uint8)Stat < StatsEffects.Num());
	StatValue += StatsEffects[(uint8)Stat];

	// Add values coming from unit pasives
	//for (FName PassiveName : KnownPassives)
	//{
	//	FPassives* Passive = PassivesTable->FindRow<FPassives>(PassiveName, "", true);
	//	check(Passive);

	//	if (Passive->Type == EPassiveType::Start)
	//	{
	//		for (const FEffects& Effect : Passive->Effects)
	//		{
	//			// Check only for effects that apply to this stat. As Start Stats Passives are initialized allways, ignore the probability check
	//			if (Effect.Type == EEffectName::Stats && Effect.SubType == (uint8)Stat && (Effect.Objective == EObjectiveType::User || Effect.Objective == EObjectiveType::Allies))
	//			{
	//				if (Effect.IsValuePercent)
	//				{
	//					StatValue = FMath::Floor(StatValue * (1 + Effect.Value / 100));
	//				}
	//				else
	//				{
	//					StatValue += Effect.Value;
	//				}
	//			}
	//		}
	//	}
	//}



	return StatValue;
}

float ABaseUnit::GetDamageTypeModifier(EUnitType DamageType) const
{
	ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
	check(GameState);

	float ResistanceValue = GameState->GetDamageTypeModifier((int32)DamageType, (int32)Type, (int32)SubType);
	return ResistanceValue;
}