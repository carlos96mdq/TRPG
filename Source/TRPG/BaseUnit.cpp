// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnit.h"
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
}

void ABaseUnit::Init(FName NewArchetype)
{
	Name = NewArchetype;
	Archetype = NewArchetype;
}

void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();

	// Get data linked to Unit Name and save it
	BaseStats = UnitsTable->FindRow<FUnits>(Archetype, "", true);
	check(BaseStats);
	
	// Set unit resistances using the just aquired type
	AddTypeModifiers(Type);
	AddTypeModifiers(SubType);

	// Load and reference assets needed
	Icon = BaseStats->Icon.LoadSynchronous();
	Model3D = BaseStats->Model3D.LoadSynchronous();
	AnimationBP = BaseStats->AnimationBP.LoadSynchronous();

	// Set variable stats using the base stats
	Health = BaseStats->MaxHealth;
	Armor = BaseStats->NaturalArmor;
	UE_LOG(LogTemp, Warning, TEXT("This unit starts with a Helath of %d and an Armor of %d"), Health, Armor);
	//TODO inicializo las pasivas manualmente, esto deberia venir como data de algun lado
	//KnownPassives.Add("Pasiva_test");

	//TODO PLACEHOLDER inicializo la tabla de acciones con algo cargado para pruebas
	EquippedCombatActions.Init(-1, 5);
	EquippedCombatActions[0] = KnownCombatActions.Add("Ember");
	EquippedCombatActions[1] = KnownCombatActions.Add("Growl");

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
	// Check if the unit loses its turn

	// Reset the Unit's armor if left

}

void ABaseUnit::TurnEnds()
{
	// Add Unit natural Armor to its Armor

	// List of Active Effects to remove because they wasted
	TArray<EEffectName> EffectsToRemove;

	// Check for damage coming from Burn or Poison effects and reduce their effects
	for (TTuple<EEffectName, int32> &Effect : ActiveEffects)
	{
		if (Effect.Key == EEffectName::Burn || Effect.Key == EEffectName::Poison)
		{
			ApplyDamage(Effect.Value, EUnitType::None);
			UE_LOG(LogTemp, Warning, TEXT("This unit receives %d damage points from an effect"), Effect.Value);
			Effect.Value--;
			if (Effect.Value <= 0)
				EffectsToRemove.Emplace(Effect.Key);
		}
	}

	// Remove all effects that finished
	for (EEffectName EffectName : EffectsToRemove)
		ActiveEffects.Remove(EffectName);

	// TODO this will stay here as future options. Delete when not needed anymore.
	//for (FActiveEffect *AE : ActiveEffects)
	//{
	//	switch (AE->Name)
	//	{
	//		case Burn: 
	//			HandleBurn(AE);
	//		break
	//	}
	//}

}

void ABaseUnit::OnUnitClicked(AActor* ClickedActor, FKey ButtonPressed)
{
	//UE_LOG(LogTemp, Warning, TEXT("Unidad seleccionada"));
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
		}
	}
}

void ABaseUnit::AddTypeModifiers(EUnitType NewType)
{
}

void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (CurrentState == EUnitState::Moving)
	{
		Move(DeltaTime);
	}

	FVector Speed = GetVelocity();
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
	for (int32 i = (Path.Num() - 1); i >= 0; i--) 
	{
		//FVector CurrentLocation = Path.Pop();
		FVector CurrentLocation = Path[i];
		QueueDestinations.Enqueue(FVector(CurrentLocation.X, CurrentLocation.Y, GetActorLocation().Z));
	}
	
	// Set the first step
	QueueDestinations.Dequeue(Destination);
}

void ABaseUnit::SetCombatAction(int32 ActionId)
{
	// Verify if the action used is valid
	check(ActionId >= 0 && ActionId <= 4 && EquippedCombatActions[ActionId] >= 0 && EquippedCombatActions[ActionId] < KnownCombatActions.Num());

	// Get action data from data table
	CurrentCombatAction = CombatActionsTable->FindRow<FCombatActions>(KnownCombatActions[EquippedCombatActions[ActionId]], "", true);
	check(CurrentCombatAction);
}

