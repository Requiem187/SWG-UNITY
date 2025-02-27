/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef CREATECREATURECOMMAND_H_
#define CREATECREATURECOMMAND_H_

#include "server/zone/objects/creature/ai/AiAgent.h"
#include "server/zone/Zone.h"
#include "server/zone/managers/creature/CreatureManager.h"
#include "server/zone/managers/creature/AiMap.h"
#include "server/zone/objects/player/sui/messagebox/SuiMessageBox.h"
#include "server/zone/managers/space/SpaceAiMap.h"
#include "server/zone/objects/ship/ai/ShipAiAgent.h"
#include "server/zone/managers/ship/ShipManager.h"

class CreateCreatureCommand : public QueueCommand {
public:

	CreateCreatureCommand(const String& name, ZoneProcessServer* server) : QueueCommand(name, server) {
	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		Zone* zone = creature->getZone();

		if (zone == nullptr)
			return GENERALERROR;

		float posX = creature->getPositionX(), posY = creature->getPositionY(), posZ = creature->getPositionZ();
		uint64 parID = creature->getParentID();

		String objName = "", tempName = "object/mobile/boba_fett.iff";
		bool baby = false;
		bool event = false;
		int level = -1;
		float scale = -1.0;

		if (!arguments.isEmpty()) {
			UnicodeTokenizer tokenizer(arguments);
			tokenizer.setDelimeter(" ");

			if (tokenizer.hasMoreTokens())
				tokenizer.getStringToken(tempName);

			if (!tempName.isEmpty() && tempName.toLowerCase() == "checkthreads") {
				ManagedReference<SuiMessageBox*> box = new SuiMessageBox(creature, SuiWindowType::NONE);

				if (box != nullptr) {
					box->setPromptTitle("CreateCreature - Check Threads");

					StringBuffer msg;

					msg << "Ground Zone AI:\n\n";
					msg << "Active AiBehaviorEvents: " << AiMap::instance()->activeBehaviorEvents.get() << "\n";
					msg << "AiAgent Exceptions: " << AiMap::instance()->countExceptions.get() << "\n";
					msg << "Scheduled AiBehaviorEvents: " << AiMap::instance()->scheduledBehaviorEvents.get() << "\n";
					msg << "AiBehaviorEvents with followObject: " << AiMap::instance()->behaviorsWithFollowObject.get() << "\n";
					msg << "AiBehaviorEvents retreating: " << AiMap::instance()->behaviorsRetreating.get() << "\n";
					msg << "AiRecoveryEvents: " << AiMap::instance()->activeRecoveryEvents.get() << "\n\n\n";

					msg << "Space Zone AI:\n\n";
					msg << "Active AiBehaviorEvents: " << SpaceAiMap::instance()->activeBehaviorEvents.get() << "\n";
					msg << "AiAgent Exceptions: " << SpaceAiMap::instance()->countExceptions.get() << "\n";
					msg << "Scheduled AiBehaviorEvents: " << SpaceAiMap::instance()->scheduledBehaviorEvents.get() << "\n";
					msg << "AiBehaviorEvents with followObject: " << SpaceAiMap::instance()->behaviorsWithFollowObject.get() << "\n";
					msg << "AiBehaviorEvents retreating: " << SpaceAiMap::instance()->behaviorsRetreating.get() << "\n";
					msg << "AiRecoveryEvents: " << SpaceAiMap::instance()->activeRecoveryEvents.get() << "\n";

					box->setPromptText(msg.toString());

					creature->sendMessage(box->generateMessage());
				}


				ZoneServer* server = creature->getZoneServer();

				int totalSpawned = 0;

				for (int i = 0; i < server->getZoneCount(); ++i) {
					Zone* zone = server->getZone(i);

					if (zone == nullptr)
						continue;

					int num = zone->getSpawnedAiAgents();

					totalSpawned += num;

					StringBuffer message;
					message << "Current number of AiAgents in " << zone->getZoneName() << ": " << num;
					creature->sendSystemMessage(message.toString());
				}

				for (int j = 0; j < server->getSpaceZoneCount(); ++j) {
					SpaceZone* zone = server->getSpaceZone(j);

					if (zone == nullptr)
						continue;

					int num = zone->getSpawnedAiAgents();

					totalSpawned += num;

					StringBuffer message;
					message << "Current number of ShipAiAgents in " << zone->getZoneName() << ": " << num;
					creature->sendSystemMessage(message.toString());
				}

				StringBuffer msg2;
				msg2 << "Total number of AiAgents spawned in galaxy: " << totalSpawned;
				creature->sendSystemMessage(msg2.toString());

				return SUCCESS;
			}

			if (!tempName.isEmpty() && zone->isSpaceZone()) {
				return createShip(creature, target, arguments);
			}

			if (tokenizer.hasMoreTokens())
				tokenizer.getStringToken(objName);

			if (!objName.isEmpty() && objName == "baby")
				baby = true;

			if (!objName.isEmpty() && objName == "event") {
				event = true;

				if (tokenizer.hasMoreTokens()) {
					String levelToken;

					tokenizer.getStringToken(levelToken);

					int idx = levelToken.indexOf("-");

					if (idx == -1) {
						level = Integer::valueOf(levelToken);
					} else {
						int minLevel = Integer::valueOf(levelToken.subString(0, idx));
						int maxLevel = Integer::valueOf(levelToken.subString(idx + 1));

						level = minLevel + System::random(maxLevel - minLevel);
					}

					if (level > 500)
						level = 500;

					if (tokenizer.hasMoreTokens()) {
						scale = tokenizer.getFloatToken();
					}
				}
			}

			if (!objName.isEmpty() && objName.indexOf("object") == -1 && !baby && !event) {
				if (objName.length() < 6)
					posX = Float::valueOf(objName);

				objName = "";
			} else if (tokenizer.hasMoreTokens()) {
					posX = tokenizer.getFloatToken();
			}

			if (tokenizer.hasMoreTokens())
				posZ = tokenizer.getFloatToken();

			if (tokenizer.hasMoreTokens())
				posY = tokenizer.getFloatToken();

			if (tokenizer.hasMoreTokens()) {
				int planet = tokenizer.getIntToken();
				zone = creature->getZoneServer()->getZone(planet);
			}

			if (tokenizer.hasMoreTokens())
				parID = tokenizer.getLongToken();
		} else {
			StringBuffer usage;

			if (zone != nullptr && zone->isSpaceZone()) {
				usage << "CreateCreatureCommand syntax: /createCreature <template> <faction>";
			} else {
				usage << "Usage: /createCreature <template> [object template | ai template | baby | event [level] [scale] ] [X] [Z] [Y] [planet] [cellID]" << endl << "/createCreature checkthreads";
			}

			creature->sendSystemMessage(usage.toString());
			return GENERALERROR;
		}

		CreatureManager* creatureManager = zone->getCreatureManager();

		uint32 templ = tempName.hashCode();
		uint32 objTempl = objName.length() > 0 ? objName.hashCode() : 0;

		AiAgent* npc = nullptr;
		if (baby)
			npc = cast<AiAgent*>(creatureManager->spawnCreatureAsBaby(templ, posX, posZ, posY, parID));
		else if (event)
			npc = cast<AiAgent*>(creatureManager->spawnCreatureAsEventMob(templ, level, posX, posZ, posY, parID));
		else if (tempName.indexOf(".iff") != -1)
			npc = cast<AiAgent*>(creatureManager->spawnCreatureWithAi(templ, posX, posZ, posY, parID));
		else {
			npc = cast<AiAgent*>(creatureManager->spawnCreature(templ, objTempl, posX, posZ, posY, parID));
			if (npc != nullptr) {
				npc->setAITemplate();

				//Locker _nlocker(npc);
				//npc->setAIDebug(true);
			}
		}

		if (baby && npc == nullptr) {
			creature->sendSystemMessage("You cannot spawn " + tempName + " as a baby.");
			return GENERALERROR;
		} else if (npc == nullptr) {
			creature->sendSystemMessage("could not spawn " + arguments.toString());
			return GENERALERROR;
		}

		npc->updateDirection(Math::deg2rad(creature->getDirectionAngle()));

		if (scale > 0 && scale != 1.0) {
			Locker nlocker(npc);
			npc->setHeight(scale);
		}

		info(true) << "CreateCreatureCommand " << creature->getDisplayedName() << " created: " << npc->getObjectID() << " [" << npc->getDisplayedName() << "] at " << npc->getWorldPosition() << " on " << npc->getZone()->getZoneName();

		return SUCCESS;
	}

