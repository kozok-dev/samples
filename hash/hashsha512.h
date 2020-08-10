#pragma once
#include "hash.h"

class HashSHA512 : public Hash {
public:
	virtual BYTE getHashSize();
	virtual BYTE getBlockSize();

protected:
	UINT64 m_hash[8], m_len[2];

	void Init();
	void Update(const BYTE *, UINT);
	void Final();
	void getHash(BYTE *);

private:
	BYTE m_buf[128];

	void Calc(const BYTE *);
	UINT64 Rotate(UINT64 a, BYTE b);
};
