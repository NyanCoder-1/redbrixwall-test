#include "Logic.h"
#include <cmath>
#include <random>
#include <ctime>

#define M_PI       3.14159265358979323846

// constants definition
const float simulationStep = 0.05f;
const std::array<std::array<float, 4>, 2> teamsColors = {
	std::array<float, 4>{ 0.784f, 0.298f, 0.259f, 1.0f },
	std::array<float, 4>{ 0.255f, 0.49f, 0.867f, 1.0f },
};
const uint32_t archersCount = 20;
const Position team1Position = { 10.0f, 10.0f, 0.0f };
const Position team2Position = { 90.0f, 90.0f, 0.0f };
const float archerHeight = 1.7f;
const float archerRadius = 0.25f;
const float walkSpeed = 1.7f;
const float arrowStartSpeed = 40.0f;
const float arrowStartElevation = 1.5f;
const float arrowStartDistance = 0.5f;
const float shootCooldown = 1.5f;
const float spawnerCooldown = 0.1f;
const float firingRange = 40.0f;
const Velocity gravity = { 0.0f, 0.0f, -9.8f };
const Health archerFullHealth = 5;

const bool friendlyFire = false;
const bool waitForSpawnWholeTeam = true;

Logic::Logic()
{
	const auto spawner1 = entities.create();
	entities.emplace<Spawner>(spawner1, archersCount, uint32_t(0));
	entities.emplace<Position>(spawner1, team1Position);
	const auto spawner2 = entities.create();
	entities.emplace<Spawner>(spawner2, archersCount, uint32_t(1));
	entities.emplace<Position>(spawner2, team2Position);
	simulationState = waitForSpawnWholeTeam ? eSimulationState::Prepare : eSimulationState::Play;
	srand(time(0));
}
void Logic::Update()
{
	float dTime = simulationStep;

	switch (simulationState)
	{
	case eSimulationState::Prepare:
		updateSpawners(dTime);
		updatePhysics(dTime);
		if (spawnersDone())
			simulationState = eSimulationState::Play;
		break;

	case eSimulationState::Play:
		updateSpawners(dTime);
		updateBehaviour(dTime);
		updatePhysics(dTime);
		break;
	}
}
entt::registry& Logic::GetRegistry()
{
	return entities;
}
void Logic::updateSpawners(float dTime)
{
	auto spawnersView = entities.view<const Position, Spawner>();
	for (auto [spawnerEntity, positionComponent, spawnerComponent] : spawnersView.each()) {
		if (spawnerComponent.archersLeft)
		{
			spawnerComponent.cooldown -= dTime;
			if (spawnerComponent.cooldown < 0.0f)
			{
				spawnerComponent.cooldown += spawnerCooldown;
				spawnerComponent.archersLeft--;
				const auto newArcherEntity = entities.create();
				entities.emplace<Position>(newArcherEntity, positionComponent.x, positionComponent.y, positionComponent.z);
				entities.emplace<Velocity>(newArcherEntity, 0.0f, 0.0f, 0.0f);
				entities.emplace<ColliderCylinder>(newArcherEntity, archerHeight, archerRadius);
				entities.emplace<Archer>(newArcherEntity);
				entities.emplace<Team>(newArcherEntity, spawnerComponent.teamId);
				entities.emplace<Health>(newArcherEntity, archerFullHealth);
				entities.emplace<Tags::Archer>(newArcherEntity);
				switch (spawnerComponent.teamId)
				{
				case 0:
					entities.emplace<Tags::Team1>(newArcherEntity);
					break;
				case 1:
					entities.emplace<Tags::Team2>(newArcherEntity);
					break;
				}
			}
		}
		else
		{
			entities.destroy(spawnerEntity);
		}
	};
}
void Logic::updateBehaviour(float dTime)
{
	auto archersView = entities.view<const Position, Velocity, Archer, const Team, const Tags::Archer>();
	for (auto [currentArcherEntity, currentArcherPositionComponent, currentArcherVelocityComponent, currentArcherArcherComponent, currentArcherTeamComponent] : archersView.each()) // shooter
	{
		std::optional<Position> closestTarget;
		float distanceToClosestTarget = 0.0f;
		// pick closest enemy
		for (auto [enemyEntity, enemyPositionComponent, enemyVelocityComponent, enemyArcherComponent, enemyTeamComponent] : archersView.each()) // target
		{
			// skip teammates
			if (currentArcherTeamComponent == enemyTeamComponent)
				continue;
			float distanceToEnemy = Distance(currentArcherPositionComponent, enemyPositionComponent);
			if (!closestTarget.has_value() || distanceToClosestTarget > distanceToEnemy)
			{
				closestTarget = enemyPositionComponent;
				distanceToClosestTarget = distanceToEnemy;
			}
		}

		currentArcherVelocityComponent = { 0.0f, 0.0f, 0.0f };

		if (!closestTarget.has_value())
			continue;

		if (distanceToClosestTarget <= firingRange)
		{
			currentArcherArcherComponent.shootCooldown -= dTime;
			if (currentArcherArcherComponent.shootCooldown < 0)
			{
				if (auto arrowVelocity = Aim(currentArcherPositionComponent, closestTarget.value()); arrowVelocity.has_value())
				{
					Position arrowShiftForward = { arrowVelocity->dX / arrowStartSpeed * arrowStartDistance, arrowVelocity->dY / arrowStartSpeed * arrowStartDistance, arrowVelocity->dZ / arrowStartSpeed * arrowStartDistance };
					const auto newArrowEntity = entities.create();
					entities.emplace<Position>(newArrowEntity, currentArcherPositionComponent.x + arrowShiftForward.x, currentArcherPositionComponent.y + arrowShiftForward.y, currentArcherPositionComponent.z + arrowStartElevation + arrowShiftForward.z);
					entities.emplace<Velocity>(newArrowEntity, arrowVelocity->dX, arrowVelocity->dY, arrowVelocity->dZ);
					entities.emplace<Team>(newArrowEntity, currentArcherTeamComponent);
					entities.emplace<Tags::Arrow>(newArrowEntity);
					entities.emplace<Tags::Gravity>(newArrowEntity);
					currentArcherArcherComponent.shootCooldown += shootCooldown;
				}
			}
			if (currentArcherArcherComponent.shootCooldown < 0)
				currentArcherArcherComponent.shootCooldown = 0;
		}
		else
		{
			currentArcherVelocityComponent = { (closestTarget->x - currentArcherPositionComponent.x) / distanceToClosestTarget * walkSpeed, (closestTarget->y - currentArcherPositionComponent.y) / distanceToClosestTarget * walkSpeed, 0.0f };
		}

	}
}
void Logic::updatePhysics(float dTime)
{
	const uint32_t subStepsAmount = 8;
	const float subDTime = dTime / subStepsAmount;
	for (int subStep = 0; subStep < subStepsAmount; subStep++)
	{
		applyGravity(subDTime);
		updateMovement(subDTime);
		solveCollision();
		keepInField();
		checkArrowOverlapping();
	}
}
void Logic::applyGravity(float dTime)
{
	auto gravityView = entities.view<Velocity, const Tags::Gravity>();
	for (auto [gravityEntity, velocityComponent] : gravityView.each())
	{
		velocityComponent.dX += gravity.dX * dTime;
		velocityComponent.dY += gravity.dY * dTime;
		velocityComponent.dZ += gravity.dZ * dTime;
	}
}
void Logic::updateMovement(float dTime)
{
	auto movingObjectView = entities.view<Position, const Velocity>();
	for (auto [movingEntity, positionComponent, velocityComponent] : movingObjectView.each())
	{
		positionComponent.x += velocityComponent.dX * dTime;
		positionComponent.y += velocityComponent.dY * dTime;
		positionComponent.z += velocityComponent.dZ * dTime;
	}
}
void Logic::solveCollision()
{
	auto archersView = entities.view<Position, const ColliderCylinder, const Tags::Archer>();
	for (auto [archerEntity1, positionComponent1, colliderComponent1] : archersView.each())
	{
		for (auto [archerEntity2, positionComponent2, colliderComponent2] : archersView.each())
		{
			if (archerEntity1 == archerEntity2)
				continue;
			Position collisionAxis = { positionComponent1.x - positionComponent2.x, positionComponent1.y - positionComponent2.y, positionComponent1.z - positionComponent2.z };
			float distance = Length(collisionAxis);
			Position direction;
			if (distance < 0.00001f) // preventing stuck by providing random collizion axis if colliders in the same coordinate
			{
				distance = 0.00001f;
				float randomAngle = (rand() % 200) * 0.01f * M_PI;
				direction = { cosf(randomAngle), sinf(randomAngle), 0.0f };
			}
			else
			{
				direction = { collisionAxis.x / distance, collisionAxis.y / distance, collisionAxis.z / distance };
			}
			const float delta = (colliderComponent1.radius + colliderComponent2.radius) - distance;
			if (delta > 0.0f)
			{
				positionComponent1.x += direction.x * delta / 2.0f;
				positionComponent1.y += direction.y * delta / 2.0f;
				positionComponent1.z += direction.z * delta / 2.0f;
				positionComponent2.x -= direction.x * delta / 2.0f;
				positionComponent2.y -= direction.y * delta / 2.0f;
				positionComponent2.z -= direction.z * delta / 2.0f;
			}
		}
	}
}
void Logic::keepInField()
{
	auto archersView = entities.view<Position, const Tags::Archer>();
	for (auto [archerEntity, positionComponent] : archersView.each())
	{
		if (positionComponent.x < 0)
			positionComponent.x = 0;
		if (positionComponent.x > 100)
			positionComponent.x = 100;
		if (positionComponent.y < 0)
			positionComponent.y = 0;
		if (positionComponent.y > 100)
			positionComponent.y = 100;
	}
	auto arrowView = entities.view<const Position, const Tags::Arrow>();
	for (auto [arrowEntity, positionComponent] : arrowView.each())
	{
		if (positionComponent.z < 0.0f)
			entities.destroy(arrowEntity);
	}
}
void Logic::checkArrowOverlapping()
{
	auto arrowsView = entities.view<const Position, const Team, const Tags::Arrow>();
	for (auto [arrowEntity, arrowPositionComponent, arrowTeamComponent] : arrowsView.each())
	{
		auto takingDamageView = entities.view<const Position, const ColliderCylinder, const Team, Health>();
		for (auto [takingDamageEntity, takingDamagePositionComponent, takingDamageColliderComponent, takingDamageTeamComponent, takingDamageHealthComponent] : takingDamageView.each())
		{
			if ((friendlyFire || (arrowTeamComponent != takingDamageTeamComponent)) && Overlaps(takingDamageColliderComponent, takingDamagePositionComponent, arrowPositionComponent))
			{
				entities.destroy(arrowEntity);
				takingDamageHealthComponent--;
				if (takingDamageHealthComponent == 0)
					entities.destroy(takingDamageEntity);
				break;
			}
		}
	}
}

