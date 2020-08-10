#pragma once
#include "hashmd5.h"

class HashSHA1 : public HashMD5 {
public:
	virtual BYTE getHashSize();

protected:
	void Init();
	void Update(const BYTE *, UINT);
	void Final();
	void getHash(BYTE *);

private:
	UINT m_hash[5];

	virtual void Calc(const BYTE *);
	UINT Rotate(UINT a, BYTE b);
};
