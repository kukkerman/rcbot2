#ifndef __RCBOT_PROFILE_H__
#define __RCBOT_PROFILE_H__

#include <vector>
#include <list>
#include <map>
#include <string>

class CBotProfile {
private:
	static constexpr int minVisionTicks = 1;
	static constexpr int maxVisionTicks = 100;

	static constexpr int minPathTicks = 1;
	static constexpr int maxPathTicks = 200;

	static constexpr int minVisionTicksClients = 1;
	static constexpr int maxVisionTicksClients = 32;

	static constexpr int minSensitivity = 1;
	static constexpr int maxSensitivity = 20;

	static constexpr float minBraveness = 0.0f;
	static constexpr float maxBraveness = 1.0f;

	static constexpr float minAimSkill = 0.0f;
	static constexpr float maxAimSkill = 1.0f;

public:
	CBotProfile();
	CBotProfile(CBotProfile &other);

	static CBotProfile genRandom(int teamId, int classId, const char *name);
	static CBotProfile parseFromFile(FILE *f);

	int getVisionTicks() const;
	
	int getPathTicks() const;

	int getVisionTicksClients() const;

	int getSensitivity() const;

	float getBraveness() const;

	float getAimSkill() const;

	// bot's name
	char *m_szName = DEFAULT_BOT_NAME;
	char *m_szModel = "default";
	// bot's team
	int m_iTeam = -1;					// preferred player team
	int m_iClass = -1;					// preferred player class

private:
	int m_iVisionTicks = 10;			// speed of finding non players (npcs/teleporters etc)
	int m_iPathTicks = 100;				// speed of finding a path
	int m_iVisionTicksClients = 2;		// speed of finding other players and enemy players
	int m_iSensitivity = 8;				// 1 to 20 sensitivity of bot's "mouse" (angle speed)
	float m_fBraveness = 0.5f;			// 0.0 to 1.0 sensitivity to danger (brave = less sensitive)
	float m_fAimSkill = 0.5f;			// 0.0 to 1.0 ability to predict players movements (aim skill)
};

class CBotProfiles
{
public:
	static void deleteProfiles ();

	// find profiles and setup list
	static void setupProfiles ();

	// return a profile unused by a bot
	static CBotProfile *getRandomFreeProfile (int classFilter = -1, int teamFilter = -1);

	//static CBotProfile *getDefaultProfile ();
	static CBotProfile *genRandomProfile(int teamId, int classId, const char *name);

private:
	static std::vector <CBotProfile*> m_Profiles;
	static std::list <CBotProfile> randomProfiles;
};

#endif