#include <Common.h>

vector<uint8_t> HexToBytes(const string& hex)
{
    vector<uint8_t> bytes;

    for (unsigned int i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    
    return bytes;
}

void binary_to_string(const uint8_t* src, uint32_t len, string& dest)
{
    char digit[4] = {0};
    dest.clear();

    for(auto i = 0; i < len; i++) {
	if ((i % 7 == 8) || (i == (len - 1)))
		sprintf(digit, "%02x\n", src[i]);
	else
		sprintf(digit, "%02x ", src[i]);
	dest.append(digit);
    }
}

ostream& DumpArray(ostream& out, uint8_t *data, uint32_t len, const char* prefix)
{
	if (prefix)
		out << prefix << ":" << endl;;
	string dest;
	binary_to_string(data, len, dest);
	out << dest << endl;;
	return out;
}
