/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <cstdint>

uint32_t nextPot(uint32_t i);
uint32_t fixSize(uint32_t size, bool isPot);
uint64_t getCurrentTime();
const char* formatNum(int num, char delimiter = '\'');
const char* isEnabled(bool enabled);
