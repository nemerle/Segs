//#include <charconv>
#include "string_utils.h"

#include "Components/Logging.h"
#include <cassert>
#include <charconv>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>

using ChronoData = std::chrono::system_clock::time_point;

// from https://github.com/B-Con/crypto-algorithms/blob/master/sha256.c

namespace {
/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
struct SHA256_CTX {
    uint8_t data[64];
    uint32_t datalen;
    unsigned long long bitlen;
    uint32_t state[8];
};

/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const uint8_t data[], size_t len);
void sha256_final(SHA256_CTX *ctx, uint8_t hash[]);

/****************************** MACROS ******************************/
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

/**************************** VARIABLES *****************************/
static const uint32_t k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

/*********************** FUNCTION DEFINITIONS ***********************/
void sha256_transform(SHA256_CTX *ctx, const uint8_t data[])
{
    uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    for ( ; i < 64; ++i)
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

void sha256_init(SHA256_CTX *ctx)
{
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX *ctx, const uint8_t data[], size_t len)
{
    uint32_t i;

    for (i = 0; i < len; ++i) {
        ctx->data[ctx->datalen] = data[i];
        ctx->datalen++;
        if (ctx->datalen == 64) {
            sha256_transform(ctx, ctx->data);
            ctx->bitlen += 512;
            ctx->datalen = 0;
        }
    }
}

void sha256_final(SHA256_CTX *ctx, uint8_t hash[])
{
    uint32_t i;

    i = ctx->datalen;

           // Pad whatever data is left in the buffer.
    if (ctx->datalen < 56) {
        ctx->data[i++] = 0x80;
        while (i < 56)
            ctx->data[i++] = 0x00;
    }
    else {
        ctx->data[i++] = 0x80;
        while (i < 64)
            ctx->data[i++] = 0x00;
        sha256_transform(ctx, ctx->data);
        memset(ctx->data, 0, 56);
    }

           // Append to the padding the total message's length in bits and transform.
    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63] = ctx->bitlen;
    ctx->data[62] = ctx->bitlen >> 8;
    ctx->data[61] = ctx->bitlen >> 16;
    ctx->data[60] = ctx->bitlen >> 24;
    ctx->data[59] = ctx->bitlen >> 32;
    ctx->data[58] = ctx->bitlen >> 40;
    ctx->data[57] = ctx->bitlen >> 48;
    ctx->data[56] = ctx->bitlen >> 56;
    sha256_transform(ctx, ctx->data);

           // Since this implementation uses little endian byte ordering and SHA uses big endian,
           // reverse all the bytes when copying the final state to the output hash.
    for (i = 0; i < 4; ++i) {
        hash[i]      = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}
}

using namespace eastl;

/*
    TODO: SEGS: When replacing QString as the underlying string type consider the following helper class from qt
    QTextBoundaryFinder for grapheme navigation in QChar-like *strings
    QUtf8 from QtCore/private/qutfcodec_p.h>
*/

#if defined(MINGW_ENABLED) || defined(_MSC_VER)
#define snprintf _snprintf_s
#endif

#define MAX_DIGITS 6
namespace {
    size_t find_char(StringView s, char p_char) {
        return s.find(p_char);
    }

    bool is_enclosed_in(StringView str, char p_char) {
        return str.starts_with(p_char) && str.ends_with(p_char);
    }
}

