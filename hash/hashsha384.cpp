#include "hashsha384.h"

BYTE HashSHA384::getHashSize() {
	return 48;
}

void HashSHA384::Init() {
	m_len[0] = m_len[1] = 0;
	m_hash[0] = 0xcbbb9d5dc1059ed8;
	m_hash[1] = 0x629a292a367cd507;
	m_hash[2] = 0x9159015a3070dd17;
	m_hash[3] = 0x152fecd8f70e5939;
	m_hash[4] = 0x67332667ffc00b31;
	m_hash[5] = 0x8eb44a8768581511;
	m_hash[6] = 0xdb0c2e0d64f98fa7;
	m_hash[7] = 0x47b5481dbefa4fa4;
}

void HashSHA384::getHash(BYTE *pdata) {
	BYTE i;

	for (i = 0; i < 48; i++) pdata[i] = (BYTE)(m_hash[i >> 3] >> 56 - ((i & 7) << 3));
}
