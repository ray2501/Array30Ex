// OVOFHanConvert.cpp: Traditional<->Simplified Chinese Output Filter
//
// Copyright (c) 2004-2007 The OpenVanilla Project (http://openvanilla.org)
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of OpenVanilla nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cctype>
#include "array30.h"
#include "SC2TCTable.h"
#include "TC2SCTable.h"

const size_t SC2TCTableSize=8189;
extern unsigned short SC2TCTable[];

const size_t TC2SCTableSize=3209;
extern unsigned short TC2SCTable[];

struct HCData {
	unsigned short key, value;
};

int HCCompare(const void *a, const void *b)
{
	unsigned short x=((const struct HCData*)a)->key, y=((const struct HCData*)b)->key;
	if (x==y) return 0;
	if (x<y) return -1;
	return 1;
}

unsigned short HCFind(unsigned key, unsigned short *table, size_t size)
{
	struct HCData k;
	k.key=key;
	struct HCData *d = (struct HCData*)bsearch(&k, table, size, sizeof(struct HCData), HCCompare);
	
	if (!d) return 0;
	return d->value;
}

unsigned short UCS2TradToSimpChinese(unsigned short c)
{
	return HCFind(c, TC2SCTable, TC2SCTableSize);
}

unsigned short UCS2SimpToTradChinese(unsigned short c)
{
	return HCFind(c, SC2TCTable, SC2TCTableSize);
}

