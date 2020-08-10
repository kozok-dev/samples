#pragma once
#include "hashsha256.h"

class HashSHA224 : public HashSHA256 {
public:
	BYTE getHashSize();

protected:
	void Init();
	void getHash(BYTE *);
};
