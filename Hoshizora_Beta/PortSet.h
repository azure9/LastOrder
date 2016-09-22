#pragma once
#include "snmp.h"
#include <bitset>

using namespace std;
class CPortSet
{
public:
	CPortSet(Vb & src)
	{
		DWORD dwLen;
		src.get_value(macS,dwLen,64,false);
	};
	CPortSet()
	{
		memset(macS,0,sizeof(macS));
	};
	~CPortSet(void){};

	bool GetPos(int pos)
	{
// 		bitset<8> bit(macS[pos/8]);
// 		return bit.test(7-pos%8);
		unsigned char b = macS[pos/8];
		return (b>>(7-pos%8))&true;
	}
	void SetPos(int pos,bool bSecurityOn)
	{
		bitset<8> bit(macS[pos/8]);
		bit.set(7-pos%8,bSecurityOn);
		macS[pos/8] = bit.to_ulong();
		return;
	}
	unsigned char *GetDesc(void)
	{
		return macS;
	}

	bool operator == (CPortSet &p)
	{
		return !memcmp(p.macS,macS,64);
	}

	bool operator != (CPortSet &p)
	{
		return memcmp(p.macS,macS,64);
	}
private:
	unsigned char macS[64];
};

