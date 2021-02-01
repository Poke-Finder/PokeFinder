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

#include "EncounterArea3.hpp"

EncounterArea3::EncounterArea3(u8 location, Encounter encounter, const std::vector<Slot> &pokemon) :
    EncounterArea(location, encounter, pokemon)
{
}

u8 EncounterArea3::calcLevel(u8 index, u16 prng) const
{
    return (prng % (pokemon[index].getMaxLevel() - pokemon[index].getMinLevel() + 1)) + pokemon[index].getMinLevel();
}

u8 EncounterArea3::calcLevel(u8 index) const
{
    return pokemon[index].getMinLevel();
}

// Only for Rock Smash since all other encounters can be forced
u8 EncounterArea3::getEncounterRate() const
{
    switch (location)
    {
    case 18: // Route 111
    case 21: // Route 114
    case 47: // Granite Cave B2F
    case 72: // Victory Road B1F (RSE)
        return 20;
    case 51: // Safari Zone 4
    case 53: // Safari Zone 6
        return 25;
    // FRLG doesn't force encounter
    // No location collisions after encounter structure change
    // Might want to add game version as a parameter in the future, but for now this is fine
    case 48: // Rock Tunnel B1F (50)
    case 59: // Cerulean Cave 1F  (50)
    case 60: // Cerulean Cave 2F (50)
    case 61: // Cerulean Cave B1F (50)
    case 66: // Kindle Road (25)
    case 81: // Sevault Canyon (25)
    case 83: // Mt. Ember (50)
    case 85: // Mt. Ember Room 2 (50)
    case 87: // Mt. Ember 1F (50)
    case 88: // Mt. Ember B1F (50)
    case 89: // Mt. Ember B2F (50)
    case 90: // Mt. Ember B3F (50)
    case 91: // Mt. Ember B4F (50)
    case 92: // Mt. Ember B5F (50)
        return 180;
    default:
        return 0;
    }
}

bool EncounterArea3::isSafariZone() const
{
    switch (location)
    {
    case 48: // Safari Zone 1
    case 49: // Safari Zone 2
    case 50: // Safari Zone 3
    case 51: // Safari Zone 4
    case 52: // Safari Zone 5
    case 53: // Safari Zone 6
        return true;
    default:
        return false;
    }
}
