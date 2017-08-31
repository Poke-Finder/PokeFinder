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

#include "Frame.hpp"

// Default constructor
Frame::Frame()
{
    tid = 12345;
    sid = 54321;
}

// Sets PID and calculates characteristics based on PID
void Frame::setPID(uint32_t pid1, uint32_t pid2)
{
    pid = (pid1 << 16) | pid2;
    nature = pid % 25;
    gender = pid & 255;
    ability = pid & 1;
    if ((pid1 ^ pid2 ^ tid ^ sid) < 8)
        shiny = true;
    else
        shiny = false;
}

// Sets PID and calculates characteristics based on PID
void Frame::setPID(uint32_t pid)
{
    this->pid = pid;
    gender = pid & 255;
    ability = pid & 1;
    if (((pid >> 16) ^ (pid & 0xffff) ^ tid ^ sid) < 8)
        shiny = true;
    else
        shiny = false;
}

// Sets IVs and calculates characteristics based on IVs
void Frame::setIVs(uint32_t iv1, uint32_t iv2)
{
    hp = iv1 & 0x1f;
    atk = (iv1 >> 5) & 0x1f;
    def = (iv1 >> 10) & 0x1f;
    spa = (iv2 >> 5) & 0x1f;
    spd = (iv2 >> 10) & 0x1f;
    spe = iv2 & 0x1f;
    hidden = ((((hp & 1) + 2 * (atk & 1) + 4 * (def & 1) + 8 * (spe & 1) + 16 * (spa & 1) + 32 * (spd & 1)) * 15) / 63);
    power = (30 + ((((hp >> 1) & 1) + 2 * ((atk >> 1) & 1) + 4 * ((def >> 1) & 1) + 8 * ((spe >> 1) & 1) + 16 * ((spa >> 1) & 1) + 32 * ((spd >> 1) & 1)) * 40 / 63));
}

// Returns string equivalent of nature
std::string Frame::getNature()
{
    return natures[nature];
}

// Returns string equivalent of hidden power
std::string Frame::getPower()
{
    return powers[hidden];
}

// Returns gender of 12.5% F ratio
std::string Frame::getFemale125()
{
    return (gender >= 31) ? "M" : "F";
}

// Returns gender of 25% F ratio
std::string Frame::getFemale25()
{
    return (gender >= 63) ? "M" : "F";
}

// Returns gender of 50% F ratio
std::string Frame::getFemale50()
{
    return (gender >= 127) ? "M" : "F";
}

// Returns gender of 75% F ratio
std::string Frame::getFemale75()
{
    return (gender >= 191) ? "M" : "F";
}

// Returns !!! if shiny or blank if not
std::string Frame::getShiny()
{
    return shiny ? "!!!" : "";
}