bool is_symbol(char c) {
    return c != '_' &&
           ((c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~') ||
            c == '\t' || c == ' ');
}

void StringUtils::erase(String &str, int p_pos, int p_chars) {
    str.erase(p_pos, p_chars);
}

String StringUtils::capitalize(StringView s) {
    String aux(strip_edges(camelcase_to_underscore(s, true).replaced("_", " ")));
    String cap;
    for (int i = 0; i < get_slice_count(aux, ' '); i++) {

        StringView slice = get_slice(aux, ' ', i);
        if (!slice.empty()) {

            //slice.set(0,StringUtils::char_uppercase(slice[0]));
            if (i > 0) {
                cap += ' ';
            }
            cap.push_back(CharToUpper(slice[0]));
            cap.append(slice.substr(1));
        }
    }

    return cap;
}

String StringUtils::camelcase_to_underscore(StringView s, bool lowercase) {
    const char *cstr = s.data();
    String new_string;
    const char A = 'A', Z = 'Z';
    const char a = 'a', z = 'z';
    int start_index = 0;

    for (size_t i = 1; i < s.size(); i++) {
        bool is_upper = cstr[i] >= A && cstr[i] <= Z;
        bool is_number = cstr[i] >= '0' && cstr[i] <= '9';
        bool are_next_2_lower = false;
        bool is_next_lower = false;
        bool is_next_number = false;
        bool was_precedent_upper = cstr[i - 1] >= A && cstr[i - 1] <= Z;
        bool was_precedent_number = cstr[i - 1] >= '0' && cstr[i - 1] <= '9';

        if (i + 2 < s.size()) {
            are_next_2_lower = cstr[i + 1] >= a && cstr[i + 1] <= z && cstr[i + 2] >= a && cstr[i + 2] <= z;
        }

        if (i + 1 < s.size()) {
            is_next_lower = cstr[i + 1] >= a && cstr[i + 1] <= z;
            is_next_number = cstr[i + 1] >= '0' && cstr[i + 1] <= '9';
        }

        const bool cond_a = is_upper && !was_precedent_upper && !was_precedent_number;
        const bool cond_b = was_precedent_upper && is_upper && are_next_2_lower;
        const bool cond_c = is_number && !was_precedent_number;
        const bool can_break_number_letter = is_number && !was_precedent_number && is_next_lower;
        const bool can_break_letter_number = !is_number && was_precedent_number && (is_next_lower || is_next_number);

        bool should_split = cond_a || cond_b || cond_c || can_break_number_letter || can_break_letter_number;
        if (should_split) {
            new_string.append(substr(s, start_index, i - start_index));
            new_string.append("_");
            start_index = i;
        }
    }

    new_string += substr(s, start_index);
    return lowercase ? to_lower(new_string) : new_string;
}

int StringUtils::get_slice_count(StringView str, char p_splitter) {
    if (str.empty()) {
        return 0;
    }
    int count = 1;
    auto loc = str.find(p_splitter);
    while (loc != str.npos) {
        count++;
        loc = str.find(p_splitter, loc + 1);
    }
    return count;
}

int StringUtils::get_slice_count(StringView str, StringView p_splitter) {
    if (str.empty() || p_splitter.empty()) {
        return 0;
    }
    int count = 1;
    auto loc = str.find(p_splitter);
    while (loc != str.npos) {
        count++;
        loc = str.find(p_splitter, loc + 1);
    }
    return count;
}

StringView StringUtils::get_slice(StringView str, StringView p_splitter, int p_slice) {
    if (p_slice < 0 || str.empty() || p_splitter.empty()) {
        return StringView();
    }

    size_t pos = 0;
    size_t prev_pos = 0;

    if (not contains(str, p_splitter)) {
        return str;
    }

    int i = 0;
    while (true) {

        pos = StringUtils::find(str, p_splitter, pos);
        if (pos == StringView::npos) {
            pos = str.length(); // reached end
        }

        int from = prev_pos;
        //int to=pos;

        if (p_slice == i) {

            return substr(str, from, pos - from);
        }

        if (pos == str.length()) { // reached end and no find
            break;
        }
        pos += p_splitter.length();
        prev_pos = pos;
        i++;
    }

    return StringView(); //no find!
}

StringView StringUtils::get_slice(StringView str, char p_splitter, int p_slice) {
    if (str.empty()) {
        return StringView();
    }

    if (p_slice < 0) {
        return StringView();
    }

    const char *c = str.data();
    int i = 0;
    int fin = str.size();
    int prev = 0;
    int count = 0;
    for (i = 0; i < fin; ++i) {

        if (c[i] == p_splitter) {
            if (p_slice == count) {
                return substr(str, prev, i - prev);
            } else {
                count++;
                prev = i + 1;
            }
        }
    }
    if (p_slice == count) {
        return substr(str, prev);
    }
    return StringView();
}

Vector<StringView> StringUtils::split_spaces(StringView str) {

    Vector<StringView> ret;
    int from = 0;
    int i = 0;
    int len = str.length();
    if (len == 0) {
        return ret;
    }

    bool inside = false;

    for (i = 0; i < len; ++i) {

        bool empty = str[i] < 33;

        if (i == 0) {
            inside = !empty;
        }

        if (!empty && !inside) {
            inside = true;
            from = i;
        }

        if (empty && inside) {

            ret.push_back(str.substr(from, i - from));
            inside = false;
        }
    }
    ret.push_back(str.substr(from));

    return ret;
}

Vector<StringView> StringUtils::split(StringView str, char p_splitter, bool p_allow_empty) {
    Vector<StringView> ret;
    String::split_ref(ret, str, p_splitter, p_allow_empty);
    return ret;
}

Vector<StringView> StringUtils::split(StringView str, StringView p_splitter, bool p_allow_empty, int p_maxsplit) {
    Vector<StringView> ret;
    size_t from = 0;
    size_t len = str.length();

    while (true) {

        size_t end = StringUtils::find(str, p_splitter, from);
        if (end == String::npos) {
            end = len;
        }
        if (p_allow_empty || (end > from)) {
            if (p_maxsplit <= 0) {
                ret.push_back(substr(str, from, end - from));
            } else {
                // Put rest of the string and leave cycle.
                if (p_maxsplit == ret.size()) {
                    ret.push_back(substr(str, from));
                    break;
                }

                // Otherwise, push items until positive limit is reached.
                ret.push_back(substr(str, from, end - from));
            }
        }

        if (end == len) {
            break;
        }

        from = end + p_splitter.length();
    }

    return ret;
}

Vector<StringView> StringUtils::rsplit(StringView str, StringView p_splitter, bool p_allow_empty, int p_maxsplit) {

    Vector<StringView> ret;
    const size_t len = str.length();
    const size_t split_len = p_splitter.size();
    size_t remaining_len = len;

    while (true) {

        if (remaining_len < split_len || (p_maxsplit > 0 && p_maxsplit == ret.size())) {
            // no room for another splitter or hit max splits, push what's left and we're done
            if (p_allow_empty || remaining_len > 0) {
                ret.push_back(substr(str, 0, remaining_len));
            }
            break;
        }

        auto left_edge = rfind(str, p_splitter, remaining_len - split_len);

        if (left_edge == String::npos) {
            // no more splitters, we're done
            ret.push_back(substr(str, 0, remaining_len));
            break;
        }

        size_t substr_start = left_edge + split_len;
        if (p_allow_empty || substr_start < remaining_len) {
            ret.push_back(substr(str, substr_start, remaining_len - substr_start));
        }

        remaining_len = left_edge;
    }

    reverse(ret.begin(), ret.end());

    return ret;
}

Vector<float> StringUtils::split_floats(StringView str, StringView p_splitter, bool p_allow_empty) {

    Vector<float> ret;
    int from = 0;
    int len = str.length();

    while (true) {

        int end = StringUtils::find(str, p_splitter, from);
        if (end < 0) {
            end = len;
        }
        if (p_allow_empty || (end > from)) {
            ret.push_back(to_double(str.data() + from));
        }

        if (end == len) {
            break;
        }

        from = end + p_splitter.length();
    }

    return ret;
}

Vector<float> StringUtils::split_floats_mk(StringView str, StringView p_splitters, bool p_allow_empty) {

    Vector<float> ret;
    size_t from = 0;
    size_t len = str.length();
    ret.reserve(str.size() / 8); // just a ballpark to reduce number of reallocations.
    while (true) {

        auto end = str.find_first_of(p_splitters, from);
        if (end == String::npos) {
            end = len;
        }

        if (p_allow_empty || (end > from)) {
            ret.push_back(to_double(str.substr(from, end - from)));
        }

        if (end == len) {
            break;
        }

        from = end + 1;
    }

    return ret;
}

char StringUtils::char_lowercase(char p_char) {
    return CharToLower(p_char);
}

char StringUtils::char_uppercase(char p_char) {
    return CharToUpper(p_char);
}

String StringUtils::to_upper(StringView str) {
    String res(str);
    res.make_upper();
    return res;
}

String StringUtils::to_lower(StringView str) {
    String res(str);
    res.make_lower();
    return res;
}

String StringUtils::md5(const uint8_t *p_md5) {
    return hex_encode_buffer(p_md5, 16);
}

String StringUtils::hex_encode_buffer(const uint8_t *p_buffer, int p_len) {
    static const char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    String ret;
    ret.reserve(p_len * 2);

    for (int i = 0; i < p_len; i++) {
        ret.push_back(hex[p_buffer[i] >> 4]);
        ret.push_back(hex[p_buffer[i] & 0xF]);
    }

    return ret;
}

String StringUtils::num(double p_num, int p_decimals, bool trailing_zeroes) {
    if (p_num == (double) (int64_t) p_num) {
        if (trailing_zeroes) {
            return num_int64((int64_t) p_num) + ".0";
        } else {
            return num_int64((int64_t) p_num);
        }
    }
    if (p_decimals > 16) {
        p_decimals = 16;
    }

    char fmt[7];
    fmt[0] = '%';
    fmt[1] = '.';

    if (p_decimals < 0) {

        fmt[1] = 'l';
        fmt[2] = 'f';
        fmt[3] = 0;

    } else if (p_decimals < 10) {
        fmt[2] = '0' + p_decimals;
        fmt[3] = 'l';
        fmt[4] = 'f';
        fmt[5] = 0;
    } else {
        fmt[2] = '0' + (p_decimals / 10);
        fmt[3] = '0' + (p_decimals % 10);
        fmt[4] = 'l';
        fmt[5] = 'f';
        fmt[6] = 0;
    }
    char buf[256];

#if defined(__GNUC__) || defined(_MSC_VER)
    snprintf(buf, 256, fmt, p_num);
#else
    sprintf(buf, fmt, p_num);
#endif

    buf[255] = 0;
    //destroy trailing zeroes
    {

        bool period = false;
        int z = 0;
        while (buf[z]) {
            if (buf[z] == '.') {
                period = true;
            }
            z++;
        }

        if (period) {
            z--;
            while (z > 0) {

                if (buf[z] == '0') {

                    buf[z] = 0;
                } else if (buf[z] == '.') {

                    buf[z] = 0;
                    break;
                } else {

                    break;
                }

                z--;
            }
        }
    }

    return buf;
}

String StringUtils::num_int64(int64_t p_num, int base, bool capitalize_hex) {

    bool sign = p_num < 0;

    int64_t n = p_num;

    int chars = 0;
    do {
        n /= base;
        chars++;
    } while (n);

    if (sign) {
        chars++;
    }
    String s;
    s.resize(chars);
    char *c = s.data();

    n = p_num;
    do {
        int mod = std::abs(n % base);
        if (mod >= 10) {
            char a = (capitalize_hex ? 'A' : 'a');
            c[--chars] = a + (mod - 10);
        } else {
            c[--chars] = '0' + mod;
        }

        n /= base;
    } while (n);

    if (sign) {
        c[0] = '-';
    }

    return s;
}

String StringUtils::num_uint64(uint64_t p_num, int base, bool capitalize_hex) {

    uint64_t n = p_num;

    int chars = 0;
    do {
        n /= base;
        chars++;
    } while (n);

    String s;
    s.resize(chars);
    char *c = s.data();
    n = p_num;
    do {
        int mod = n % base;
        if (mod >= 10) {
            char a = (capitalize_hex ? 'A' : 'a');
            c[--chars] = a + (mod - 10);
        } else {
            c[--chars] = '0' + mod;
        }

        n /= base;
    } while (n);

    return s;
}

String StringUtils::num_real(double p_num) {

    String s;
    String sd;
    /* integer part */

    bool neg = p_num < 0;
    p_num = std::abs(p_num);
    int intn = (int) p_num;

    /* decimal part */

    if ((int) p_num != p_num) {

        double dec = p_num - (float) ((int) p_num);

        int digit = 0;
        int decimals = MAX_DIGITS;

        int dec_int = 0;
        int dec_max = 0;

        while (true) {

            dec *= 10.0;
            dec_int = dec_int * 10 + (int) dec % 10;
            dec_max = dec_max * 10 + 9;
            digit++;

            if ((dec - (float) ((int) dec)) < 1e-6) {
                break;
            }

            if (digit == decimals) {
                break;
            }
        }

        dec *= 10;
        int last = (int) dec % 10;

        if (last > 5) {
            if (dec_int == dec_max) {

                dec_int = 0;
                intn++;
            } else {

                dec_int++;
            }
        }

        String decimal;
        for (int i = 0; i < digit; i++) {

            char num[2] = {0, 0};
            num[0] = '0' + dec_int % 10;
            decimal = num + decimal;
            dec_int /= 10;
        }
        sd = '.' + decimal;
    } else {
        sd = ".0";
    }

    if (intn == 0) {
        s = "0";
    } else {
        while (intn) {

            char num = '0' + (intn % 10);
            intn /= 10;
            s = num + s;
        }
    }

    s = s + sd;
    if (neg) {
        s = "-" + s;
    }
    return s;
}

String StringUtils::num_scientific(double p_num, int p_decimals) {
    char fmt[16] = {'%','g',0};
    if(p_decimals>=0) {
        snprintf(fmt,15,"%%0.%d%%g",p_decimals);
    }
    return String(String::CtorSprintf(), fmt, p_num);
}

// int StringUtils::hex_to_int(StringView s, bool p_with_prefix) {
//     if (p_with_prefix && s.length() < 3) {
//         return 0;
//     }
//     StringView to_convert;
//     if (p_with_prefix) {
//         if (!begins_with(s, "0x")) {
//             return 0;
//         }
//         to_convert = s.substr(2);
//     } else {
//         to_convert = s;
//     }
//     int res = 0;
//  #ifndef __MINGW32__
//      std::from_chars(to_convert.begin(), to_convert.end(), res, 16);
//  #else
//     String zeroterm(to_convert);
//     res = strtol(zeroterm.c_str(),nullptr,16);
// #endif
//     return res;
// }

// int64_t StringUtils::hex_to_int64(StringView s, bool p_with_prefix) {
//     if (p_with_prefix && s.length() < 3) {
//         return 0;
//     }
//     StringView to_convert;
//     if (p_with_prefix) {
//         if (!s.starts_with("0x")) {
//             return 0;
//         }
//         to_convert = s.substr(2);
//     } else {
//         to_convert = s;
//     }
//     int64_t v;
// #ifndef __MINGW32__
//     std::from_chars(to_convert.data(), to_convert.data() + to_convert.length(), v, 16);
// #else
//     String zeroterm(to_convert);
//     v = strtoll(zeroterm.c_str(),nullptr,16);
// #endif
//     return v;
// }

// int64_t StringUtils::bin_to_int64(StringView s, bool p_with_prefix) {
//     if (p_with_prefix && s.length() < 3) {
//         return 0;
//     }
//     StringView to_convert;
//     if (p_with_prefix) {
//         if (!s.starts_with("0b")) {
//             return 0;
//         }
//         to_convert = s.substr(2);
//     } else {
//         to_convert = s;
//     }
//     int64_t v;
//  #ifndef __MINGW32__
//      std::from_chars(to_convert.data(), to_convert.data() + to_convert.length(), v, 2);
//  #else
//     String zeroterm(to_convert);
//     v = strtoll(zeroterm.c_str(),nullptr,2);
// #endif
//     return v;
// }

// int64_t StringUtils::to_int64(StringView s) {
//     String tmp(s);
//     return strtoll(tmp.c_str(), nullptr, 0);
// }

// int StringUtils::to_int(const char *p_str, int p_len) {
//     int res_val=0;
//     auto conv_res=std::from_chars(p_str, p_str + p_len, res_val);
//     if(conv_res.ec==std::errc::invalid_argument)
//         return 0;
//     return res_val;
// }

int StringUtils::to_int(StringView p_str, bool *ok) {
    int res_val=0;
    auto conv_res=std::from_chars(p_str.data(), p_str.data() + p_str.length(), res_val);
    if(conv_res.ec==std::errc::invalid_argument)
    {
        if(ok)
            *ok=false;
        return 0;
    }
    if(ok)
        *ok=true;
    return res_val;
}

bool StringUtils::is_numeric(StringView str) {

    if (str.length() == 0) {
        return false;
    }

    size_t s = 0;
    if (str[0] == '-') {
        ++s;
    }
    bool dot = false;
    for (size_t i = s; i < str.length(); i++) {

        char c = str[i];
        if (c == '.') {
            if (dot) {
                return false;
            }
            dot = true;
        }
        if (c < '0' || c > '9') {
            return false;
        }
    }

    return true;
}

double StringUtils::to_double(const char *p_str, char **r_end) {
    return strtod(p_str, r_end);
}

/*
String StringUtils::md5_text(StringView str) {
    unsigned char hash[16];
    CryptoCore::md5((unsigned char *) str.data(), str.length(), hash);
    return hex_encode_buffer(hash, 16);
}

String StringUtils::sha1_text(StringView str) {
    unsigned char hash[20];
    CryptoCore::sha1((unsigned char *) str.data(), str.length(), hash);
    return hex_encode_buffer(hash, 20);
}
*/
String StringUtils::sha256_text(StringView cs) {
    unsigned char hash[32];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx,(unsigned char *) cs.data(), cs.length());
    sha256_final(&ctx,hash);
    //CryptoCore::sha256((unsigned char *) cs.data(), cs.length(), hash);
    return hex_encode_buffer(hash, 32);
}
/*
FixedVector<uint8_t, 16, false> StringUtils::md5_buffer(StringView cs) {

    unsigned char hash[16];
    CryptoCore::md5((unsigned char *) cs.data(), cs.length(), hash);

    FixedVector<uint8_t, 16, false> ret;
    ret.resize(16);
    for (int i = 0; i < 16; i++) {
        ret[i] = hash[i];
    }
    return ret;
}
*/

