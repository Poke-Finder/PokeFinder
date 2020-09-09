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

#include "StationaryGenerator5.hpp"
#include <Core/Enum/Encounter.hpp>
#include <Core/Enum/Lead.hpp>
#include <Core/Enum/Method.hpp>
#include <Core/Parents/Filters/StateFilter.hpp>
#include <Core/RNG/LCRNG64.hpp>
#include <Core/RNG/MT.hpp>
#include <Core/RNG/RNGList.hpp>

StationaryGenerator5::StationaryGenerator5(u32 initialAdvances, u32 maxAdvances, u16 tid, u16 sid, u8 genderRatio, Method method,
                                           Encounter encounter, const StateFilter &filter) :
    StationaryGenerator(initialAdvances, maxAdvances, tid, sid, genderRatio, method, filter),
    idBit((tid & 1) ^ (sid & 1)),
    encounter(encounter)
{
}

QVector<StationaryState> StationaryGenerator5::generate(u64 seed) const
{
    switch (method)
    {
    case Method::Method5IVs:
        switch (encounter)
        {
        case Encounter::Roamer:
            return generateRoamerIVs(seed);
        default:
            return generateIVs(seed);
        }
    case Method::Method5CGear:
        switch (encounter)
        {
        case Encounter::Roamer:
            return generateRoamerCGear(seed);
        default:
            return generateCGear(seed);
        }
    // case Method::Method5:
    //    break;
    default:
        return QVector<StationaryState>();
    }
}

QVector<StationaryState> StationaryGenerator5::generateRoamerIVs(u64 seed) const
{
    QVector<StationaryState> states;

    MT mt(seed >> 32);
    mt.advance(initialAdvances + offset);

    RNGList<u8, MT, 8, 27> rngList(mt);

    for (u32 cnt = 0; cnt < maxAdvances; cnt++, rngList.advanceState())
    {
        StationaryState state;

        rngList.advance(1); // Blank ???
        u8 hp = rngList.getValue();
        u8 atk = rngList.getValue();
        u8 def = rngList.getValue();
        u8 spd = rngList.getValue();
        u8 spe = rngList.getValue();
        u8 spa = rngList.getValue();

        state.setIVs(hp, atk, def, spa, spd, spe);
        state.calculateHiddenPower();

        if (filter.compareIVs(state))
        {
            // state.setIVFrame(initialAdvances + cnt);
            states.append(state);
        }
    }

    return states;
}

QVector<StationaryState> StationaryGenerator5::generateIVs(u64 seed) const
{
    QVector<StationaryState> states;

    MT mt(seed >> 32);
    mt.advance(initialAdvances + offset);

    RNGList<u8, MT, 8, 27> rngList(mt);

    for (u32 cnt = 0; cnt < maxAdvances; cnt++, rngList.advanceState())
    {
        StationaryState state;

        u8 hp = rngList.getValue();
        u8 atk = rngList.getValue();
        u8 def = rngList.getValue();
        u8 spa = rngList.getValue();
        u8 spd = rngList.getValue();
        u8 spe = rngList.getValue();

        state.setIVs(hp, atk, def, spa, spd, spe);
        state.calculateHiddenPower();

        if (filter.compareIVs(state))
        {
            // state.setIVFrame(initialAdvances + cnt);
            states.append(state);
        }
    }

    return states;
}

QVector<StationaryState> StationaryGenerator5::generateRoamerCGear(u64 seed) const
{
    QVector<StationaryState> states;

    MT mt(seed >> 32);
    mt.advance(initialAdvances + offset);
    mt.advance(2); // Skip first two advances

    RNGList<u8, MT, 8, 27> rngList(mt);

    for (u32 cnt = 0; cnt < maxAdvances; cnt++, rngList.advanceState())
    {
        StationaryState state;

        rngList.advance(1); // Blank ???
        u8 hp = rngList.getValue();
        u8 atk = rngList.getValue();
        u8 def = rngList.getValue();
        u8 spd = rngList.getValue();
        u8 spe = rngList.getValue();
        u8 spa = rngList.getValue();

        state.setIVs(hp, atk, def, spa, spd, spe);
        state.calculateHiddenPower();

        if (filter.compareIVs(state))
        {
            // state.setIVFrame(initialAdvances + cnt);
            states.append(state);
        }
    }

    return states;
}

QVector<StationaryState> StationaryGenerator5::generateCGear(u64 seed) const
{
    QVector<StationaryState> states;

    MT mt(seed >> 32);
    mt.advance(initialAdvances + offset);
    mt.advance(2); // Skip first two advances

    RNGList<u8, MT, 8, 27> rngList(mt);

    for (u32 cnt = 0; cnt < maxAdvances; cnt++, rngList.advanceState())
    {
        StationaryState state;

        u8 hp = rngList.getValue();
        u8 atk = rngList.getValue();
        u8 def = rngList.getValue();
        u8 spa = rngList.getValue();
        u8 spd = rngList.getValue();
        u8 spe = rngList.getValue();

        state.setIVs(hp, atk, def, spa, spd, spe);
        state.calculateHiddenPower();

        if (filter.compareIVs(state))
        {
            // state.setIVFrame(initialAdvances + cnt);
            states.append(state);
        }
    }

    return states;
}

