#pragma once

#define WORD uint16_t
#if __has_feature(modules) // doesnt work on macos unfortunately
import std;
#else
#include <algorithm>
#include <cmath>
#include <map>
#include <cstddef>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#endif
