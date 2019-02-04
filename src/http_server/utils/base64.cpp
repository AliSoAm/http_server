#include "base64.hpp"

#include <stdexcept>

using namespace std;
vector<uint8_t> base64Decode(const string& base64Encoded)
{
    if (base64Encoded.length() % 4 != 0)
        throw length_error("Base 64 encoded string length should be multiple of 4");
    unsigned long long charCounter = 0;
    unsigned short int equCounter = 0;
    uint32_t val = 0;
    vector<uint8_t> toBeReturned;
    for (auto& ch : base64Encoded)
    {
        if      (ch >= 'A' && ch <= 'Z')
            val |= (ch - 'A' + 0 ) << (18 - charCounter * 6);
        else if (ch >= 'a' && ch <= 'z')
            val |= (ch - 'a' + 26) << (18 - charCounter * 6);
        else if (ch >= '0' && ch <= '9')
            val |= (ch - '0' + 52) << (18 - charCounter * 6);
        else if (ch == '+')
            val |= 62              << (18 - charCounter * 6);
        else if (ch == '/')
            val |= 63              << (18 - charCounter * 6);
        else if (ch == '=')
            equCounter++;
        else
            throw domain_error("Base 64 encoded string can contain \'A\'-\'Z\', \'a\'-\'z\', \'0\'-\'9\', \'+\', \'/\' and \'=\'");
        charCounter++;
        if (charCounter == 4)
        {
            for (auto i = 0; i < (3 - equCounter); i++)
                toBeReturned.push_back((val >> (16 - 8 * i)) & 0xFF);
            charCounter = 0;
            equCounter = 0;
            val = 0;
        }
    }
    return toBeReturned;
}
