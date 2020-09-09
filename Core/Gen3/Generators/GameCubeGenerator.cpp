/*
 * This file is part of PokéFinder
 * Copyright (C) 2017-2020 by Admiral_Fish, bumba, and EzPzStreamz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "GameCubeGenerator.hpp"
#include <Core/Enum/Method.hpp>
#include <Core/Enum/ShadowType.hpp>
#include <Core/Parents/Filters/StateFilter.hpp>

GameCubeGenerator::GameCubeGenerator(u32 initialAdvances, u32 maxAdvances, u16 tid, u16 sid, u8 genderRatio, Method method,
                                     const StateFilter &filter) :
    Generator(initialAdvances, maxAdvances, tid, sid, genderRatio, method, filter)
{
}

QVector<GameCubeState> GameCubeGenerator::generate(u32 seed) const
{
    switch (method)
    {
    case Method::XDColo:
        return generateXDColo(seed);
    case Method::XD:
        return generateXDShadow(seed);
    case Method::Colo:
        return generateColoShadow(seed);
    case Method::Channel:
        return generateChannel(seed);
    default:
        return QVector<GameCubeState>();
    }
}

void GameCubeGenerator::setShadowTeam(u8 index, u8 type)
{
    team = ShadowTeam::loadShadowTeams(method).at(index);
    this->type = type;
}

QVector<GameCubeState> GameCubeGenerator::generateXDColo(u32 seed) const
{
    QVector<GameCubeState> states;

    XDRNG rng(seed);
    rng.advance(initialAdvances + offset);

    // Method XD/Colo [SEED] [IVS] [IVS] [BLANK] [PID] [PID]

    for (u32 cnt = 0; cnt < maxAdvances; cnt++, rng.next())
    {
        GameCubeState state(initialAdvances + cnt);
        XDRNG go(rng.getSeed());

        u16 iv1 = go.nextUShort();
        u16 iv2 = go.nextUShort();
        u8 ability = go.nextUShort() & 1;
        u16 high = go.nextUShort();
        u16 low = go.nextUShort();

        state.setPID(high, low);
        state.setAbility(ability);
        state.setGender(low & 255, genderRatio);
        state.setNature(state.getPID() % 25);
        state.setShiny(tsv, high ^ low, 8);

        state.setIVs(iv1, iv2);
        state.calculateHiddenPower();

        if (filter.compareState(state))
        {
            states.append(state);
        }
    }

    return states;
}

QVector<GameCubeState> GameCubeGenerator::generateXDShadow(u32 seed) const
{
    QVector<GameCubeState> states;

    XDRNG rng(seed);
    rng.advance(initialAdvances + offset);

    QVector<LockInfo> locks = team.getLocks();

    for (u32 cnt = 0; cnt < maxAdvances; cnt++, rng.next())
    {
        GameCubeState state(initialAdvances + cnt);

        XDRNG go(rng.getSeed());

        // Enemy TID/SID
        go.advance(2);

        for (auto lock = locks.rbegin(); lock != locks.rend(); lock++)
        {
            // Temporary PID: 2 advances
            // IVs: 2 advances
            // Ability: 1 state
            go.advance(5);

            // If we are looking at a shadow pokemon
            // We will assume it is already set and skip the PID process
            if (!lock->getFree())
            {
                u32 pid=0;
                do
                {
                    u16 high = go.nextUShort();
                    u16 low = go.nextUShort();
                    pid = (high << 16) | low;

                    if ((high ^ low ^ tsv) < 8) // Shiny lock is from TSV of savefile
                    {
                        continue;
                    }
                } while (!lock->compare(pid));
            }
        }

        if (team.getType() == ShadowType::SecondShadow || team.getType() == ShadowType::Salamence)
        {
            go.advance(5); // Set and Unset start the same

            if (type == 1) // Check for shiny lock with unset
            {
                u16 psv = go.nextUShort() ^ go.nextUShort();
                while ((psv ^ tsv) < 8)
                {
                    psv = go.nextUShort() ^ go.nextUShort();
                }
            }
        }

        go.advance(2); // Fake PID

        u16 iv1 = go.nextUShort();
        u16 iv2 = go.nextUShort();
        state.setIVs(iv1, iv2);
        state.calculateHiddenPower();

        state.setAbility(go.nextUShort() & 1);

        u16 high = go.nextUShort();
        u16 low = go.nextUShort();
        while ((high ^ low ^ tsv) < 8) // Shiny lock is from TSV of savefile
        {
            high = go.nextUShort();
            low = go.nextUShort();
        }

        state.setPID(high, low);
        state.setGender(low & 255, genderRatio);
        state.setNature(state.getPID() % 25);
        state.setShiny(0);

        if (filter.compareState(state))
        {
            states.append(state);
        }
    }

    return states;
}

QVector<GameCubeState> GameCubeGenerator::generateColoShadow(u32 seed) const
{
    QVector<GameCubeState> states;

    XDRNG rng(seed);
    rng.advance(initialAdvances + offset);

    QVector<LockInfo> locks = team.getLocks();

    for (u32 cnt = 0; cnt < maxAdvances; cnt++, rng.next())
    {
        GameCubeState state(initialAdvances + cnt);

        XDRNG go(rng.getSeed());

        // Trainer TID/SID
        u16 trainerTSV = go.nextUShort() ^ go.nextUShort();

        u8 ability=0;
        u32 pid=0;
        for (auto lock = locks.rbegin(); lock != locks.rend(); lock++)
        {
            // Temporary PID: 2 advances
            // IVs: 2 advances
            // Ability: 1 state
            go.advance(4);
            ability = go.nextUShort() & 1;

            do
            {
                u16 high = go.nextUShort();
                u16 low = go.nextUShort();
                pid = (high << 16) | low;

                if ((high ^ low ^ trainerTSV) < 8) // Shiny lock is from enemy TSV
                {
                    continue;
                }
            } while (!lock->compare(pid));
        }

        // E-Reader is included as part of the above loop
        // Set the PID and ability that was already computed
        // IVs are 0
        if (team.getType() == ShadowType::EReader)
        {
            state.setIVs(0);
            state.calculateHiddenPower();

            state.setPID(pid);
            state.setAbility(ability);
            state.setGender(pid & 255, genderRatio);
            state.setNature(pid % 25);
            state.setShiny(tsv, (pid >> 16) ^ (pid & 0xffff), 8);
        }
        else
        {
            go.advance(2); // Fake PID

            u16 iv1 = go.nextUShort();
            u16 iv2 = go.nextUShort();
            state.setIVs(iv1, iv2);
            state.calculateHiddenPower();

            state.setAbility(go.nextUShort() & 1);

            u16 high = go.nextUShort();
            u16 low = go.nextUShort();
            while ((high ^ low ^ trainerTSV) < 8) // Shiny lock is from enemy TSV
            {
                high = go.nextUShort();
                low = go.nextUShort();
            }

            state.setPID(high, low);
            state.setGender(low & 255, genderRatio);
            state.setNature(state.getPID() % 25);
            state.setShiny(tsv, high ^ low, 8);
        }

        if (filter.compareState(state))
        {
            states.append(state);
        }
    }

    return states;
}

QVector<GameCubeState> GameCubeGenerator::generateChannel(u32 seed) const
{
    QVector<GameCubeState> states;

    XDRNG rng(seed);
    rng.advance(initialAdvances + offset);

    // Method Channel [SEED] [SID] [PID] [PID] [BERRY] [GAME ORIGIN] [OT GENDER] [IV] [IV] [IV] [IV] [IV] [IV]

    for (u32 cnt = 0; cnt < maxAdvances; cnt++, rng.next())
    {
        GameCubeState state(initialAdvances + cnt);
        XDRNG go(rng.getSeed());

        // u16 tid = 40122;
        u16 sid = go.nextUShort();
        u16 high = go.nextUShort();
        u16 low = go.nextUShort();
        go.advance(3);

        // u16 berry = go.nextUShort() >> 13; If >= 4 salac, else ganlon
        // u16 game = go.nextUShort() >> 12; If >= 8 ruby, else sapphire
        // u16 gender = go.nextUShort() >> 11; If >= 16 female, else male

        if ((low > 7 ? 0 : 1) != (high ^ 40122 ^ sid))
        {
            high ^= 0x8000;
        }

        state.setPID(high, low);
        state.setAbility(low & 1);
        state.setGender(low & 255, genderRatio);
        state.setNature(state.getPID() % 25);
        state.setShiny(40122 ^ sid, high ^ low, 8);

        u8 hp = go.nextUShort() >> 11;
        u8 atk = go.nextUShort() >> 11;
        u8 def = go.nextUShort() >> 11;
        u8 spe = go.nextUShort() >> 11;
        u8 spa = go.nextUShort() >> 11;
        u8 spd = go.nextUShort() >> 11;

        state.setIVs(hp, atk, def, spa, spd, spe);
        state.calculateHiddenPower();

        if (filter.compareState(state))
        {
            states.append(state);
        }
    }

    return states;
}
