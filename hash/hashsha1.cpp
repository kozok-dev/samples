#include "hashsha1.h"

#define SHA1CALC1(a, b, c, d, e, t, data) \
	t = (b & (c ^ d) ^ d) + Rotate(a, 5) + e + data + 0x5a827999; \
	b = Rotate(b, 30);

#define SHA1CALC2(a, b, c, d, e, t, data) \
	t = (b ^ c ^ d) + Rotate(a, 5) + e + data + 0x6ed9eba1; \
	b = Rotate(b, 30);

#define SHA1CALC3(a, b, c, d, e, t, data) \
	t = (b & (c | d) | c & d) + Rotate(a, 5) + e + data + 0x8f1bbcdc; \
	b = Rotate(b, 30);

#define SHA1CALC4(a, b, c, d, e, t, data) \
	t = (b ^ c ^ d) + Rotate(a, 5) + e + data + 0xca62c1d6; \
	b = Rotate(b, 30);

BYTE HashSHA1::getHashSize() {
	return 20;
}

void HashSHA1::Init() {
	m_len[0] = m_len[1] = 0;
	m_hash[0] = 0x67452301;
	m_hash[1] = 0xefcdab89;
	m_hash[2] = 0x98badcfe;
	m_hash[3] = 0x10325476;
	m_hash[4] = 0xc3d2e1f0;
}

void HashSHA1::Update(const BYTE *pdata, UINT len) {
	UINT i, j;

	i = m_len[0] >> 3 & 63;

	j = len << 3;
	m_len[0] += j;
	if (m_len[0] < j) m_len[1]++;
	m_len[1] += len >> 29;

	j = 64 - i;
	if (len < j) {
		j = 0;
	} else {
		CopyMemory(m_buf + i, pdata, j);
		i = 0;

		Calc(m_buf);
		for (; j + 63 < len; j += 64) Calc(pdata + j);
	}

	CopyMemory(m_buf + i, pdata + j, len - j);
}

void HashSHA1::Final() {
	BYTE pad[64], len[8], i;

	len[0] = (BYTE)(m_len[1] >> 24);
	len[1] = (BYTE)(m_len[1] >> 16);
	len[2] = (BYTE)(m_len[1] >> 8);
	len[3] = (BYTE)m_len[1];
	len[4] = (BYTE)(m_len[0] >> 24);
	len[5] = (BYTE)(m_len[0] >> 16);
	len[6] = (BYTE)(m_len[0] >> 8);
	len[7] = (BYTE)m_len[0];

	i = m_len[0] >> 3 & 63;
	i = (i < 56 ? 56 : 120) - i;
	pad[0] = 0x80;
	ZeroMemory(pad + 1, i - 1);
	Update(pad, i);

	Update(len, 8);
}

void HashSHA1::getHash(BYTE *pdata) {
	BYTE i;

	for (i = 0; i < 20; i++) pdata[i] = (BYTE)(m_hash[i >> 2] >> 24 - ((i & 3) << 3));
}

