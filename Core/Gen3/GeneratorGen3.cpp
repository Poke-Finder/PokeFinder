/*
 * This file is part of PokéFinder
 * Copyright (C) 2017 by Admiral_Fish and bumba
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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

#include "GeneratorGen3.hpp"

// Default constructor
GeneratorGen3::GeneratorGen3()
{
    MaxResults = 100000;
    InitialFrame = 1;
    InitialSeed = 0;
    tid = 12345;
    sid = 54321;
}

// Constructor given user defined parameters
GeneratorGen3::GeneratorGen3(uint32_t MaxResults, uint32_t InitialFrame, uint32_t InitialSeed, uint32_t tid, uint32_t sid)
{
    this->MaxResults = MaxResults;
    this->InitialFrame = InitialFrame;
    this->InitialSeed = InitialSeed;
    this->tid = tid;
    this->sid = sid;
}

// Determines what generational method to return
std::vector<FrameGen3> GeneratorGen3::Generate()
{
    rng.setSeed(InitialSeed);
    rng.advanceFrames(InitialFrame - 1);

    if (FrameType == Method1 || FrameType == MethodH1)
    {
        iv1 = FrameType == MethodH1 ? 1 : 2;
        iv2 = FrameType == MethodH1 ? 2 : 3;
    }
    else if (FrameType == Method2 || FrameType == MethodH2)
    {
        iv1 = FrameType == MethodH2 ? 2 : 3;
        iv2 = FrameType == MethodH2 ? 3 : 4;
    }
    else if (FrameType == Method4 || FrameType == MethodH4)
    {
        iv1 = FrameType == MethodH4 ? 1 : 2;
        iv2 = FrameType == MethodH4 ? 3 : 4;
    }

    switch (FrameType)
    {
        case Method1:
        case Method2:
        case Method4:
            rng.setpokeRNG();
            return GenerateMethod124();
        case MethodH1:
        case MethodH2:
        case MethodH4:
            rng.setpokeRNG();
            switch (LeadType)
            {
                case None:
                    return GenerateMethodH124();
                case Synchronize:
                    return GenerateMethodH124Synch();
                // case CuteCharm:
                // Set to default to avoid compiler warning message
                default:
                    return GenerateMethodH124CuteCharm();
            }
        case XDColo:
            rng.setxdRNG();
            return GenerateMethodXDColo();
        // case Channel:
        // Set to default to avoid compiler warning message
        default:
            rng.setxdRNG();
            return GenerateMethodChannel();
    }
}

// Returns vector of frames Method 1, 2, or 4
std::vector<FrameGen3> GeneratorGen3::GenerateMethod124()
{
    std::vector<FrameGen3> frames;
    for (int i = 0; i < 5; i++)
        rngList.push_back(rng.next16Bit());

    // Method 1 [SEED] [PID] [PID] [IVS] [IVS]
    // Method 2 [SEED] [PID] [PID] [BLANK] [IVS] [IVS]
    // Method 4 [SEED] [PID] [PID] [IVS] [BLANK] [IVS]

    uint32_t max = InitialFrame + MaxResults;
    for (uint32_t cnt = InitialFrame; cnt < max; cnt++, rngList.erase(rngList.begin()), rngList.push_back(rng.next16Bit()))
    {
        FrameGen3 frame = FrameGen3(tid, sid);
        frame.setPID(rngList[1], rngList[0]);
        frame.setIVs(rngList[iv1], rngList[iv2]);
        frame.frame = cnt;
        frames.push_back(frame);
    }
    rngList.clear();
    return frames;
}

// Returns vector of frames Method H 1, 2, or 4
std::vector<FrameGen3> GeneratorGen3::GenerateMethodH124()
{
    std::vector<FrameGen3> frames;
    for (int i = 0; i < 1100; i++)
        rngList.push_back(rng.next16Bit());

    uint32_t max = InitialFrame + MaxResults;
    uint32_t pid, pid1, pid2, hunt;
    for (uint32_t cnt = InitialFrame; cnt < max; cnt++, rngList.erase(rngList.begin()), rngList.push_back(rng.next16Bit()))
    {
        FrameGen3 frame = FrameGen3(tid, sid);

        hunt = 0;
        frame.encounterSlot = EncounterSlot::HSlot(rngList[hunt++], EncounterType);

        // Method H relies on grabbing a hunt nature and generating PIDs until the PID nature matches the hunt nature
        // Grab the hunt nature
        frame.nature = rngList[++hunt] % 25;

        // Now search for a Method 124 PID that matches our hunt nature
        do
        {
            pid2 = rngList[++hunt];
            pid1 = rngList[++hunt];
            pid = (pid1 << 16) | pid2;
        }
        while (pid % 25 != frame.nature);

        // Valid PID is found now time to generate IVs
        frame.setIVs(rngList[hunt + iv1], rngList[hunt + iv2]);
        frame.frame = cnt;
        frame.setPID(pid);
        frame.occidentary = hunt + cnt - 1;
        frames.push_back(frame);
    }
    rngList.clear();
    return frames;
}

// Returns vector of frames Method H 1, 2, or 4 given synch lead
std::vector<FrameGen3> GeneratorGen3::GenerateMethodH124Synch()
{
    std::vector<FrameGen3> frames;
    for (int i = 0; i < 1200; i++)
        rngList.push_back(rng.next16Bit());

    uint32_t max = InitialFrame + MaxResults;
    uint32_t pid, pid1, pid2, hunt, first;
    for (uint32_t cnt = InitialFrame; cnt < max; cnt++, rngList.erase(rngList.begin()), rngList.push_back(rng.next16Bit()))
    {
        FrameGen3 frame = FrameGen3(tid, sid);

        hunt = 0;
        frame.encounterSlot = EncounterSlot::HSlot(rngList[hunt++], EncounterType);

        // Method H relies on grabbing a hunt nature and generating PIDs until the PID nature matches the hunt nature
        // Check by seeing if frame can synch
        first = rngList[++hunt];
        if ((first & 1) == 0)
        {
            // Frame is synchable so set nature to synch nature
            frame.nature = SynchNature;
        }
        else
        {
            // Synch failed so grab hunt nature from next RNG call
            frame.nature = rngList[++hunt] % 25;
        }

        // Now search for a Method 124 PID that matches our hunt nature
        do
        {
            pid2 = rngList[++hunt];
            pid1 = rngList[++hunt];
            pid = (pid1 << 16) | pid2;
        }
        while (pid % 25 != frame.nature);

        // Valid PID is found now time to generate IVs
        frame.setIVs(rngList[hunt + iv1], rngList[hunt + iv2]);
        frame.frame = cnt;
        frame.setPID(pid);
        frame.occidentary = hunt + cnt - 1;
        frames.push_back(frame);
    }
    rngList.clear();
    return frames;
}

// Returns vector of frames Method H 1, 2, or 4 given cute charm lead
std::vector<FrameGen3> GeneratorGen3::GenerateMethodH124CuteCharm()
{
    std::vector<FrameGen3> frames;
    for (int i = 0; i < 2500; i++)
        rngList.push_back(rng.next16Bit());

    uint32_t max = InitialFrame + MaxResults;
    uint32_t pid, pid1, pid2, hunt, first;
    for (uint32_t cnt = InitialFrame; cnt < max; cnt++, rngList.erase(rngList.begin()), rngList.push_back(rng.next16Bit()))
    {
        FrameGen3 frame = FrameGen3(tid, sid);

        hunt = 0;
        frame.encounterSlot = EncounterSlot::HSlot(rngList[hunt++], EncounterType);

        // Method H relies on grabbing a hunt nature and generating PIDs until the PID nature matches the hunt nature
        first = rngList[++hunt];

        // Cute charm uses first call
        // Call next RNG to determine hunt nature
        frame.nature = rngList[++hunt] % 25;

        // Check if cute charm will effect frame
        if (first % 3 > 0)
        {
            // Now search for a Method 124 PID that matches our hunt nature and cute charm
            do
            {
                pid2 = rngList[++hunt];
                pid1 = rngList[++hunt];
                pid = (pid1 << 16) | pid2;
            }
            while (pid % 25 != frame.nature || !cuteCharm(pid));
        }
        else
        {
            // Cute charm failed
            // Only search for a Method 124 PID that matches our hunt nature
            do
            {
                pid2 = rngList[++hunt];
                pid1 = rngList[++hunt];
                pid = (pid1 << 16) | pid2;
            }
            while (pid % 25 != frame.nature);
        }

        // Valid PID is found now time to generate IVs
        frame.setIVs(rngList[hunt + iv1], rngList[hunt + iv2]);
        frame.frame = cnt;
        frame.setPID(pid);
        frame.occidentary = hunt + cnt - 1;
        frames.push_back(frame);
    }
    rngList.clear();
    return frames;
}

// Checks if PID fits constraints of cute charm
bool GeneratorGen3::cuteCharm(uint32_t pid)
{
    switch (LeadType)
    {
        case CuteCharm125F:
            return (pid & 0xff) < 31;
        case CuteCharm875M:
            return (pid & 0xff) >= 31;
        case CuteCharm25F:
            return (pid & 0xff) < 63;
        case CuteCharm75M:
            return (pid & 0xff) >= 63;
        case CuteCharm50F:
            return (pid & 0xff) < 127;
        case CuteCharm50M:
            return (pid & 0xff) >= 127;
        case CuteCharm75F:
            return (pid & 0xff) < 191;
        // Case CuteCharm25M:
        // Set to default to avoid compiler warning message
        default:
            return (pid & 0xff) >= 191;
    }
}

// Returns vector of frames Method XD/Colo
std::vector<FrameGen3> GeneratorGen3::GenerateMethodXDColo()
{
    std::vector<FrameGen3> frames;
    for (int i = 0; i < 5; i++)
        rngList.push_back(rng.next16Bit());

    // Method XD/Colo [SEED] [IVS] [IVS] [BLANK] [PID] [PID]

    uint32_t max = InitialFrame + MaxResults;
    for (uint32_t cnt = InitialFrame; cnt < max; cnt++, rngList.erase(rngList.begin()), rngList.push_back(rng.next16Bit()))
    {
        FrameGen3 frame = FrameGen3(tid, sid);
        frame.setPID(rngList[3], rngList[4]);
        frame.setIVs(rngList[0], rngList[1]);
        frame.frame = cnt;
        frames.push_back(frame);
    }
    rngList.clear();
    return frames;
}

// Returns vector of frames Method Channel
std::vector<FrameGen3> GeneratorGen3::GenerateMethodChannel()
{
    std::vector<FrameGen3> frames;
    for (int i = 0; i < 12; i++)
        rngList.push_back(rng.next16Bit());

    // Method Channel [SEED] [SID] [PID] [PID] [BERRY] [GAME ORIGIN] [OT GENDER] [IV] [IV] [IV] [IV] [IV] [IV]

    uint32_t max = InitialFrame + MaxResults;
    for (uint32_t cnt = InitialFrame; cnt < max; cnt++, rngList.erase(rngList.begin()), rngList.push_back(rng.next16Bit()))
    {
        FrameGen3 frame = FrameGen3(40122, rngList[0]);
        if ((rngList[2] > 7 ? 0 : 1) != (rngList[1] ^ 40122 ^ sid))
            frame.setPID(rngList[1] ^ 0x8000, rngList[2]);
        else
            frame.setPID(rngList[1], rngList[2]);
        frame.setIVsChannel(rngList[6] >> 11, rngList[7] >> 11, rngList[8] >> 11, rngList[10] >> 11, rngList[11] >> 11, rngList[9] >> 11);
        frame.frame = cnt;
        frames.push_back(frame);
    }
    rngList.clear();
    return frames;
}