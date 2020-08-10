#pragma once
#include "hash.h"

class HashMD5 : public Hash {
public:
	virtual BYTE getBlockSize();
	virtual BYTE getHashSize();

protected:
	UINT m_len[2];
	BYTE m_buf[64];

	void Init();
	void Update(const BYTE *, UINT);
	void Final();
	void getHash(BYTE *);

private:
	UINT m_hash[4];

	void Calc(const UINT *);
};