void ABaseUnit::UseCurrentAction(ABaseUnit* Objective)
{
	// TODO Verify if this action can be used in that objective

	// TODO Check for actions effects that are executed before accuracy check

	// Check if this action pass the accuracy check (if it hits)
	bool IsCritic = false;
	bool IsSuccess = false;
	int32 HitChanceDie = FMath::RandRange(1, 100);
	int32 HitThreshold = GetUnitStat(EUnitStats::Accuracy) + CurrentCombatAction->Accuracy - Objective->GetUnitStat(EUnitStats::DoggingChance);
	int32 CriticThreshold = GetUnitStat(EUnitStats::CriticChance);	//TODO ver el tema del critico
	
	// If it is a critic it allways hit, don't matter the objective DoggingChance 
	if (HitChanceDie <= CriticThreshold)
		IsCritic = true;
	else if (HitChanceDie <= HitThreshold)
		IsSuccess = true;

	// The action passed the accuracy check, so continue
	if (IsSuccess || IsCritic)
	{
		UE_LOG(LogTemp, Warning, TEXT("The Attack launched by %s hit %s"), *Name.ToString(), *(Objective->Name).ToString());

		// First apply the damge only if this Action is an Attack
		if (CurrentCombatAction->Type == ECombatActionType::Attack)
		{
			// Get Attack base power
			int32 Damage = CurrentCombatAction->Power;
			UE_LOG(LogTemp, Warning, TEXT("This attack has a power of %d"), Damage);
			// Apply unit damage modifier according to damage type
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
			UE_LOG(LogTemp, Warning, TEXT("And after adding this unit Modifier the damage is %d"), Damage);
			// Apply critic bonus
			if (IsCritic)
			{
				Damage = FMath::Floor(Damage * 1.5);
				UE_LOG(LogTemp, Warning, TEXT("And after adding the critic bonus it is %d"), Damage);
			}
				
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
					EffectObjective->ApplyEffect(Effect.Name, Effect.Value);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack missed"));
	}
}

void ABaseUnit::ApplyDamage(int32 Damage, EUnitType DamageType)
{
	int32 FinalDamage = Damage;
	UE_LOG(LogTemp, Warning, TEXT("This unit receive a damage to process of: %d, and has a current Armor of: %d"), Damage, Armor);

	// First apply the unit armor
	FinalDamage -= Armor;
	Armor -= Damage;

	// Check for not negative Armor
	if (Armor < 0)
		Armor = 0;
	UE_LOG(LogTemp, Warning, TEXT("After processing the armor, this unit get a Final Damage of: %d, and has an Armor of: %d"), FinalDamage, Armor);
	// TODO Reduce or add the unit resistenace or weakness to the Final Damage
	FinalDamage -= GetUnitResistance(DamageType);

	// Apply any non-negative Damage to the Unit's Health
	if (FinalDamage > 0)
	{
		Health = Health - FinalDamage;

		UE_LOG(LogTemp, Warning, TEXT("Current Damage: %d"), Damage);
		UE_LOG(LogTemp, Warning, TEXT("Current Health: %d"), Health);

		// If this unit Helath reaches 0, kill this unit
		if (true)
		{
			UE_LOG(LogTemp, Warning, TEXT("This unit just died"));
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("The Final Damage wasn't enought to damage this unit"));
}

void ABaseUnit::ApplyEffect(EEffectName EffectName, int32 Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Apply the Combat Action Effect to this objective unit"));
	// Apply effect to objective unit
	if (ActiveEffects.Contains(EffectName))
		ActiveEffects[EffectName] += Value;
	else
		ActiveEffects.Add(EffectName, Value);
}

void ABaseUnit::SetUnitState(EUnitState NewState)
{
	CurrentState = NewState;
}

bool ABaseUnit::HasActionEquipped(int32 CombatActionSlot) const
{
	UE_LOG(LogTemp, Warning, TEXT("HasActionEquipped"));
	//UE_LOG(LogTemp, Warning, TEXT("CombatActionSlot: %d, KnownCombatActions: %d"), CombatActionSlot, EquippedCombatActions[CombatActionSlot]);
	check(CombatActionSlot >= 0 && CombatActionSlot <= 4);
	if (EquippedCombatActions[CombatActionSlot] >= 0 && EquippedCombatActions[CombatActionSlot] < KnownCombatActions.Num())
		return true;
	else
		return false;
}

const int32 ABaseUnit::GetUnitStat(EUnitStats Stat) const
{
	int32 StatValue = 0;
	
	// Add base value coming from unit archetype base stats
	switch (Stat)
	{
		case EUnitStats::MaxHealth:
			StatValue += BaseStats->MaxHealth;
			break;
		case EUnitStats::NaturalArmor:
			StatValue += BaseStats->NaturalArmor;
			break;
		case EUnitStats::Strenght:
			StatValue += BaseStats->Strenght;
			if (ActiveEffects.Contains(EEffectName::Strenght))
				StatValue += ActiveEffects[EEffectName::Strenght];
			UE_LOG(LogTemp, Warning, TEXT("This unit Strenght value is %d"), StatValue);
			break;
		case EUnitStats::Dexterity:
			StatValue += BaseStats->Dexterity;
			if (ActiveEffects.Contains(EEffectName::Dexterity))
				StatValue += ActiveEffects[EEffectName::Dexterity];
			break;
		case EUnitStats::Special:
			StatValue += BaseStats->Special;
			if (ActiveEffects.Contains(EEffectName::Special))
				StatValue += ActiveEffects[EEffectName::Special];
			break;
		case EUnitStats::Velocity:
			StatValue += BaseStats->Velocity;
			break;
		default:
			break;
	}

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

	//TODO a eso agregarle cualquier modificacion surgida por el equipamiento

	// TODO Depending on the stat check for different active status that modify that stat


	return StatValue;
}

const int32 ABaseUnit::GetUnitResistance(EUnitType DamageType)
{
	int32 ResistanceValue = 0;

	// Add base value coming from unit archetype base stats	//TODO esto queda comentado para futuro, pero por ahora la resistencia no se almacena en el data asset, sino que depende al tipo de la unidad
	//switch (DamageType)
	//{
	//case EUnitResistances::Slashing:
	//	ResistanceValue += BaseStats->Resistances.Slashing;
	//	break;
	//case EUnitResistances::Bludgeoning:
	//	ResistanceValue += BaseStats->Resistances.Bludgeoning;
	//	break;
	//case EUnitResistances::Piercing:
	//	ResistanceValue += BaseStats->Resistances.Piercing;
	//	break;
	//case EUnitResistances::Fire:
	//	ResistanceValue += BaseStats->Resistances.Fire;
	//	break;
	//case EUnitResistances::Poison:
	//	ResistanceValue += BaseStats->Resistances.Poison;
	//	break;
	//case EUnitResistances::Lightning:
	//	ResistanceValue += BaseStats->Resistances.Lightning;
	//	break;
	//default:
	//	break;
	//}

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
	//			if (Effect.Type == EEffectName::Resistances && Effect.SubType == (uint8)DamageType && (Effect.Objective == EObjectiveType::User || Effect.Objective == EObjectiveType::Allies))
	//			{
	//				if (Effect.IsValuePercent)
	//				{
	//					ResistanceValue = FMath::Floor(ResistanceValue * (1 + Effect.Value / 100));
	//				}
	//				else
	//				{
	//					ResistanceValue += Effect.Value;
	//				}
	//			}
	//		}
	//	}
	//}

	//TODO a eso agregarle cualquier modificacion surgida por el equipamiento

	//TODO finalmente tener en cuenta cualquier status que pueda afectar

	return ResistanceValue;
}