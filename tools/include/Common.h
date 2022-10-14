#pragma once
#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;
vector<uint8_t> HexToBytes(const string& hex);
ostream& DumpArray(ostream& out, uint8_t *data, uint32_t len, const char *prefix);