/*Vector<uint8_t> StringUtils::sha1_buffer(StringView cs) {
    Vector<uint8_t> ret(20, 0);
    CryptoCore::sha1((unsigned char *) cs.data(), cs.length(), ret.data());
    return ret;
}


}*/

Vector<uint8_t> StringUtils::sha256_buffer(StringView str) {
    Vector<uint8_t> ret(32, 0);
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx,(unsigned char *) str.data(), str.length());
    sha256_final(&ctx,ret.data());
 //CryptoCore::sha256((unsigned char *) str.data(), str.length(), ret.data());
    return ret;
}

String StringUtils::insert(StringView s, int p_at_pos, StringView p_string) {
    String res(s);
    res.insert(p_at_pos, String(p_string));
    return res;
}

StringView StringUtils::substr(StringView s, int p_from, size_t p_chars) {
    StringView res(s);
    if (s.empty()) {
        return res;
    }
    auto count = static_cast<ssize_t>(p_chars);
    if ((p_from + count) > ssize_t(s.length())) {

        p_chars = s.length() - p_from;
    }

    return res.substr(p_from, p_chars);
}

size_t StringUtils::find(StringView s, StringView p_str, size_t p_from) {

    return s.find(p_str, p_from);
}

size_t StringUtils::find(StringView s, char c, size_t p_from) {
    return s.find(c, p_from);
}

size_t StringUtils::findn(StringView s, StringView p_str, int p_from) {
    if (p_from < 0) {
        return String::npos;
    }

    size_t src_len = p_str.length();

    if (src_len == 0 || s.empty() || src_len > s.length()) {
        return String::npos; // won't find anything!
    }

    const char *srcd = s.data();

    for (size_t i = p_from; i <= (s.length() - src_len); i++) {

        bool found = true;
        for (size_t j = 0; j < src_len; j++) {

            size_t read_pos = i + j;

            if (read_pos >= s.length()) {

                //ERR_PRINT("read_pos>=length()");
                return String::npos;
            }

            char src = CharToLower(srcd[read_pos]);
            char dst = CharToLower(p_str[j]);

            if (src != dst) {
                found = false;
                break;
            }
        }

        if (found) {
            return i;
        }
    }

    return String::npos;
}

size_t StringUtils::rfind(StringView s, StringView p_str, int p_from) {
    return s.rfind(p_str, p_from);
}

