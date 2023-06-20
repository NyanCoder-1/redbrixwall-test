#pragma once

#include "entt/entt.hpp"
#include <array>
#include <optional>

// components definitions
struct Position {
	float x;
	float y;
	float z;
};
struct Velocity {
	float dX;
	float dY;
	float dZ;
};
struct Spawner {
	uint32_t archersLeft;
	uint32_t teamId;
	float cooldown;
};
namespace Tags {
	struct Team1 {};
	struct Team2 {};
	struct Archer {};
	struct Arrow {};
	struct Gravity {};
}
struct Archer {
	float shootCooldown;
};
struct ColliderCylinder
{
	float height;
	float radius;
};
typedef uint8_t Health;
typedef uint32_t Team;

// helper functtions declaration
bool Overlaps(ColliderCylinder collider, Position entity, Position point);
float Distance(Position point1, Position point2);
float Length(Position vec);
std::optional<Velocity> Aim(Position me, Position target);

// constants declaration
extern const std::array<std::array<float, 4>, 2> teamsColors;
extern const uint32_t archersCount;
extern const Position team1Position;
extern const Position team2Position;
extern const float archerHeight;
extern const float archerRadius;
extern const float walkSpeed;
extern const float arrowStartSpeed;
extern const float arrowStartElevation;
extern const float arrowStartDistance;
extern const float shootCooldown;
extern const float spawnerCooldown;
extern const float firingRange;
extern const Velocity gravity;
extern const Health archerFullHealth;

// rules
extern const bool friendlyFire;
extern const bool waitForSpawnWholeTeam;

class Logic
{
public:
	Logic();

	void Update();

	entt::registry& GetRegistry(); // For adding and iterating through components that are not needed for simulation but needed in visualization

private:
	void updateSpawners(float dTime);
	void updateBehaviour(float dTime);
	void updatePhysics(float dTime);
	void applyGravity(float dTime);
	void updateMovement(float dTime);
	void solveCollision();
	void keepInField();
	void checkArrowOverlapping();
	bool spawnersDone();

	enum class eSimulationState {
		Prepare,
		Play
	};

	eSimulationState simulationState;
	entt::registry entities;
};