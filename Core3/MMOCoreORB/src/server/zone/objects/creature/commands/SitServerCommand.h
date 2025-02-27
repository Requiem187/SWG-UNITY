/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef SITSERVERCOMMAND_H_
#define SITSERVERCOMMAND_H_

#include "server/zone/objects/creature/commands/StandCommand.h"

class SitServerCommand : public QueueCommand {
public:
	const static int MINDELTA = 1600;

	SitServerCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (creature->isInCombat())
			return INVALIDSTATE;

		if (arguments.isEmpty()) {
			if (creature->isPlayerCreature()) {
				return setPlayerPosture(creature);
			}

			creature->setPosture(CreaturePosture::SITTING);
		} else {
			StringTokenizer tokenizer(arguments.toString());
			tokenizer.setDelimeter(",");
			float x = tokenizer.getFloatToken();
			float y = tokenizer.getFloatToken();
			float z = tokenizer.getFloatToken();

			uint64 coID = 0;
			if (tokenizer.hasMoreTokens())
				coID = tokenizer.getLongToken();

			if (x < -8192 || x > 8192)
				x = 0;
			if (y < -8192 || y > 8192)
				y = 0;
			if (z < -8192 || z > 8192)
				z = 0;

			auto cellParent = creature->getParent().get().castTo<CellObject*>();

			WorldCoordinates position(Vector3(x, z, y), cellParent);
			WorldCoordinates current(creature);

			float distance = position.getWorldPosition().squaredDistanceTo(current.getWorldPosition());
			if (distance > 5) {
				return TOOFAR;
			}

			if (cellParent != nullptr) {
				Reference<Vector<float> *> collisionPoints = CollisionManager::getCellFloorCollision(x, z, cellParent);

				if (collisionPoints == nullptr) {
					return TOOFAR;
				}
			}

			auto ghost = creature->getPlayerObject();

			if (ghost != nullptr && ghost->isTeleporting()) {
				return GENERALERROR;
			}

			creature->teleport(position.getX(), position.getZ(), position.getY(), creature->getParentID());

			creature->setState(CreatureState::SITTINGONCHAIR);
		}

		return SUCCESS;
	}

	int setPlayerPosture(CreatureObject* creature) const {
		const String& commandName = getQueueCommandName();

		if (creature->getQueueCommandDeltaTime("setPosture") < StandCommand::MINDELTA) {
			return GENERALERROR;
		}

		if (creature->getQueueCommandDeltaTime(commandName) < SitServerCommand::MINDELTA) {
			return GENERALERROR;
		}

		creature->setQueueCommandDeltaTime(commandName, "setPosture");
		creature->setPosture(CreaturePosture::SITTING);
		return SUCCESS;
	}
};

#endif //SITSERVERCOMMAND_H_

