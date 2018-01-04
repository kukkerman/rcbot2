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
#ifndef __RCBOT_EHANDLE_H__
#define __RCBOT_EHANDLE_H__

////// entity handling in network
class MyEHandle {
public:
    MyEHandle() :
        m_pEnt(nullptr),
        m_iSerialNumber(0) { }

    MyEHandle(edict_t *pent) :
        m_pEnt(pent),
        m_iSerialNumber(pent != nullptr ? pent->m_NetworkSerialNumber : 0) { }

    inline bool notValid() const { return get() == nullptr; }
    inline bool isValid() const { return get() != nullptr; }

    inline edict_t *get() const {
        if (m_iSerialNumber != 0 && m_pEnt != nullptr) {
            if (!m_pEnt->IsFree() && m_iSerialNumber == m_pEnt->m_NetworkSerialNumber) {
                return m_pEnt;
            }

        } else if (m_pEnt) {
            m_pEnt = nullptr;
        }

        return nullptr;
    }

    inline edict_t *get_old() const {
        return m_pEnt;
    }

    inline operator edict_t* const () {
        return get();
    }

    inline operator bool() const {
        return get() != nullptr;
    }

    inline bool operator == (edict_t *pent) const {
        return get() == pent;
    }

    inline bool operator != (edict_t *pent) const {
        return get() != pent;
    }

    inline bool operator == (const MyEHandle &other) const {
        return get() == other.get();
    }

    inline bool operator != (const MyEHandle &other) const {
        return get() != other.get();
    }

    inline MyEHandle& operator = (edict_t *pent) {
        m_pEnt = pent;

        if (pent) {
            m_iSerialNumber = pent->m_NetworkSerialNumber;

        } else {
            m_iSerialNumber = 0;
        }

        return *this;
    }

private:
	int m_iSerialNumber;
	mutable edict_t *m_pEnt;
};

#endif