size_t StringUtils::rfind(StringView s, char c, int p_from) {
    return s.rfind(c, p_from);
}

bool StringUtils::ends_with(StringView s, StringView p_string) {
    return s.ends_with(p_string);
}

bool StringUtils::ends_with(StringView s, StringView p_string, Compare mode) {
    if (mode == Compare::CaseInsensitive) {
        auto size1 = p_string.size();
        auto size2 = s.size();
        if (size2 < size1) {
            return false;
        }
        for (size_t i = 0; i < size1; i++) {
            if (CharToLower(s[size2 - size1 + i]) != CharToLower(p_string[i])) {
                return false;
            }
        }

    }
    return s.ends_with(p_string);
}


bool StringUtils::ends_with(StringView s, char c) {
    return s.ends_with(c);
}

bool StringUtils::begins_with(StringView s, StringView p_string) {
    if (s.size() < p_string.size()) {
        return false;
    }
    return String::compare(s.begin(), s.begin() + p_string.size(), p_string.begin(), p_string.end()) == 0;
}

bool StringUtils::begins_with(StringView s, StringView p_string, Compare mode) {
    if (s.size() < p_string.size()) {
        return false;
    }
    return StringUtils::compare(s.substr(0, p_string.size()), p_string,mode) == 0;
}

bool StringUtils::is_subsequence_of(StringView str, StringView p_string, Compare mode) {
    if (str.empty()) {
        return true;
    }
    if (str.length() > p_string.length()) {
        return false;
    }
    return str.contains(p_string, mode == CaseSensitive);
}

bool StringUtils::is_quoted(StringView str) {

    return is_enclosed_in(str, '"') || is_enclosed_in(str, '\'');
}

static int str_count(StringView s, StringView p_string, int p_from, int p_to, bool p_case_insensitive) {
    if (p_string.empty()) {
        return 0;
    }
    size_t len = s.length();
    size_t slen = p_string.length();
    if (len < slen) {
        return 0;
    }
    StringView str;
    if (p_from >= 0 && p_to >= 0) {
        if (p_to == 0) {
            p_to = len;
        } else if (p_from >= p_to) {
            return 0;
        }
        if (p_from == 0 && p_to == len) {
            str = s.substr(0, len);
        } else {
            str = s.substr(p_from, p_to - p_from);
        }
    } else {
        return 0;
    }
    int c = 0;
    auto idx = String::npos;
    do {
        idx = p_case_insensitive ? StringUtils::findn(str, p_string) : StringUtils::find(str, p_string);
        if (idx != String::npos) {
            str = StringUtils::substr(str, idx + slen, str.length() - slen);
            ++c;
        }
    } while (idx != String::npos);
    return c;
}

int StringUtils::count(StringView s, StringView p_string, int p_from, int p_to) {
    return str_count(s, p_string, p_from, p_to, false);
}

int StringUtils::countn(StringView s, StringView p_string, int p_from, int p_to) {

    return str_count(s, p_string, p_from, p_to, true);
}

Vector<StringView> StringUtils::bigrams(StringView str) {
    int n_pairs = str.length() - 1;
    Vector<StringView> b;
    if (n_pairs <= 0) {
        return b;
    }
    b.resize(n_pairs);
    for (int i = 0; i < n_pairs; i++) {
        b[i] = substr(str, i, 2);
    }
    return b;
}

// Similarity according to Sorensen-Dice coefficient
float StringUtils::similarity(StringView lhs, StringView p_string) {
    if (lhs == p_string) {
        // Equal strings are totally similar
        return 1.0f;
    }
    if (lhs.length() < 2 || p_string.length() < 2) {
        // No way to calculate similarity without a single bigram
        return 0.0f;
    }

    Vector<StringView> src_bigrams = bigrams(lhs);
    Vector<StringView> tgt_bigrams = bigrams(p_string);

    int src_size = src_bigrams.size();
    int tgt_size = tgt_bigrams.size();

    float sum = src_size + tgt_size;
    float inter = 0;
    for (int i = 0; i < src_size; i++) {
        for (int j = 0; j < tgt_size; j++) {
            if (src_bigrams[i] == tgt_bigrams[j]) {
                inter++;
                break;
            }
        }
    }

    return (2.0f * inter) / sum;
}

static bool _wildcard_match(StringView p_pattern, StringView p_string, bool p_case_sensitive) {
    if (p_pattern.empty() && p_string.empty()) {
        return true;
    }
    switch (p_pattern[0]) {
        case '*':
            return _wildcard_match(p_pattern.substr(1), p_string, p_case_sensitive) ||
                   (!p_string.empty() && _wildcard_match(p_pattern, p_string.substr(1), p_case_sensitive));
        case '?':
            return !p_string.empty() && (p_string[0] != '.') &&
                   _wildcard_match(p_pattern.substr(1), p_string.substr(1), p_case_sensitive);
        default:

            return (p_case_sensitive ? (p_string[0] == p_pattern[0]) : (CharToUpper(p_string[0]) ==
                                                                        CharToUpper(p_pattern[0]))) &&
                   _wildcard_match(p_pattern.substr(1), p_string.substr(1), p_case_sensitive);
    }
}

bool StringUtils::match(StringView s, StringView p_wildcard, Compare sensitivity) {
    if (p_wildcard.empty() || s.empty()) {
        return false;
    }

    return _wildcard_match(p_wildcard, s, sensitivity == CaseSensitive);
}

bool StringUtils::matchn(StringView s, StringView p_wildcard) {
    return match(s, p_wildcard, CaseInsensitive);
}

String StringUtils::replace_first(StringView s, StringView p_key, StringView p_with) {

    auto pos = find(s, p_key);
    String res(s);
    if (pos == String::npos) {
        return res;
    }
    res.replace(pos, p_key.length(), p_with.data(), p_with.size());
    return res;
}

String StringUtils::replace(StringView str, StringView p_key, StringView p_with) {
    return String(str).replaced(p_key, p_with);
}

String StringUtils::replace(StringView str, char p_key, char p_with) {
    return String(str).replaced(p_key, p_with);
}

String StringUtils::repeat(StringView str, int p_count) {

    assert(p_count > 0 && "Parameter count should be a positive number.");

    String new_string;

    new_string.reserve(p_count * str.size());
    for (int i = 0; i < p_count; ++i) {
        new_string.append(str);
    }

    return new_string;
}

StringView StringUtils::left(StringView s, int p_pos) {
    if (p_pos < 0) {
        return StringView();
    }
    return StringView(s).substr(0, p_pos);
}

StringView StringUtils::right(StringView s, int p_pos) {
    if (p_pos >= s.size()) {
        return StringView();
    }
    return s.substr(p_pos);
}

String StringUtils::dedent(StringView str) {

    String new_string;
    String indent;
    bool has_indent = false;
    bool has_text = false;
    size_t line_start = 0;
    int indent_stop = -1;

    for (size_t i = 0; i < str.length(); i++) {

        char c = str[i];
        if (c == '\n') {
            if (has_text) {
                new_string += substr(str, indent_stop, i - indent_stop);
            }
            new_string += '\n';
            has_text = false;
            line_start = i + 1;
            indent_stop = -1;
        } else if (!has_text) {
            if (c > 32) {
                has_text = true;
                if (!has_indent) {
                    has_indent = true;
                    indent = substr(str, line_start, i - line_start);
                    indent_stop = i;
                }
            }
            if (has_indent && indent_stop < 0) {
                int j = i - line_start;
                if (j >= indent.length() || c != indent[j]) {
                    indent_stop = i;
                }
            }
        }
    }

    if (has_text) {
        new_string += substr(str, indent_stop);
    }

    return new_string;
}

StringView StringUtils::strip_edges(StringView str, bool left, bool right) {

    int len = str.length();
    int beg = 0, end = len;

    if (left) {
        for (int i = 0; i < len; i++) {
            if (str[i] <= 32) {
                beg++;
            } else {
                break;
            }
        }
    }

    if (right) {
        for (int i = (int) (len - 1); i >= 0; i--) {
            if (str[i] <= 32) {
                end--;
            } else {
                break;
            }
        }
    }

    if (beg == 0 && end == len) {
        return str;
    }

    return substr(str, beg, end - beg);
}

