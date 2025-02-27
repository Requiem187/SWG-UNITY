recovery_overseer_shuttle = ShipAgent:new {
	template = "lambdashuttle_tier3",
	pilotTemplate = "bomber_tier1",
	shipType = "bomber",

	experience = 3200,

	lootChance = 0.375,
	lootRolls = 1,
	lootTable = "space_imperial_tier1",

	minCredits = 58,
	maxCredits = 736,

	aggressive = 0,

	spaceFaction = "imperial",
	alliedFactions = {"imperial", "merchant", "civilian"},
	enemyFactions = {"rebel", "hutt", "pirate", "nym"},
	imperialFactionReward = -106,
	rebelFactionReward = 53,
	formationLocation = 1,
	appearance = "imperial_officer",
	tauntAttackChance = 0.1,
	tauntDefendChance = 0.05,
	tauntDieChance = 0.1,

	pvpBitmask = ATTACKABLE,
	shipBitmask = NONE,
	optionsBitmask = AIENABLED,

	customShipAiMap = "",

	conversationTemplate = "",
	conversationMobile = "",
	conversationMessage = "", --Too Far Message
}

ShipAgentTemplates:addShipAgentTemplate(recovery_overseer_shuttle, "recovery_overseer_shuttle")