bool Logic::spawnersDone()
{
	auto spawnersView = entities.view<const Spawner>();
	for (auto [spawnerEntity, spawnerComponent] : spawnersView.each()) {
		if (spawnerComponent.archersLeft)
			return false;
	}
	return true;
}

// helper functions definitions
bool Overlaps(ColliderCylinder collider, Position entity, Position point)
{
	// assume that the collider always vertical, no tilting
	return (Distance({ entity.x, entity.y, 0.0f }, { point.x, point.y, 0.0f }) < collider.radius) && (point.z >= entity.z) && (point.z <= (entity.z + collider.height));
}
float Distance(Position point1, Position point2)
{
	return Length({ point1.x - point2.x, point1.y - point2.y, point1.z - point2.z });
}
float Length(Position vec)
{
	return std::sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}
std::optional<Velocity> Aim(Position me, Position target)
{
	Position delta = { target.x - me.x, target.y - me.y, 0.0f /*target.z - me.z*/ }; // currently supports only on the same elevation
	float distance = Length(delta);

	//
	// distance = speedVector * time + (gravityVector * time^2) / 2
	// 
	// speedVert = gravityVectorLength * time / 2
	// speedHor = distance / time
	// 
	// (gravityVectorLength^2 * time^4) / 2^2 - arrowSpeed^2 * time^2 + distance^2 = 0
	// (9.8/2)^2 * time^4 - 40^2 * time^2 + distance^2 = 0
	// 24.01 * time^4 - 1600 * time^2 + distance^2 = 0
	// a = 24.01
	// b = -1600
	// c = distance^2
	// D = b^2 - 4 * a * c = 2560000 - 96.04 * distance^2
	// time^2 = (-b ± √D)/(2 * a) = (1600 ± √D) / 48.02
	//

	// calculate arrow flight time
	float discriminant = 2560000.0f - 96.04f * (distance * distance);
	if (discriminant >= 0.0f)
	{
		float sqrtD = std::sqrtf(discriminant);
		std::array<float, 2> tSqr = { (1600.0f - sqrtD) / 48.02f, (1600.0f + sqrtD) / 48.02f };
		std::optional<float> time;
		for (int i = 0; i < 2; i++)
		{
			if (tSqr[i] >= 0.0f)
			{
				if (time.has_value())
					time = std::min<float>(time.value(), sqrtf(tSqr[i]));
				else
					time = sqrtf(tSqr[i]);
			}
		}
		if (time.has_value())
		{
			// calculate derivative of arc based on flight time
			float speedVert = 4.9f * time.value();
			float speedHor = distance / time.value();
			// calculate arrow start direction
			return Velocity{ delta.x / distance * speedHor, delta.y / distance * speedHor, speedVert };
		}
	}

	// under the this task's conditions this should never happen, but i'll keep this just in case
	return std::nullopt;
}
