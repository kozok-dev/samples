#pragma once
#include "hashsha512.h"

class HashSHA384 : public HashSHA512 {
public:
	BYTE getHashSize();

protected:
	void Init();
	void getHash(BYTE *);
};