QVector<StationaryState> StationaryGenerator5::generateStationary(u64 seed) const
{
    QVector<StationaryState> states;

    BWRNG rng(seed);
    rng.advance(initialAdvances + offset);

    for (u32 cnt = 0; cnt < maxAdvances; cnt++)
    {
        StationaryState state(initialAdvances + cnt);
        BWRNG go(rng.getSeed());

        u32 pid=0;
        if (lead == Lead::Synchronize)
        {
            bool synch = (go.nextUInt() >> 1) == 1;
            pid = go.nextUInt() ^ 0x10000;

            if (synch)
            {
                state.setNature(synchNature);
            }
            else
            {
                state.setNature(static_cast<u8>(go.nextUInt(25)));
            }
        }
        else if (lead == Lead::CuteCharm)
        {
            bool charm = (go.nextUInt(0xffff) / 656) < 67;
            pid = go.nextUInt() ^ 0x10000;

            if (!charm)
            {
                state.setNature(static_cast<u8>(go.nextUInt(25)));
            }
            else
            {
                go.next(); // TODO modify pid
                state.setNature(static_cast<u8>(go.nextUInt(25)));
            }
        }
        else if (lead == Lead::CompoundEyes)
        {
            pid = go.nextUInt();
            state.setNature(static_cast<u8>(go.nextUInt(25)));
        }
        else if (lead == Lead::Search)
        {
            // TODO
        }
        else // No lead
        {
            go.advance(1);
            pid = go.nextUInt();
            state.setNature(static_cast<u8>(go.nextUInt(25)));
        }

        u8 val = idBit ^ (pid & 1) ^ (pid >> 31);
        if (val == 1)
        {
            pid ^= 0x80000000;
        }

        state.setPID(pid);
        state.setAbility((pid >> 16) & 1);
        state.setGender(pid & 255, genderRatio);
        state.setShiny(tsv, (pid >> 16) ^ (pid & 0xffff), 8);

        if (filter.comparePID(state))
        {
            states.append(state);
        }
    }

    return states;
}

QVector<StationaryState> StationaryGenerator5::generateRoamer(u64 seed)
{
    QVector<StationaryState> states;

    BWRNG rng(seed);
    rng.advance(initialAdvances + offset);

    for (u32 cnt = 0; cnt < maxAdvances; cnt++)
    {
        StationaryState state(initialAdvances + cnt);
        BWRNG go(rng.getSeed());

        u32 pid = go.nextUInt();
        state.setNature(static_cast<u8>(go.nextUInt(25)));

        state.setPID(pid);
        state.setAbility((pid >> 16) & 1);
        state.setGender(pid & 255, genderRatio);
        state.setShiny(tsv, (pid >> 16) ^ (pid & 0xffff), 8);

        if (filter.comparePID(state))
        {
            states.append(state);
        }
    }

    return states;
}

QVector<StationaryState> StationaryGenerator5::generateGift(u64 seed)
{
    QVector<StationaryState> states;

    BWRNG rng(seed);
    rng.advance(initialAdvances + offset);

    for (u32 cnt = 0; cnt < maxAdvances; cnt++)
    {
        StationaryState state(initialAdvances + cnt);
        BWRNG go(rng.getSeed());

        u32 pid = go.nextUInt() ^ 0x10000;
        state.setNature(static_cast<u8>(go.nextUInt(25)));

        state.setPID(pid);
        state.setAbility((pid >> 16) & 1);
        state.setGender(pid & 255, genderRatio);
        state.setShiny(tsv, (pid >> 16) ^ (pid & 0xffff), 8);

        if (filter.comparePID(state))
        {
            states.append(state);
        }
    }

    return states;
}

QVector<StationaryState> StationaryGenerator5::generateEntraLink(u64 seed)
{
    QVector<StationaryState> states;

    BWRNG rng(seed);
    rng.advance(initialAdvances + offset);

    for (u32 cnt = 0; cnt < maxAdvances; cnt++)
    {
        StationaryState state(initialAdvances + cnt);
        BWRNG go(rng.getSeed());

        // TODO
    }

    return states;
}

QVector<StationaryState> StationaryGenerator5::generateLarvestaEgg(u64 seed)
{
    QVector<StationaryState> states;

    BWRNG rng(seed);
    rng.advance(initialAdvances + offset);

    for (u32 cnt = 0; cnt < maxAdvances; cnt++)
    {
        StationaryState state(initialAdvances + cnt);
        BWRNG go(rng.getSeed());

        u32 pid = go.nextUInt();
        go.advance(1);
        state.setNature(static_cast<u8>(go.nextUInt(25)));

        state.setPID(pid);
        state.setAbility((pid >> 16) & 1);
        state.setGender(pid & 255, genderRatio);
        state.setShiny(tsv, (pid >> 16) ^ (pid & 0xffff), 8);

        if (filter.comparePID(state))
        {
            states.append(state);
        }
    }

    return states;
}

QVector<StationaryState> StationaryGenerator5::generateHiddenGrotto(u64 seed)
{
    QVector<StationaryState> states;

    BWRNG rng(seed);
    rng.advance(initialAdvances + offset);

    for (u32 cnt = 0; cnt < maxAdvances; cnt++)
    {
        StationaryState state(initialAdvances + cnt);
        BWRNG go(rng.getSeed());

        // TODO
    }

    return states;
}