String StringUtils::strip_escapes(StringView str) {

    String new_string;
    for (size_t i = 0; i < str.length(); i++) {

        // Escape characters on first page of the ASCII table, before 32 (Space).
        if (str[i] < 32) {
            continue;
        }
        new_string += str[i];
    }

    return new_string;
}

StringView StringUtils::lstrip(StringView str, StringView p_chars) {

    size_t len = str.length();
    size_t beg;

    for (beg = 0; beg < len; beg++) {
        if (find_char(p_chars, str[beg]) == String::npos) {
            break;
        }
    }

    if (beg == 0) {
        return str;
    }

    return substr(str, beg, len - beg);
}

StringView StringUtils::rstrip(StringView str, StringView p_chars) {

    int len = str.length();
    int end;

    for (end = len - 1; end >= 0; end--) {
        if (find_char(p_chars, str[end]) == StringView::npos) {
            break;
        }
    }

    if (end == len - 1) {
        return str;
    }

    return substr(str, 0, end + 1);
}

bool PathUtils::is_network_share_path(StringView path) {
    return path.starts_with("//") || path.starts_with("\\\\");
}

String PathUtils::simplify_path(StringView str) {

    String s(str);

    s.replace('\\', '/');
    while (true) { // in case of using 2 or more slash
        String compare = s.replaced("//", "/");
        if (s == compare) {
            break;
        } else {
            s = compare;
        }
    }
    FixedVector<StringView, 16, true> dirs;
    FixedVector<StringView, 16, true> filtered;
    String::split_ref(dirs, s, '/');

    for (StringView d: dirs) {
        if (d == "."_sv) {
            continue;
        } else if (d == ".."_sv) {
            if (filtered.empty()) {
                continue;
            }
            filtered.pop_back(); // remove pre
        } else {
            filtered.push_back(d);
        }
    }

    return String::joined(filtered, "/");
}

static int _humanize_digits(int p_num) {
    if (p_num < 100) {
        return 2;
    } else if (p_num < 1024) {
        return 1;
    } else {
        return 0;
    }
}

bool PathUtils::is_abs_path(StringView str) {
    if (str.length() > 1) {
        return (str[0] == '/' || str[0] == '\\' || str.contains(":/") || str.contains(":\\"));
    } else if (str.length() == 1) {
        return (str[0] == '/' || str[0] == '\\');
    } else {
        return false;
    }
}

String StringUtils::http_escape(StringView temp) {
    String res;
    for (char ord: temp) {
        if (ord == '.' || ord == '-' || ord == '_' || ord == '~' ||
            (ord >= 'a' && ord <= 'z') ||
            (ord >= 'A' && ord <= 'Z') ||
            (ord >= '0' && ord <= '9')) {
            res += ord;
        } else {
            char h_Val[3];
#if defined(__GNUC__) || defined(_MSC_VER)
            snprintf(h_Val, 3, "%hhX", ord);
#else
            sprintf(h_Val, "%hhX", ord);
#endif
            res += "%";
            res += h_Val;
        }
    }
    return res;
}

// String StringUtils::http_unescape(StringView str) {
//     String res;
//     for (size_t i = 0; i < str.length(); ++i) {
//         if (str.at(i) == '%' && i + 2 < str.length()) {
//             char ord1 = str.at(i + 1);
//             if ((ord1 >= '0' && ord1 <= '9') || (ord1 >= 'A' && ord1 <= 'Z')) {
//                 char ord2 = str.at(i + 2);
//                 if ((ord2 >= '0' && ord2 <= '9') || (ord2 >= 'A' && ord2 <= 'Z')) {
//                     char bytes[3] = {(char) ord1, (char) ord2, 0};
//                     res += (char) strtol(bytes, nullptr, 16);
//                     i += 2;
//                 }
//             } else {
//                 res += str.at(i);
//             }
//         } else {
//             res += str.at(i);
//         }
//     }
//     return res;
// }

String StringUtils::c_unescape(StringView str) {

    String escaped(str);
    escaped = StringUtils::replace(escaped, "\\a", "\a");
    escaped = StringUtils::replace(escaped, "\\b", "\b");
    escaped = StringUtils::replace(escaped, "\\f", "\f");
    escaped = StringUtils::replace(escaped, "\\n", "\n");
    escaped = StringUtils::replace(escaped, "\\r", "\r");
    escaped = StringUtils::replace(escaped, "\\t", "\t");
    escaped = StringUtils::replace(escaped, "\\v", "\v");
    escaped = StringUtils::replace(escaped, "\\'", "\'");
    escaped = StringUtils::replace(escaped, "\\\"", "\"");
    escaped = StringUtils::replace(escaped, "\\?", "\?");
    escaped = StringUtils::replace(escaped, "\\\\", "\\");

    return escaped;
}
//String StringUtils::c_escape(const String &e) {

//    String escaped = e;
//    escaped = escaped.replace("\\", "\\\\");
//    escaped = escaped.replace("\a", "\\a");
//    escaped = escaped.replace("\b", "\\b");
//    escaped = escaped.replace("\f", "\\f");
//    escaped = escaped.replace("\n", "\\n");
//    escaped = escaped.replace("\r", "\\r");
//    escaped = escaped.replace("\t", "\\t");
//    escaped = escaped.replace("\v", "\\v");
//    escaped = escaped.replace("\'", "\\'");
//    escaped = escaped.replace("\?", "\\?");
//    escaped = escaped.replace("\"", "\\\"");

//    return escaped;
//}
String StringUtils::c_escape(StringView e) {

    String escaped(e);
    escaped = StringUtils::replace(escaped, "\\", "\\\\");
    escaped = StringUtils::replace(escaped, "\a", "\\a");
    escaped = StringUtils::replace(escaped, "\b", "\\b");
    escaped = StringUtils::replace(escaped, "\f", "\\f");
    escaped = StringUtils::replace(escaped, "\n", "\\n");
    escaped = StringUtils::replace(escaped, "\r", "\\r");
    escaped = StringUtils::replace(escaped, "\t", "\\t");
    escaped = StringUtils::replace(escaped, "\v", "\\v");
    escaped = StringUtils::replace(escaped, "\'", "\\'");
    escaped = StringUtils::replace(escaped, "\?", "\\?");
    escaped = StringUtils::replace(escaped, "\"", "\\\"");

    return escaped;
}

//String StringUtils::c_escape_multiline(const String &str) {

//    String escaped = str;
//    escaped = escaped.replace("\\", "\\\\");
//    escaped = escaped.replace("\"", "\\\"");

//    return escaped;
//}
String StringUtils::c_escape_multiline(StringView str) {

    String escaped(str);
    escaped.replace("\\", "\\\\");
    escaped.replace("\"", "\\\"");

    return escaped;
}
//String StringUtils::json_escape(const String &str) {

//    String escaped = str;
//    escaped = escaped.replace("\\", "\\\\");
//    escaped = escaped.replace("\b", "\\b");
//    escaped = escaped.replace("\f", "\\f");
//    escaped = escaped.replace("\n", "\\n");
//    escaped = escaped.replace("\r", "\\r");
//    escaped = escaped.replace("\t", "\\t");
//    escaped = escaped.replace("\v", "\\v");
//    escaped = escaped.replace("\"", "\\\"");

//    return escaped;
//}
String StringUtils::json_escape(StringView str) {

    String escaped(str);
    escaped.replace("\\", "\\\\");
    escaped.replace("\b", "\\b");
    escaped.replace("\f", "\\f");
    escaped.replace("\n", "\\n");
    escaped.replace("\r", "\\r");
    escaped.replace("\t", "\\t");
    escaped.replace("\v", "\\v");
    escaped.replace("\"", "\\\"");

    return escaped;
}

String StringUtils::xml_escape(StringView arg, bool p_escape_quotes) {

    String str(arg);
    str = StringUtils::replace(str, "&", "&amp;");
    str = StringUtils::replace(str, "<", "&lt;");
    str = StringUtils::replace(str, ">", "&gt;");
    if (p_escape_quotes) {
        str = StringUtils::replace(str, "'", "&apos;");
        str = StringUtils::replace(str, "\"", "&quot;");
    }
    return str;
}

