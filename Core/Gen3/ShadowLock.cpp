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

#include "ShadowLock.hpp"

/* Each non-shadow before a shadow has to match
 * a specific gender/nature and these preset
 * values directly impact what spreads are available */

inline bool isShiny(u32 pid, u16 tsv)
{
    return ((pid >> 16) ^ (pid & 0xffff) ^ tsv) < 8;
}

inline u32 getPIDBackward(XDRNGR &rng)
{
    u32 pid = rng.nextUShort();
    pid |= rng.next() & 0xffff0000;
    return pid;
}

inline u32 getPIDForward(XDRNG &rng)
{
    u32 pid = rng.next() & 0xffff0000;
    pid |= rng.nextUShort();
    return pid;
}

ShadowLock::ShadowLock(u8 num, Method version)
{
    switchLock(num, version);
}

ShadowType ShadowLock::getType()
{
    return type;
}

bool ShadowLock::singleNL(u32 seed, u16 tsv)
{
    XDRNGR backward(seed);
    backward.advance(1);

    // Build PID of non-shadow
    u32 pid = getPIDBackward(backward);

    // Backwards nature lock check
    return locks.first().compare(pid) && !isShiny(pid, tsv);
}

// Working backwards it is hard to know what PID would get rerolled from shiny lock
// Only check shiny lock working forwards which should be good enough
bool ShadowLock::firstShadowNormal(u32 seed, u16 tsv)
{
    XDRNGR backward(seed);
    backward.advance(1);

    // Grab PID from first non-shadow going backwards
    // If it doesn't match spread fails
    u32 pidOriginal = getPIDBackward(backward);
    auto backwardLock = locks.cbegin();
    if (!backwardLock->compare(pidOriginal))
    {
        return false;
    }

    u32 pid=0;
    for (backwardLock++; backwardLock != locks.cend(); backwardLock++)
    {
        backward.advance(5);

        // If lock is a shadow assume PID is already set
        if (!backwardLock->getFree())
        {
            do
            {
                pid = getPIDBackward(backward);
            } while (!backwardLock->compare(pid));
        }
    }

    XDRNG forward(backward.getSeed());
    forward.advance(1);

    auto forwardLock = locks.crbegin();
    for (forwardLock++; forwardLock != locks.crend(); forwardLock++)
    {
        forward.advance(5);

        // If lock is a shadow assume PID is already set
        if (!forwardLock->getFree())
        {
            do
            {
                pid = getPIDForward(forward);
            } while (!forwardLock->compare(pid) || isShiny(pid, tsv));
        }
    }

    // Check if we end on the same PID as first non-shadow going backwards
    return pidOriginal == pid;
}

// Working backwards it is hard to know what PID would get rerolled from shiny lock
// Only check shiny lock working forwards which should be good enough
bool ShadowLock::firstShadowSet(u32 seed, u16 tsv)
{
    XDRNGR backward(seed);
    backward.advance(6);

    // Grab PID from first non-shadow going backwards
    // If it doesn't match spread fails
    u32 pidOriginal = getPIDBackward(backward);
    auto backwardLock = locks.cbegin();
    if (!backwardLock->compare(pidOriginal))
    {
        return false;
    }

    u32 pid=0;
    for (backwardLock++; backwardLock != locks.cend(); backwardLock++)
    {
        backward.advance(5);

        // If lock is a shadow assume PID is already set
        if (!backwardLock->getFree())
        {
            do
            {
                pid = getPIDBackward(backward);
            } while (!backwardLock->compare(pid));
        }
    }

    XDRNG forward(backward.getSeed());
    forward.advance(1);

    auto forwardLock = locks.crbegin();
    for (forwardLock++; forwardLock != locks.crend(); forwardLock++)
    {
        forward.advance(5);

        // If lock is a shadow assume PID is already set
        if (!forwardLock->getFree())
        {
            do
            {
                pid = getPIDForward(forward);
            } while (!forwardLock->compare(pid) || isShiny(pid, tsv));
        }
    }

    // Check if we end on the same PID as first non-shadow going backwards
    return pidOriginal == pid;
}

