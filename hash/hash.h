#pragma once
#include <stdio.h>
#include <windows.h>

class Hash {
public:
	void Str(const void *, void *, UINT = 0, const void * = NULL, UINT = 0);
	bool File(LPCSTR, void *, const void * = NULL, UINT = 0);
	void Stop();
	virtual BYTE getHashSize() = 0;
	virtual BYTE getBlockSize() = 0;

protected:
	bool m_stop;
	BYTE m_opad[128];

	void initHMAC(const BYTE *, UINT);
	void finalHMAC();

	virtual void Init() = 0;
	virtual void Update(const BYTE *, UINT) = 0;
	virtual void Final() = 0;
	virtual void getHash(BYTE *) = 0;
};
