/*
 * File:   Power.cpp
 * Author: b4n92uid
 *
 * Created on 25 juin 2011, 20:47
 */

#include "Power.h"

#include "GameManager.h"
#include "SoundManager.h"

Power::Power(GameManager* gameManager)
{
    m_slot = 0;
    m_name = "Power";

    m_active = false;
    m_owner = NULL;
    m_gameManager = gameManager;
    m_soundManager = m_gameManager->manager.sound;
}

Power::~Power()
{
}

void Power::activate(tbe::Vector3f target)
{
    m_active = true;
}

void Power::diactivate()
{
    m_active = false;
}

bool Power::isActive() const
{
    return m_active;
}

void Power::setOwner(Player* owner)
{
    this->m_owner = owner;
}

Player* Power::getOwner() const
{
    return m_owner;
}

void Power::setName(std::string name)
{
    this->m_name = name;
}

std::string Power::getName() const
{
    return m_name;
}

unsigned Power::getSlot() const
{
    return m_slot;
}