void HashSHA1::Calc(const BYTE *pdata) {
	UINT data[80], a, b, c, d, e, t, i;

	for (i = 0; i < 64; i += 4) {
		data[i >> 2] = (pdata[i] << 24) + (pdata[i + 1] << 16) + (pdata[i + 2] << 8) + pdata[i + 3];
	}
	for (i = 16; i < 80; i++) data[i] = Rotate(data[i - 3] ^ data[i - 8] ^ data[i - 14] ^ data[i - 16], 1);

	a = m_hash[0];
	b = m_hash[1];
	c = m_hash[2];
	d = m_hash[3];
	e = m_hash[4];

	SHA1CALC1(a, b, c, d, e, t, data[ 0]);
	SHA1CALC1(t, a, b, c, d, e, data[ 1]);
	SHA1CALC1(e, t, a, b, c, d, data[ 2]);
	SHA1CALC1(d, e, t, a, b, c, data[ 3]);
	SHA1CALC1(c, d, e, t, a, b, data[ 4]);
	SHA1CALC1(b, c, d, e, t, a, data[ 5]);
	SHA1CALC1(a, b, c, d, e, t, data[ 6]);
	SHA1CALC1(t, a, b, c, d, e, data[ 7]);
	SHA1CALC1(e, t, a, b, c, d, data[ 8]);
	SHA1CALC1(d, e, t, a, b, c, data[ 9]);
	SHA1CALC1(c, d, e, t, a, b, data[10]);
	SHA1CALC1(b, c, d, e, t, a, data[11]);
	SHA1CALC1(a, b, c, d, e, t, data[12]);
	SHA1CALC1(t, a, b, c, d, e, data[13]);
	SHA1CALC1(e, t, a, b, c, d, data[14]);
	SHA1CALC1(d, e, t, a, b, c, data[15]);
	SHA1CALC1(c, d, e, t, a, b, data[16]);
	SHA1CALC1(b, c, d, e, t, a, data[17]);
	SHA1CALC1(a, b, c, d, e, t, data[18]);
	SHA1CALC1(t, a, b, c, d, e, data[19]);

	SHA1CALC2(e, t, a, b, c, d, data[20]);
	SHA1CALC2(d, e, t, a, b, c, data[21]);
	SHA1CALC2(c, d, e, t, a, b, data[22]);
	SHA1CALC2(b, c, d, e, t, a, data[23]);
	SHA1CALC2(a, b, c, d, e, t, data[24]);
	SHA1CALC2(t, a, b, c, d, e, data[25]);
	SHA1CALC2(e, t, a, b, c, d, data[26]);
	SHA1CALC2(d, e, t, a, b, c, data[27]);
	SHA1CALC2(c, d, e, t, a, b, data[28]);
	SHA1CALC2(b, c, d, e, t, a, data[29]);
	SHA1CALC2(a, b, c, d, e, t, data[30]);
	SHA1CALC2(t, a, b, c, d, e, data[31]);
	SHA1CALC2(e, t, a, b, c, d, data[32]);
	SHA1CALC2(d, e, t, a, b, c, data[33]);
	SHA1CALC2(c, d, e, t, a, b, data[34]);
	SHA1CALC2(b, c, d, e, t, a, data[35]);
	SHA1CALC2(a, b, c, d, e, t, data[36]);
	SHA1CALC2(t, a, b, c, d, e, data[37]);
	SHA1CALC2(e, t, a, b, c, d, data[38]);
	SHA1CALC2(d, e, t, a, b, c, data[39]);

	SHA1CALC3(c, d, e, t, a, b, data[40]);
	SHA1CALC3(b, c, d, e, t, a, data[41]);
	SHA1CALC3(a, b, c, d, e, t, data[42]);
	SHA1CALC3(t, a, b, c, d, e, data[43]);
	SHA1CALC3(e, t, a, b, c, d, data[44]);
	SHA1CALC3(d, e, t, a, b, c, data[45]);
	SHA1CALC3(c, d, e, t, a, b, data[46]);
	SHA1CALC3(b, c, d, e, t, a, data[47]);
	SHA1CALC3(a, b, c, d, e, t, data[48]);
	SHA1CALC3(t, a, b, c, d, e, data[49]);
	SHA1CALC3(e, t, a, b, c, d, data[50]);
	SHA1CALC3(d, e, t, a, b, c, data[51]);
	SHA1CALC3(c, d, e, t, a, b, data[52]);
	SHA1CALC3(b, c, d, e, t, a, data[53]);
	SHA1CALC3(a, b, c, d, e, t, data[54]);
	SHA1CALC3(t, a, b, c, d, e, data[55]);
	SHA1CALC3(e, t, a, b, c, d, data[56]);
	SHA1CALC3(d, e, t, a, b, c, data[57]);
	SHA1CALC3(c, d, e, t, a, b, data[58]);
	SHA1CALC3(b, c, d, e, t, a, data[59]);

	SHA1CALC4(a, b, c, d, e, t, data[60]);
	SHA1CALC4(t, a, b, c, d, e, data[61]);
	SHA1CALC4(e, t, a, b, c, d, data[62]);
	SHA1CALC4(d, e, t, a, b, c, data[63]);
	SHA1CALC4(c, d, e, t, a, b, data[64]);
	SHA1CALC4(b, c, d, e, t, a, data[65]);
	SHA1CALC4(a, b, c, d, e, t, data[66]);
	SHA1CALC4(t, a, b, c, d, e, data[67]);
	SHA1CALC4(e, t, a, b, c, d, data[68]);
	SHA1CALC4(d, e, t, a, b, c, data[69]);
	SHA1CALC4(c, d, e, t, a, b, data[70]);
	SHA1CALC4(b, c, d, e, t, a, data[71]);
	SHA1CALC4(a, b, c, d, e, t, data[72]);
	SHA1CALC4(t, a, b, c, d, e, data[73]);
	SHA1CALC4(e, t, a, b, c, d, data[74]);
	SHA1CALC4(d, e, t, a, b, c, data[75]);
	SHA1CALC4(c, d, e, t, a, b, data[76]);
	SHA1CALC4(b, c, d, e, t, a, data[77]);
	SHA1CALC4(a, b, c, d, e, t, data[78]);
	SHA1CALC4(t, a, b, c, d, e, data[79]);

	m_hash[0] += e;
	m_hash[1] += t;
	m_hash[2] += a;
	m_hash[3] += b;
	m_hash[4] += c;
}

UINT HashSHA1::Rotate(UINT a, BYTE b) {
	return a << b | a >> 32 - b;
}
