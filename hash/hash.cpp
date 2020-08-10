#include "hash.h"

void Hash::Str(const void *pstr, void *pdata, UINT len, const void *pkey, UINT key_len) {
	if (pkey == NULL) Init();
	else initHMAC((BYTE *)pkey, key_len > 0 ? key_len : strlen((char *)pkey));

	Update((BYTE *)pstr, len > 0 ? len : strlen((char *)pstr));

	if (pkey == NULL) Final();
	else finalHMAC();

	getHash((BYTE *)pdata);
}

bool Hash::File(LPCSTR pfilename, void *pdata, const void *pkey, UINT key_len) {
	BYTE buf[4096];
	UINT r;
	FILE *pfile;

	pfile = fopen(pfilename, "rb");
	if (pfile == NULL) return false;

	m_stop = false;

	if (pkey == NULL) Init();
	else initHMAC((BYTE *)pkey, key_len);

	for (;;) {
		r = fread(buf, 1, sizeof(buf), pfile);
		if (r == 0 || m_stop) break;
		Update(buf, r);
	}

	if (pkey == NULL) Final();
	else finalHMAC();

	getHash((BYTE *)pdata);
	return true;
}

void Hash::initHMAC(const BYTE *pkey, UINT len) {
	BYTE buf[128], i;

	if (len > getBlockSize()) {
		Init();
		Update(pkey, len);
		Final();
		getHash(buf);

		pkey = buf;
		len = getHashSize();
	}

	for (i = 0; i < len; i++) {
		buf[i] = pkey[i] ^ 0x36;
		m_opad[i] = pkey[i] ^ 0x5c;
	}
	for (; i < getBlockSize(); i++) {
		buf[i] = 0x36;
		m_opad[i] = 0x5c;
	}

	Init();
	Update(buf, getBlockSize());
}

void Hash::finalHMAC() {
	BYTE buf[128];

	Final();
	getHash(buf);

	Init();
	Update(m_opad, getBlockSize());
	Update(buf, getHashSize());
	Final();
}

void Hash::Stop() {
	m_stop = true;
}