static int _xml_unescape(const char *p_src, int p_src_len, char *p_dst) {

    int len = 0;
    while (p_src_len) {

        if (*p_src == '&') {

            int eat = 0;

            if (p_src_len >= 4 && p_src[1] == '#') {

                char c = 0;

                for (int i = 2; i < p_src_len; i++) {

                    eat = i + 1;
                    char ct = p_src[i];
                    int ct_v;
                    if (ct == ';') {
                        break;
                    } else if (ct >= '0' && ct <= '9') {
                        ct_v = ct - '0';
                    } else if (ct >= 'a' && ct <= 'f') {
                        ct_v = (ct - 'a') + 10;
                    } else if (ct >= 'A' && ct <= 'F') {
                        ct_v = (ct - 'A') + 10;
                    } else {
                        continue;
                    }
                    c <<= 4;
                    c |= ct_v;
                }

                if (p_dst) {
                    *p_dst = c;
                }

            } else if (p_src_len >= 4 && p_src[1] == 'g' && p_src[2] == 't' && p_src[3] == ';') {
                if (p_dst) {
                    *p_dst = '>';
                }
                eat = 4;
            } else if (p_src_len >= 4 && p_src[1] == 'l' && p_src[2] == 't' && p_src[3] == ';') {
                if (p_dst) {
                    *p_dst = '<';
                }
                eat = 4;
            } else if (p_src_len >= 5 && p_src[1] == 'a' && p_src[2] == 'm' && p_src[3] == 'p' && p_src[4] == ';') {
                if (p_dst) {
                    *p_dst = '&';
                }
                eat = 5;
            } else if (p_src_len >= 6 && p_src[1] == 'q' && p_src[2] == 'u' && p_src[3] == 'o' && p_src[4] == 't' &&
                       p_src[5] == ';') {
                if (p_dst) {
                    *p_dst = '"';
                }
                eat = 6;
            } else if (p_src_len >= 6 && p_src[1] == 'a' && p_src[2] == 'p' && p_src[3] == 'o' && p_src[4] == 's' &&
                       p_src[5] == ';') {
                if (p_dst) {
                    *p_dst = '\'';
                }
                eat = 6;
            } else {
                if (p_dst) {
                    *p_dst = *p_src;
                }
                eat = 1;
            }

            if (p_dst) {
                p_dst++;
            }

            len++;
            p_src += eat;
            p_src_len -= eat;
        } else {

            if (p_dst) {
                *p_dst = *p_src;
                p_dst++;
            }
            len++;
            p_src++;
            p_src_len--;
        }
    }

    return len;
}

String StringUtils::xml_unescape(StringView arg) {

    String str;
    int l = arg.length();
    int len = _xml_unescape(arg.data(), l, nullptr);
    if (len == 0) {
        return String();
    }
    str.resize(len);
    _xml_unescape(arg.data(), l, str.data());

    return str;
}

String StringUtils::pad_decimals(StringView str, int p_digits) {

    String s(str);
    auto c = s.find('.');

    if (c == String::npos) {
        if (p_digits <= 0) {
            return s;
        }
        s += '.';
        c = s.length() - 1;
    } else {
        if (p_digits <= 0) {
            return String(substr(s, 0, c));
        }
    }

    if (s.length() - (c + 1) > p_digits) {
        s = substr(s, 0, c + p_digits + 1);
    } else {
        while (s.length() - (c + 1) < p_digits) {
            s += '0';
        }
    }
    return s;
}

String StringUtils::pad_zeros(StringView src, int p_digits) {

    String s(src);
    auto end = s.find('.');

    if (end == String::npos) {
        end = s.length();
    }

    if (end == 0) {
        return s;
    }

    size_t begin = 0;

    while (begin < end && (s[begin] < '0' || s[begin] > '9')) {
        begin++;
    }

    if (begin >= end) {
        return s;
    }

    while (end - begin < size_t(p_digits)) {

        s = s.insert(begin, "0");
        end++;
    }

    return s;
}

StringView StringUtils::trim_prefix(StringView src, StringView p_prefix) {

    StringView s = src;
    if (begins_with(s, p_prefix)) {
        return substr(s, p_prefix.length(), s.length() - p_prefix.length());
    }
    return s;
}

StringView StringUtils::trim_suffix(StringView src, StringView p_suffix) {

    StringView s = src;
    if (ends_with(s, p_suffix)) {
        return substr(s, 0, s.length() - p_suffix.length());
    }
    return s;
}

bool StringUtils::is_valid_integer(StringView str) {

    int len = str.length();

    if (len == 0) {
        return false;
    }

    int from = 0;
    if (len != 1 && (str[0] == '+' || str[0] == '-')) {
        from++;
    }

    for (int i = from; i < len; i++) {
        if (isdigit(str[i]) == 0) {
            return false; // no start with number plz
        }
    }

    return true;
}

bool StringUtils::is_valid_hex_number(StringView str, bool p_with_prefix) {

    size_t len = str.length();

    if (len == 0) {
        return false;
    }

    size_t from = 0;
    if (len != 1 && (str.front() == '+' || str.front() == '-')) {
        from++;
    }

    if (p_with_prefix) {
        if (len < 3) {
            return false;
        }
        if (str[from] != '0' || str[from + 1] != 'x') {
            return false;
        }
        from += 2;
    }

    for (size_t i = from; i < len; i++) {

        char c = str[i];
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
            continue;
        }
        return false;
    }

    return true;
}

bool StringUtils::is_valid_float(StringView str) {

    size_t len = str.length();

    if (len == 0) {
        return false;
    }

    size_t from = 0;
    if (str.front() == '+' || str.front() == '-') {
        from++;
    }

    bool exponent_found = false;
    bool period_found = false;
    bool sign_found = false;
    bool exponent_values_found = false;
    bool numbers_found = false;

    for (size_t i = from; i < len; i++) {

        if (isdigit(str[i])) {
            if (exponent_found) {
                exponent_values_found = true;
            } else {
                numbers_found = true;
            }
        } else if (numbers_found && !exponent_found && str[i] == 'e') {
            exponent_found = true;
        } else if (!period_found && !exponent_found && str[i] == '.') {
            period_found = true;
        } else if ((str[i] == '-' || str[i] == '+') && exponent_found && !exponent_values_found && !sign_found) {
            sign_found = true;
        } else {
            return false; // no start with number plz
        }
    }

    return numbers_found;
}

bool StringUtils::is_valid_int(StringView str) {
    int len = str.length();

    if (len == 0) {
        return false;
    }

    int from = 0;
    if (len != 1 && (str[0] == '+' || str[0] == '-')) {
        from++;
    }

    for (int i = from; i < len; i++) {
        if (!CharUtils::is_digit(str[i])) {
            return false; // no start with number plz
        }
    }

    return true;
}

String PathUtils::path_to_file(StringView base, StringView p_path) {
    // Don't get base dir for src, this is expected to be a dir already.
    String src = from_native_path(base);
    String dst = get_base_dir(from_native_path(p_path));
    String rel = path_to(src, dst);
    if (rel == dst) { // failed
        return String(p_path);
    }

    return String(rel) + get_file(p_path);
}

//String PathUtils::path_to(const String &str,String p_path) {

//    QString src = PathUtils::from_native_path(str);
//    QString dst = PathUtils::from_native_path(p_path);
//    if (!src.endsWith("/"))
//        src += "/";
//    if (!dst.endsWith("/"))
//        dst += "/";

//    String base;

//    if (src.startsWith("res://") && dst.startsWith("res://")) {

//        base = "res:/";
//        src.replace("res://", "/");
//        dst.replace("res://", "/");

//    } else if (src.startsWith("user://") && dst.startsWith("user://")) {

//        base = "user:/";
//        src.replace("user://", "/");
//        dst.replace("user://", "/");

//    } else if (src.startsWith("/") && dst.startsWith("/")) {

//        //nothing
//    } else {
//        //dos style
//        String src_begin = StringUtils::get_slice(src,'/', 0);
//        String dst_begin = StringUtils::get_slice(dst,'/', 0);

//        if (src_begin != dst_begin)
//            return p_path; //impossible to do this

