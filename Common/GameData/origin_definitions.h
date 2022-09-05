/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include <vector>
#include <QtCore/QString>
#define ENABLE_Q_REFLECTION
#ifdef ENABLE_Q_REFLECTION
#include <QObject>
#endif

struct Parse_Origin
{
#ifdef ENABLE_Q_REFLECTION
    Q_GADGET
    Q_PROPERTY(QByteArray Name MEMBER Name)
    Q_PROPERTY(QByteArray DisplayName MEMBER DisplayName)
    Q_PROPERTY(QByteArray DisplayHelp MEMBER DisplayHelp)
    Q_PROPERTY(QByteArray DisplayShortHelp MEMBER DisplayShortHelp)
    Q_PROPERTY(QByteArray Icon MEMBER Icon)
    Q_PROPERTY(int NumBonusPowerSets MEMBER NumBonusPowerSets)
    Q_PROPERTY(int NumBonusPowers MEMBER NumBonusPowers)
    Q_PROPERTY(int NumBonusBoostSlots MEMBER NumBonusBoostSlots)
    Q_PROPERTY(int NumContacts MEMBER NumContacts)
    Q_PROPERTY(float ContactBonusLength MEMBER ContactBonusLength)
#endif

public:
    QByteArray Name;
    QByteArray DisplayName;
    QByteArray DisplayHelp;
    QByteArray DisplayShortHelp;
    QByteArray Icon; // i24
    // Those fields below, are missing in I24
    // probably since all origins are the same in regards to those settings.
    int NumBonusPowerSets=0;
    int NumBonusPowers=0;
    int NumBonusBoostSlots=0;
    int NumContacts=0;
    float ContactBonusLength=0.0f;
};
using Parse_AllOrigins = std::vector<Parse_Origin>;
