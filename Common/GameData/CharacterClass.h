/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "CharacterAttributes.h"

#include "Common/Containers/String.h"
#include <EASTL/unique_ptr.h>

struct ClassMod_Data
{
    String Name;
    Vector<float> Values;
    template<class Archive>
    void serialize(Archive & archive);
};

struct CharClass_Data
{
    String m_Name;
    String m_DisplayName;
    String m_DisplayHelp;
    String m_DisplayShortHelp;
    Vector<Parse_CharAttrib> m_AttribBase;
    Vector<Parse_CharAttrib> m_AttribMin;
    Vector<Parse_CharAttrib> m_StrengthMin;
    Vector<Parse_CharAttrib> m_ResistanceMin;
    eastl::unique_ptr<Parse_CharAttrib> _FinalAttrMax_;
    eastl::unique_ptr<Parse_CharAttrib> _FinalAttrMaxMax_;
    eastl::unique_ptr<Parse_CharAttrib> _FinalAttrStrengthMax_;
    eastl::unique_ptr<Parse_CharAttrib> _FinalAttrResistanceMax_;
    Vector<ClassMod_Data> m_ModTable;
    String m_PrimaryCategory;
    String m_SecondaryCategory;
    String m_PowerPoolCategory;
    Vector<Parse_CharAttribMax> m_AttribMaxTable;
    Vector<Parse_CharAttribMax> m_AttribMaxMaxTable;
    Vector<Parse_CharAttribMax> m_StrengthMaxTable;
    Vector<Parse_CharAttribMax> m_ResistanceMaxTable;
    template<class Archive>
    void serialize(Archive & archive);
};

using Parse_AllCharClasses = Vector<CharClass_Data>;
