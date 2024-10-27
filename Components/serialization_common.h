/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include <Common/Utils/IServiceLocator.h>
#include "Common/Containers/String.h"
#include "Logging.h"
#include "Common/Containers/StringView.h"
#include "Common/Containers/Vector.h"

#include <cereal/archives/json.hpp>
#include <cereal/archives/memory_binary.hpp>
#include <cereal/eastl/vector.hpp>
#include <cereal/eastl/deque.hpp>
#include <cereal/eastl/array.hpp>
#include <cereal/eastl/string.hpp>
#include <cereal/cereal.hpp>
#include <EASTL/functional.h>
#include <EASTL/unique_ptr.h>

#include <cstdio>
namespace SEGS {

inline Vector<char> IFile_readAll(IFile *self)
{
    Vector<char> res;
    res.resize(self->size());
    self->read(res.data(),res.size());
    return res;
}

inline Vector<char> IFile_read(IFile *self,int64_t len) {
    if(self->pos()+len>=self->size())
        return {};
    Vector<char> res;
    res.resize(len);
    if(self->read(res.data(),len)==len)
        return res;
    return {};
}


template<class T>
void commonSaveTo(const T & target, const char *classname, const String & baseName, bool text_format)
{
    using namespace magic_enum::bitwise_operators;
    String target_fname;
    if(text_format)
        target_fname = baseName + ".crl.json";
    else
        target_fname = baseName + ".crl.bin";
    auto fs=getServiceLocator()->getFS();
    try
    {
        if(text_format) {
            std::ostringstream tgt;
            {
                cereal::JSONOutputArchive ar( tgt );
                ar(cereal::make_nvp(classname,target));
            }
            eastl::unique_ptr<IFile> tgt_fle(
                fs->open(target_fname, IFile::OpenMode(SEGS::IFile::WriteOnly | SEGS::IFile::Text)));
            if(!tgt_fle) {
                sCritical() << "Failed to open"<<target_fname<<"in write mode";
                return;
            }
            tgt_fle->write(tgt.str().c_str(),tgt.str().size());
        }
        else {
            eastl::unique_ptr<IFile> tgt_fle(fs->open(target_fname,SEGS::IFile::WriteOnly));
            eastl::vector<uint8_t> tgt;
            cereal::VectorOutputArchive ar( tgt );
            ar(cereal::make_nvp(classname,target));
            if(!tgt_fle) {
                sCritical() << "Failed to open"<<target_fname<<"in write mode";
                return;
            }
            tgt_fle->write((const char *)tgt.data(),tgt.size());
        }
    }
    catch(cereal::RapidJSONException &e)
    {
        sWarning() << e.what();
    }
    catch(std::exception &e)
    {
        sCritical() << e.what();
    }
}
template<class T>
bool commonReadFrom(const String &crl_path,const char *classname, T &target)
{
    auto fs=getServiceLocator()->getFS();
    if(crl_path.ends_with("json") || crl_path.ends_with("crl_json"))
    {
        eastl::unique_ptr<IFile> ifl(fs->open(crl_path,(IFile::OpenMode)(IFile::ReadOnly|IFile::Text)));
        if(!ifl)
        {
            sWarning() << "Failed to open" << crl_path;
            return false;
        }
        auto contents=IFile_readAll(ifl.get());
        std::istringstream istr(std::string(contents.begin(),contents.end()));
        try
        {
            cereal::JSONInputArchive arc(istr);
            arc(cereal::make_nvp(classname,target));
        }
        catch(cereal::RapidJSONException &e)
        {
            sWarning() << e.what();
        }
        catch (std::exception &e)
        {
            sCritical() << e.what();
        }
    }
    else if(crl_path.ends_with(".crl.bin"))
    {
        eastl::unique_ptr<IFile> ifl(fs->open(crl_path,IFile::ReadOnly));
        if(!ifl)
        {
            sWarning() << "Failed to open" << crl_path;
            return false;
        }

        eastl::vector<uint8_t> istr;
        istr.resize(ifl->size());
        ifl->read((char *)istr.data(),ifl->size());
        try
        {
            cereal::VectorInputArchive arc(istr);
            arc(cereal::make_nvp(classname,target));
        }
        catch(cereal::RapidJSONException &e)
        {
            sWarning() << e.what();
        }
        catch (std::exception &e)
        {
            sCritical() << e.what();
        }
    }
    else {
        sWarning() << "Invalid serialized data extension in" <<crl_path;
    }
    return true;
}
}




template<class T>
void serializeToQString(const T &data, String &tgt)
{
    std::ostringstream ostr;
    {
        cereal::JSONOutputArchive ar(ostr);
        ar(data);
    }
    auto res = ostr.str();
    tgt = String(res.c_str(),res.size());
}

template<class T>
void serializeFromQString(T &data,const String &src)
{
    if(src.empty())
        return;
    std::istringstream istr;
    std::string src_str(src.begin(),src.end());
    istr.str(src_str);
    {
        cereal::JSONInputArchive ar(istr);
        ar(data);
    }
}
#define SPECIALIZE_SERIALIZATIONS(type)\
template \
void type::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive & archive);\
template \
void type::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive & archive);\
template \
void type::serialize<cereal::VectorInputArchive>(cereal::VectorInputArchive & archive);\
template \
void type::serialize<cereal::VectorOutputArchive>(cereal::VectorOutputArchive & archive);
#define SPECIALIZE_VERSIONED_SERIALIZATIONS(type)\
template \
void serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive & archive, type & m, uint32_t const version);\
template \
void serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive & archive, type & m, uint32_t const version);\
template \
void serialize<cereal::VectorInputArchive>(cereal::VectorInputArchive & archive, type & m, uint32_t const version);\
template \
void serialize<cereal::VectorOutputArchive>(cereal::VectorOutputArchive & archive, type & m, uint32_t const version);
#define SPECIALIZE_CLASS_VERSIONED_SERIALIZATIONS(type)\
template \
void type::serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive & archive, uint32_t const version);\
template \
void type::serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive & archive, uint32_t const version);\
template \
void type::serialize<cereal::VectorInputArchive>(cereal::VectorInputArchive & archive, uint32_t const version);\
template \
void type::serialize<cereal::VectorOutputArchive>(cereal::VectorOutputArchive & archive, uint32_t const version);

#define SPECIALIZE_SPLIT_SERIALIZATIONS(type)\
template \
void CEREAL_SAVE_FUNCTION_NAME<cereal::JSONOutputArchive>(cereal::JSONOutputArchive & archive, const type & m);\
template \
void CEREAL_SAVE_FUNCTION_NAME<cereal::VectorOutputArchive>(cereal::VectorOutputArchive & archive, const type & m);\
template \
void CEREAL_LOAD_FUNCTION_NAME<cereal::JSONInputArchive>(cereal::JSONInputArchive & archive, type & m);\
template \
void CEREAL_LOAD_FUNCTION_NAME<cereal::VectorInputArchive>(cereal::VectorInputArchive & archive, type & m);

