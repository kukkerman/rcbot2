/*
 *    This file is part of RCBot.
 *
 *    RCBot by Paul Murphy adapted from Botman's HPB Bot 2 template.
 *
 *    RCBot is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    RCBot is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RCBot; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */
#include "bot.h"
#include "bot_strings.h"
#include "bot_globals.h"
#include "bot_profile.h"
#include "bot_genclass.h"
#include "bot_visibles.h"
#include "bot_navigator.h"
#include "bot_kv.h"

vector <CBotProfile*> CBotProfiles :: m_Profiles;
list <CBotProfile> CBotProfiles::randomProfiles;

extern ConVar bot_general_difficulty;

inline float fclamp(float v, float min, float max) {
	if (v < min) {
		v = min;
	} else if (v > max) {
		v = max;
	}

	return v;
}

CBotProfile::CBotProfile() { }

CBotProfile :: CBotProfile ( CBotProfile &other )
{
	*this = other;

	m_szName = CStrings::getString(other.m_szName);
	m_szModel = CStrings::getString(other.m_szModel);
}

CBotProfile CBotProfile::genRandom(int teamId, int classId, const char *name) {
	CBotProfile profile;

	const auto mod = CBotGlobals::getCurrentMod();

	if (teamId == -1) {
		teamId = randomInt(mod->getMinTeamId(), mod->getMaxTeeamId());
	}

	if (classId == -1) {
		classId = randomInt(mod->getMinClassId(), mod->getMaxClassId());
	}

	const char *botName = DEFAULT_BOT_NAME;
	if (name != nullptr && name[0] != 0) {
		botName = name;

	} else {
		const auto &names = mod->getBotNamesForTeamId(teamId);
		if (!names.empty()) {
			botName = names[randomInt(0, names.size() - 1)];
		}
	}

	profile.m_szName = CStrings::getString(botName);
	profile.m_iTeam = teamId;
	profile.m_iClass = classId;
	profile.m_iVisionTicks = randomNormalFloat(CBotProfile::minVisionTicks, CBotProfile::maxVisionTicks);
	profile.m_iPathTicks = randomNormalFloat(CBotProfile::minPathTicks, CBotProfile::maxPathTicks);
	profile.m_iVisionTicksClients = randomNormalFloat(CBotProfile::minVisionTicksClients, CBotProfile::maxVisionTicksClients);
	profile.m_iSensitivity = randomNormalFloat(CBotProfile::minSensitivity, CBotProfile::maxSensitivity);
	profile.m_fBraveness = randomNormalFloat(CBotProfile::minBraveness, CBotProfile::maxBraveness);
	profile.m_fAimSkill = randomNormalFloat(CBotProfile::minAimSkill, CBotProfile::maxAimSkill);

	return profile;
}

CBotProfile CBotProfile::parseFromFile(FILE *f) {
	CBotProfile profile;
	
	CRCBotKeyValueList kv;
	kv.parseFile(f);

	kv.getInt("team", &profile.m_iTeam);
	kv.getInt("class", &profile.m_iClass);
	kv.getString("model", &profile.m_szModel);
	kv.getString("name", &profile.m_szName);
	kv.getInt("visionticks", &profile.m_iVisionTicks);
	kv.getInt("visionticksclients", &profile.m_iVisionTicksClients);
	kv.getInt("pathticks", &profile.m_iPathTicks);
	kv.getInt("sensitivity", &profile.m_iSensitivity);
	kv.getFloat("aim_skill", &profile.m_fAimSkill);
	kv.getFloat("braveness", &profile.m_fBraveness);

	return profile;
}

int CBotProfile::getVisionTicks() const {
	return fclamp(m_iVisionTicks * bot_general_difficulty.GetFloat(), minVisionTicks, maxVisionTicks);
}

int CBotProfile::getPathTicks() const {
	return fclamp(m_iPathTicks * bot_general_difficulty.GetFloat(), minPathTicks, maxPathTicks);
}

int CBotProfile::getVisionTicksClients() const {
	return fclamp(m_iVisionTicksClients * bot_general_difficulty.GetFloat(), minVisionTicksClients, maxVisionTicksClients);
}

int CBotProfile::getSensitivity() const {
	return fclamp(m_iSensitivity * bot_general_difficulty.GetFloat(), minSensitivity, maxSensitivity);
}

float CBotProfile::getBraveness() const {
	return fclamp(m_fBraveness * bot_general_difficulty.GetFloat(), minBraveness, maxBraveness);
}

float CBotProfile::getAimSkill() const {
	return fclamp(m_fAimSkill * bot_general_difficulty.GetFloat(), minAimSkill, maxBraveness);
}

void CBotProfiles :: deleteProfiles ()
{
	for ( unsigned int i = 0; i < m_Profiles.size(); i ++ )
	{
		delete m_Profiles[i];
		m_Profiles[i] = NULL;
	}

	m_Profiles.clear();
}

// find profiles and setup list
void CBotProfiles :: setupProfiles ()
{
	unsigned int iId;
	bool bDone;
	char szId[4];
	char filename[512];

	extern ConVar bot_anglespeed;

		// read profiles
	iId = 1;
	bDone = false;

	while ( (iId < 999) && (!bDone) )
	{
		sprintf(szId,"%d",iId);
		CBotGlobals::buildFileName(filename,szId,BOT_PROFILE_FOLDER,BOT_CONFIG_EXTENSION);

		FILE *fp = CBotGlobals::openFile(filename,"r");

		if ( fp )
		{
			CBotGlobals::botMessage(NULL, 0, "Reading bot profile \"%s\"", filename);

			CBotProfile profile = CBotProfile::parseFromFile(fp);
			fclose(fp);

			m_Profiles.push_back(new CBotProfile(profile));
		}
		else
		{
			bDone = true;
			CBotGlobals::botMessage(NULL,0,"Bot profile \"%s\" not found",filename);
		}

		iId ++;
	}

}

CBotProfile* CBotProfiles::genRandomProfile(int teamId, int classId, const char *name) {
	randomProfiles.emplace_back(CBotProfile::genRandom(teamId, classId, name));
	return &randomProfiles.back();
}

// return a profile unused by a bot
CBotProfile *CBotProfiles :: getRandomFreeProfile (int classFilter, int teamFilter)
{
	unsigned int i;
	dataUnconstArray<int> iList;
	CBotProfile *found = NULL;

	for ( i = 0; i < m_Profiles.size(); i ++ )
	{
		if ( !CBots::findBotByProfile(m_Profiles[i]) && 
			 (classFilter == -1 || m_Profiles[i]->m_iClass == -1 || classFilter == m_Profiles[i]->m_iClass) &&
			 (teamFilter  == -1 || m_Profiles[i]->m_iTeam  == -1 || teamFilter  == m_Profiles[i]->m_iTeam) )
			iList.Add(i);
	}

	if ( iList.IsEmpty() )
		return NULL;
	
	found = m_Profiles[iList.Random()];
	iList.Clear();

	return found;
}