	int createShip(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		if (creature == nullptr || arguments == "") {
			return INVALIDPARAMETERS;
		}

		auto spaceZone = creature->getZone();

		if (spaceZone == nullptr || !spaceZone->isSpaceZone()) {
			return GENERALERROR;
		}

		UnicodeTokenizer tokens(arguments);

		String shipName = "";
		String faction = "";

		if (tokens.hasMoreTokens()) {
			tokens.getStringToken(shipName);
		}

		if (tokens.hasMoreTokens()) {
			tokens.getStringToken(faction);
		}

		ManagedReference<ShipAiAgent*> shipAgent = ShipManager::instance()->createAiShip(shipName);

		if (shipAgent == nullptr) {
			creature->sendSystemMessage("CreateCreatureCommand error: invalid ship agent template " + shipName);
			return GENERALERROR;
		}

		Locker sLock(shipAgent, creature);

		if (!faction.isEmpty() && faction != shipAgent->getShipFactionString()) {
			shipAgent->setShipFactionString(faction, false);
		}

		shipAgent->setFactionStatus(FactionStatus::OVERT);

		Vector3 position = creature->getWorldPosition();

		position.setX(Math::clamp(-7999.f, (System::random(128) - 64.f) + position.getX(), 7999.f));
		position.setY(Math::clamp(-7999.f, (System::random(128) - 64.f) + position.getY(), 7999.f));
		position.setZ(Math::clamp(-7999.f, (System::random(128) - 64.f) + position.getZ(), 7999.f));

		shipAgent->initializePosition(position.getX(), position.getZ(), position.getY());

		shipAgent->setHomeLocation(position.getX(), position.getZ(), position.getY(), Quaternion::IDENTITY);
		shipAgent->initializeTransform(position, Quaternion::IDENTITY);

		shipAgent->setHyperspacing(true);

		if (!spaceZone->transferObject(shipAgent, -1, true)) {
			shipAgent->destroyObjectFromWorld(true);
			shipAgent->destroyObjectFromDatabase(true);

			return GENERALERROR;
		}

		shipAgent->setHyperspacing(false);

		info(true) << "CreateCreatureCommand " << creature->getDisplayedName() << " Created Ship: " << shipAgent->getObjectID() << " [" << shipAgent->getDisplayedName() << "] at " << shipAgent->getWorldPosition() << " in Space Zone: " << spaceZone->getZoneName();

		return SUCCESS;
	}
};

#endif //CREATECREATURECOMMAND_H_