//        base = src_begin;
//        src = src.mid(src_begin.length(), src.length());
//        dst = dst.mid(dst_begin.length(), dst.length());
//    }

//    //remove leading and trailing slash and split
//    auto src_dirs = src.mid(1, src.length() - 2).splitRef("/");
//    auto dst_dirs = dst.mid(1, dst.length() - 2).splitRef("/");

//    //find common parent
//    int common_parent = 0;

//    while (true) {
//        if (src_dirs.size() == common_parent)
//            break;
//        if (dst_dirs.size() == common_parent)
//            break;
//        if (src_dirs[common_parent] != dst_dirs[common_parent])
//            break;
//        common_parent++;
//    }

//    common_parent--;

//    QString dir;

//    for (int i = src_dirs.size() - 1; i > common_parent; i--) {

//        dir += "../";
//    }

//    for (int i = common_parent + 1; i < dst_dirs.size(); i++) {

//        dir += dst_dirs[i] + "/";
//    }

//    if (dir.length() == 0)
//        dir = "./";
//    return String(dir);
//}
String PathUtils::path_to(StringView str, StringView p_path) {

    String src = from_native_path(str);
    String dst = from_native_path(p_path);
    // Remove trailing separators except for root
    if (src.length() > 1 && src.ends_with("/")) {
        src = src.substr(0, src.length() - 1);
    }
    if (dst.length() > 1 && dst.ends_with("/")) {
        dst = dst.substr(0, dst.length() - 1);
    }

    String base;

    if (src.starts_with("/") && dst.starts_with("/")) {

        //nothing
    } else {
        //dos style
        StringView src_begin(StringUtils::get_slice(src, '/', 0));
        StringView dst_begin(StringUtils::get_slice(dst, '/', 0));

        if (src_begin != dst_begin) {
            return String(p_path); // impossible to do this
        }

        base = src_begin;
        src = src.substr(src_begin.length());
        dst = dst.substr(dst_begin.length());
    }

    //remove leading and trailing slash and split
    auto src_dirs = StringUtils::split(src.starts_with('/') ? StringView(src).substr(1):StringView(src), "/");
    auto dst_dirs = StringUtils::split(dst.starts_with('/') ? StringView(dst).substr(1):StringView(dst), "/");

    //find common parent
    size_t common_parent = 0;

    while (true) {
        if (src_dirs.size() == common_parent) {
            break;
        }
        if (dst_dirs.size() == common_parent) {
            break;
        }
        if (src_dirs[common_parent] != dst_dirs[common_parent]) {
            break;
        }
        common_parent++;
    }

    common_parent--;

    String dir;

    for (int i = src_dirs.size() - 1; i > common_parent; i--) {

        dir += "../";
    }

    Span<StringView> from_common(dst_dirs.begin()+common_parent+1,dst_dirs.end());
    if(!from_common.empty())
        dir += String::joined(from_common,"/");

    if (dir.length() == 0) {
        dir = "./";
    }
    return dir;
}

bool StringUtils::is_valid_filename(StringView str) {

    StringView stripped = strip_edges(str);
    if (str != stripped) {
        return false;
    }

    if (stripped.empty()) {
        return false;
    }
    return str.find_first_of(":/\\?*\"|%<>") == String::npos;
}

// bool StringUtils::is_valid_ip_address(StringView str) {

//     if (contains(str, ':')) {
//         FixedVector<StringView, 8, true> ip;
//         String::split_ref(ip, str, ':');
//         for (size_t i = 0; i < ip.size(); i++) {

//             StringView n = ip[i];
//             if (n.empty()) {
//                 continue;
//             }
//             if (is_valid_hex_number(n, false)) {
//                 int nint = hex_to_int(n, false);
//                 if (nint < 0 || nint > 0xffff) {
//                     return false;
//                 }
//                 continue;
//             }
//             if (!is_valid_ip_address(n)) {
//                 return false;
//             }
//         }

//     } else {
//         FixedVector<StringView, 4, false> ip;
//         String::split_ref(ip, str, '.');
//         if (ip.size() != 4) {
//             return false;
//         }
//         for (size_t i = 0; i < ip.size(); i++) {

//             StringView n = ip[i];
//             if (!is_valid_integer(n)) {
//                 return false;
//             }
//             int val = to_int(n);
//             if (val < 0 || val > 255) {
//                 return false;
//             }
//         }
//     }

//     return true;
// }

bool PathUtils::is_rel_path(StringView str) {

    return !is_abs_path(str);
}

StringView PathUtils::trim_trailing_slash(StringView path) {
    char last_char = path.back();
    if (last_char == '/' || last_char == '\\') {
        return StringUtils::substr(path, 0, path.size() - 1);
    }
    return path;
}

String PathUtils::get_base_dir(StringView path) {

    StringView rs;
    StringView base;
    if (path.starts_with('/')) {
        rs = StringUtils::substr(path, 1);
        base = "/";
    } else {

        rs = path;
    }

    auto parent_path = PathUtils::path(rs);
    if (parent_path == StringView(".")) {
        return String(base);
    }
    return String(base) + parent_path;
}

StringView PathUtils::get_file(StringView path) {
    auto pos = path.find_last_of("/\\");
    if (pos == String::npos) {
        return path;
    }
    return path.substr(pos + 1);
}

StringView PathUtils::get_extension(StringView path) {
    auto pos = path.rfind(".");
    if (pos == String::npos) {
        return StringView();
    }
    auto sep = path.find_last_of("/\\");
    if (sep != String::npos && pos < sep) {
        return StringView();
    }

    return StringUtils::substr(path, pos + 1);
}

String PathUtils::plus_file(StringView bp, StringView p_file) {
    if (bp.empty()) {
        return String(p_file);
    }
    if (p_file.empty()) {
        return String(bp) + "/";
    }
    if (bp.back() == '/' || p_file.front() == '/') {
        return String(bp) + p_file;
    }
    return String(bp) + "/" + p_file;
}

String PathUtils::join_path(Span<StringView> parts) {
    if (parts.empty()) {
        return String();
    }
    size_t needed_memory = 0;
    for (StringView v: parts) {
        needed_memory += v.size() + 1;
    }
    String res;
    res.reserve(needed_memory);
    for (StringView v: parts) {
        if (!res.empty() && res.back() != '/') {
            res.push_back('/');
        }
        res.append(v);
    }
    return res;
}

String PathUtils::join_path(std::initializer_list<StringView> parts) {
    size_t needed_memory = 0;
    for (StringView v: parts) {
        needed_memory += v.size() + 1;
    }
    if (needed_memory) {
        return {};
    }

    String res;
    res.reserve(needed_memory);
    for (StringView v: parts) {
        if (!res.empty() && res.back() != '/') {
            res.push_back('/');
        }
        res.append(v);
    }
    return res;
}

String StringUtils::percent_encode(StringView cs) {

    String encoded;
    for (size_t i = 0; i < cs.length(); i++) {
        char c = cs[i];
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_' ||
            c == '~' || c == '.') {
            encoded += c;
        } else {
            char p[4] = {'%', 0, 0, 0};
            static const char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e',
                                         'f'};

            p[1] = hex[c >> 4];
            p[2] = hex[c & 0xF];
            encoded.append(p);
        }
    }

    return encoded;
}

String StringUtils::percent_decode(StringView str) {

    String pe;

    for (size_t i = 0; i < str.length(); i++) {

        char c = str[i];
        if (c == '%' && i < str.length() - 2) {

            char a = tolower(str[i + 1]);
            char b = tolower(str[i + 2]);

            if (a >= '0' && a <= '9') {
                c = char((a - '0') << 4);
            } else if (a >= 'a' && a <= 'f') {
                c = char((a - 'a' + 10) << 4);
            } else {
                continue;
            }

            uint8_t d = 0;

            if (b >= '0' && b <= '9') {
                d = (b - '0');
            } else if (b >= 'a' && b <= 'f') {
                d = (b - 'a' + 10);
            } else {
                continue;
            }
            c += d;
            i += 2;
        }
        pe += c;
    }
    return pe;
}

StringView PathUtils::get_basename(StringView path) {

    auto pos = path.rfind('.');
    if (pos == String::npos) {
        return path;
    }
    auto file = get_file(path);
    return file.substr(0, file.rfind('.'));
}

