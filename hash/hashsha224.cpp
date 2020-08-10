#include "hashsha224.h"

BYTE HashSHA224::getHashSize() {
	return 28;
}

void HashSHA224::Init() {
	m_len[0] = m_len[1] = 0;
	m_hash[0] = 0xc1059ed8;
	m_hash[1] = 0x367cd507;
	m_hash[2] = 0x3070dd17;
	m_hash[3] = 0xf70e5939;
	m_hash[4] = 0xffc00b31;
	m_hash[5] = 0x68581511;
	m_hash[6] = 0x64f98fa7;
	m_hash[7] = 0xbefa4fa4;
}

void HashSHA224::getHash(BYTE *pdata) {
	BYTE i;

	for (i = 0; i < 28; i++) pdata[i] = (BYTE)(m_hash[i >> 2] >> 24 - ((i & 3) << 3));
}
