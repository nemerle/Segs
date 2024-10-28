/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Components/serialization_common.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <type_traits>

struct RGBA;

using Vec3 = glm::vec3;
using Vec2 = glm::vec2;

// Note: members reading strings are not thread safe, as they use a static buffer
class BinStore // binary storage
{
public:

    const String &source_name() {
        return read_str(12000);
    }
    bool        read_bytes(char *tgt,size_t sz);
    const String &  read_str(size_t maxlen);
    bool        read(uint32_t &v);
    bool        read(int32_t &v);
    bool        read(float &v);
    bool        read(uint16_t &v);
    bool        read(uint8_t &v);
    bool        readU(uint8_t &v);
    bool        read(Vec2 &v);
    bool        read(Vec3 &v);
    bool        read(RGBA &v);
    bool        read(Vector<uint32_t> &v);
    bool        read(Vector<int32_t> &res);
    bool        read(Vector<float> &res);
    bool        read(Vector<String> &res);
    bool        read(Vector<Vector<String>> &res);
    bool        read(uint8_t *&val, uint32_t length);
    bool        read(String &val);
    bool        read(eastl::pair<uint8_t,uint8_t> &v) {
                    uint8_t skipped, skipped2;
                    return read_internal(v.first)!=0 &&
                    read_internal(v.second)!=0 && read_internal(skipped) != 0 && read_internal(skipped2) != 0;
                }
                template<class Enum>
    bool        readEnum(Enum &val)
                {
                    typename std::underlying_type<Enum>::type true_val;
                    if(!read(true_val))
                        return false;
                    val = Enum(true_val);
                    return true;
                }
                template<class Enum>
    bool        readEnum(Vector<Enum> &val)
                {
                    Vector<typename std::underlying_type<Enum>::type> true_val;
                    if(!read(true_val))
                        return false;
                    // this is bad, and I feel bad for writing it :/
                    val = std::move(*(Vector<Enum> *)(&true_val));
                    return true;
                }
    void        prepare();
    bool        prepare_nested();
    bool        nesting_name(String &name);
    void        nest_in() {  }
    void        nest_out() { m_file_sizes.pop_back(); }
    bool        end_encountered() const;
    bool        open(const String & name, uint32_t required_crc);
                ~BinStore();
    uint32_t    get_bytes_to_read() const { return bytes_to_read; }
private:
    struct FileEntry {
        String name;
        uint32_t date=0;
    };
    SEGS::IFile *m_str = nullptr;
    size_t bytes_read=0;
    uint32_t bytes_to_read=0;
    Vector<uint32_t> m_file_sizes; // implicit stack
    Vector<FileEntry> m_entries;

    template<class V>
    size_t read_internal(V &res)
    {
        if(!m_file_sizes.empty() && current_fsize()<sizeof(V))
            return 0;
        bool rr=m_str->read((char *)&res,sizeof(V));
        assert(rr);
        if(!m_file_sizes.empty())
        {
            bytes_read+=sizeof(V);
            m_file_sizes.back()-=sizeof(V);
        }
        return sizeof(V);
    }
    const String &read_pstr(size_t maxlen);
    void        skip_pstr();
    bool        read_data_blocks(bool file_data_blocks);
    bool        check_bin_version_and_crc(uint32_t req_crc);
    uint32_t    current_fsize() {return m_file_sizes.back();}
    uint32_t    read_header(String &name, size_t maxlen);
    void        fixup();
};
