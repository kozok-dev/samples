#pragma once
#include "hashsha1.h"

class HashSHA256 : public HashSHA1 {
public:
	virtual BYTE getHashSize();

protected:
	UINT m_hash[8];

	void Init();
	void getHash(BYTE *);

private:
	void Calc(const BYTE *);
	UINT Rotate(UINT a, BYTE b);
};