// Working backwards it is hard to know what PID would get rerolled from shiny lock
// Only check shiny lock working forwards which should be good enough
bool ShadowLock::firstShadowUnset(u32 seed, u16 tsv)
{
    XDRNGR backward(seed);
    backward.advance(3);

    // Shiny lock test for first shadow
    XDRNGR test(backward.getSeed());
    u16 shadowPSV = test.nextUShort() ^ test.nextUShort();
    while ((shadowPSV ^ tsv) < 8)
    {
        backward.setSeed(test.getSeed());
        shadowPSV = test.nextUShort() ^ test.nextUShort();
    }

    backward.advance(5);

    // Grab PID from first non-shadow going backwards
    // If it doesn't match spread fails
    u32 pidOriginal = getPIDBackward(backward);
    auto backwardLock = locks.cbegin();
    if (!backwardLock->compare(pidOriginal))
    {
        return false;
    }

    u32 pid=0;
    for (backwardLock++; backwardLock != locks.cend(); backwardLock++)
    {
        backward.advance(5);

        // If lock is a shadow assume PID is already set
        if (!backwardLock->getFree())
        {
            do
            {
                pid = getPIDBackward(backward);
            } while (!backwardLock->compare(pid));
        }
    }

    XDRNG forward(backward.getSeed());
    forward.advance(1);

    auto forwardLock = locks.crbegin();
    for (forwardLock++; forwardLock != locks.crend(); forwardLock++)
    {
        forward.advance(5);

        // If lock is a shadow assume PID is already set
        if (!forwardLock->getFree())
        {
            do
            {
                pid = getPIDForward(forward);
            } while (!forwardLock->compare(pid) || isShiny(pid, tsv));
        }
    }

    // Check if we end on the same PID as first non-shadow going backwards
    return pidOriginal == pid;
}

bool ShadowLock::salamenceSet(u32 seed, u16 tsv)
{
    XDRNGR backward(seed);
    backward.advance(6);

    // Build PID of non-shadow
    u32 pid = getPIDBackward(backward);

    return locks.first().compare(pid) && !isShiny(pid, tsv);
}

bool ShadowLock::salamenceUnset(u32 seed, u16 tsv)
{
    XDRNGR backward(seed);
    backward.advance(3);

    // Shiny lock test for first shadow
    XDRNGR test(backward.getSeed());
    u16 shadowPSV = test.nextUShort() ^ test.nextUShort();
    while ((shadowPSV ^ tsv) < 8)
    {
        backward.setSeed(test.getSeed());
        shadowPSV = test.nextUShort() ^ test.nextUShort();
    }

    backward.advance(5);

    // Build PID of non-shadow
    u32 pid = getPIDBackward(backward);

    // Backwards nature lock check
    return locks.first().compare(pid) && !isShiny(pid, tsv);
}

// The following is technically shiny locked by the trainer TID/SID
// It is extremely hard to know what that value is working from only IVs
// Assume that the shiny lock won't play a significant factor
bool ShadowLock::coloShadow(u32 seed)
{
    XDRNGR backward(seed);
    backward.advance(1);

    // Grab PID from first non-shadow going backwards
    // If it doesn't match spread fails
    u32 pidOriginal = getPIDBackward(backward);
    auto backwardLock = locks.cbegin();
    if (!backwardLock->compare(pidOriginal))
    {
        return false;
    }

    u32 pid=0;
    for (backwardLock++; backwardLock != locks.cend(); backwardLock++)
    {
        backward.advance(5);

        // If lock is a shadow assume PID is already set
        if (!backwardLock->getFree())
        {
            do
            {
                pid = getPIDBackward(backward);
            } while (!backwardLock->compare(pid));
        }
    }

    XDRNG forward(backward.getSeed());
    forward.advance(1);

    auto forwardLock = locks.crbegin();
    for (forwardLock++; forwardLock != locks.crend(); forwardLock++)
    {
        forward.advance(5);

        // If lock is a shadow assume PID is already set
        if (!forwardLock->getFree())
        {
            do
            {
                pid = getPIDForward(forward);
            } while (!forwardLock->compare(pid));
        }
    }

    // Check if we end on the same PID as first non-shadow going backwards
    return pidOriginal == pid;
}

// The following is technically shiny locked by the trainer TID/SID
// It is extremely hard to know what that value is working from only IVs
// Assume that the shiny lock won't play a significant factor
bool ShadowLock::ereader(u32 seed, u32 readerPID)
{
    // Check if PID is even valid for E-Reader
    // E-Reader have set nature/gender
    auto backwardLock = locks.cbegin();
    if (!backwardLock->compare(readerPID))
    {
        return false;
    }

    XDRNGR backward(seed);
    backward.advance(1);

    u32 pid=0;
    for (backwardLock++; backwardLock != locks.cend(); backwardLock++)
    {
        if (backwardLock != locks.begin() + 1)
        {
            backward.advance(5);
        }

        do
        {
            pid = getPIDBackward(backward);
        } while (!backwardLock->compare(pid));
    }

    XDRNG forward(backward.getSeed());
    forward.advance(1);

    auto forwardLock = locks.crbegin();
    for (forwardLock++; forwardLock != locks.crend(); forwardLock++)
    {
        forward.advance(5);

        do
        {
            pid = getPIDForward(forward);
        } while (!forwardLock->compare(pid));
    }

    // Checks if PID matches original
    return pid == readerPID;
}

void ShadowLock::switchLock(u8 lockNum, Method version)
{
    auto team = ShadowTeam::loadShadowTeams(version).at(lockNum);

    locks = team.getLocks();
    type = team.getType();
}