StringView PathUtils::path(StringView path) {

    auto last_slash_pos = path.find_last_of("/\\");
    if (last_slash_pos == String::npos) {
        return ".";
    }
    return path.substr(0, last_slash_pos);
}

String itos(int64_t p_val) {

    return StringUtils::num_int64(p_val);
}

String rtos(double p_val) {

    return StringUtils::num(p_val);
}

String rtoss(double p_val) {

    return StringUtils::num_scientific(p_val);
}

// Right-pad with a character.
String StringUtils::rpad(const String &src, int min_length, char character) {
    String s = src;
    int padding = min_length - s.length();
    if (padding > 0) {
        for (int i = 0; i < padding; i++) {
            s = s + character;
        }
    }

    return s;
}

// Left-pad with a character.
String StringUtils::lpad(const String &src, int min_length, char character) {
    String s = src;
    int padding = min_length - s.length();
    if (padding > 0) {
        for (int i = 0; i < padding; i++) {
            s = character + s;
        }
    }

    return s;
}

String StringUtils::quote(StringView str, char character) {
    return character + String(str) + character;
}

StringView StringUtils::unquote(StringView str) {
    if (!is_quoted(str)) {
        return str;
    }

    return str.substr(1, str.length() - 2);
}

int StringUtils::compare(StringView lhs, StringView rhs, Compare case_sensitive) {
    if (case_sensitive == StringUtils::CaseSensitive) {
        return lhs.compare(rhs);
    }
    if (case_sensitive == StringUtils::CaseInsensitive) {
        for (size_t i = 0; i < lhs.length() && i < rhs.length(); i++) {
            char l = tolower(lhs[i]);
            char r = tolower(rhs[i]);
            if (l != r) {
                return l - r;
            }
        }
        return 0;
    }
    return 0;
}

bool StringUtils::contains(StringView heystack, char c) {
    return heystack.find(c) != heystack.npos;
}

bool StringUtils::contains(StringView heystack, StringView c, Compare mode) {
    return heystack.contains(c, mode==Compare::CaseSensitive);
}

bool PathUtils::is_internal_path(StringView path) {
    return StringUtils::contains(path, ("local://")) || StringUtils::contains(path, ("::"));
}

String StringUtils::property_name_encode(StringView str) {
    // Escape and quote strings with extended ASCII or further Unicode characters
    // as well as '"', '=' or ' ' (32)
    for (char c: str) {
        if (c == '=' || c == '"' || c == ';' || c == '[' || c == ']' || c < 33 || c > 126) {
            return "\"" + c_escape_multiline(str) + "\"";
        }
    }
    // Keep as is
    return String(str);
}

String StringUtils::to_snake_case(StringView str) {
    return String(strip_edges(replace(camelcase_to_underscore(str), ' ', '_')));
}

static bool _is_valid_identifier_bit(int p_index, char32_t p_char) {
    if (p_index == 0 && CharUtils::is_digit(p_char)) {
        return false; // No start with number plz.
    }
    return CharUtils::is_ascii_identifier_char(p_char);
}

String StringUtils::validate_identifier(StringView str) {
    if (str.empty()) {
        return "_"; // Empty string is not a valid identifier;
    }

    String result(str);
    int len = result.length();
    char *buffer = result.data();

    for (int i = 0; i < len; i++) {
        if (!_is_valid_identifier_bit(i, buffer[i])) {
            buffer[i] = '_';
        }
    }

    return result;
}

static String _camelcase_to_underscore(const String &self) {
    const auto *cstr = self.data();
    String new_string;
    int start_index = 0;

    for (int i = 1; i < self.size(); i++) {
        bool is_prev_upper = CharUtils::is_ascii_upper_case(cstr[i - 1]);
        bool is_prev_lower = CharUtils::is_ascii_lower_case(cstr[i - 1]);
        bool is_prev_digit = CharUtils::is_digit(cstr[i - 1]);

        bool is_curr_upper = CharUtils::is_ascii_upper_case(cstr[i]);
        bool is_curr_lower = CharUtils::is_ascii_lower_case(cstr[i]);
        bool is_curr_digit = CharUtils::is_digit(cstr[i]);

        bool is_next_lower = false;
        if (i + 1 < self.size()) {
            is_next_lower = CharUtils::is_ascii_lower_case(cstr[i + 1]);
        }

        const bool cond_a = is_prev_lower && is_curr_upper; // aA
        const bool cond_b = (is_prev_upper || is_prev_digit) && is_curr_upper && is_next_lower; // AAa, 2Aa
        const bool cond_c = is_prev_digit && is_curr_lower && is_next_lower; // 2aa
        const bool cond_d = (is_prev_upper || is_prev_lower) && is_curr_digit; // A2, a2

        if (cond_a || cond_b || cond_c || cond_d) {
            new_string += self.substr(start_index, i - start_index) + "_";
            start_index = i;
        }
    }

    new_string += self.substr(start_index, self.size() - start_index);
    return StringUtils::to_lower(new_string);
}

String StringUtils::to_camel_case(const String &self) {
    String s = to_pascal_case(self);
    if (!s.empty()) {
        s[0] = char_lowercase(s[0]);
    }
    return s;
}

String StringUtils::to_pascal_case(const String &self) {
    return capitalize(self).replaced(" ", "");
}

String StringUtils::to_snake_case(const String &self) {
    return String(strip_edges(_camelcase_to_underscore(self).replaced(" ", "_")));
}

DateTime StringUtils::parseDate(const char *value, StringView fmt) {
    std::tm tm = {};
    std::istringstream ss(value);
    ss >> std::get_time(&tm, fmt.data());
    if (ss.fail()) {
        return DateTime(fromChronoType(std::chrono::system_clock::time_point::max()));
    }

    // Convert std::tm to time_t (assumes tm is in local time)
    std::time_t time_t_val = std::mktime(&tm);
    if (time_t_val == -1) {
        return DateTime(fromChronoType(std::chrono::system_clock::time_point::max()));
    }

    // Convert time_t to time_point
    ChronoWrapper tp = fromChronoType(std::chrono::system_clock::from_time_t(time_t_val));
    return DateTime(tp);
}

namespace PathUtils {
    String from_native_path(StringView p) {
        return StringUtils::replace(p, '\\', '/');
    }

    String to_win_path(StringView v) {
        return StringUtils::replace(v, "/", "\\");
    }

    String join_path(StringView p1, StringView p2) {
        return join_path({p1, p2});
    }
}

Vector<StringView> StringUtils::split_any(StringView str, StringView split_chars, bool p_allow_empty)
{
    Vector<StringView> tgt;
    const char* strEnd = str.data() + str.size();
    const char *start = str.data();
    for (const char* splitEnd = start; splitEnd != strEnd; ++splitEnd)
    {
    if (split_chars.contains(*splitEnd))
    {
        const ptrdiff_t splitLen = splitEnd - start;
        if (splitLen > 0 || p_allow_empty)
            tgt.emplace_back(start,splitLen);
        start = splitEnd + 1;
    }
    }

    const ptrdiff_t splitLen = strEnd - start;
    if (splitLen > 0 || p_allow_empty)
    tgt.emplace_back(start, splitLen);
    return tgt;
}

double StringUtils::to_double(StringView str, bool* ok) {
    char* endptr;
    double val = strtod(str.data(), &endptr);
    if (ok) {
        *ok = endptr != str.data();
    }
    return val;
}

int Vsnprintf8(char* pDestination, size_t n, const char* pFormat, va_list arguments)
{
#ifdef _MSC_VER
    return _vsnprintf(pDestination, n, pFormat, arguments);
#else
    return vsnprintf(pDestination, n, pFormat, arguments);
#endif
}

String StringUtils::simplified(StringView from) {
    StringView stripped=StringUtils::strip_edges(from);
    String res;
    bool prev_space=false;
    for(auto c : stripped) {
        if(CharUtils::is_whitespace(c)) {
            if(!prev_space) {
                prev_space=true;
                res.push_back(' ');
            }
            continue;
        }
        res.push_back(c);
        prev_space=false;
    }
    return res;
}
