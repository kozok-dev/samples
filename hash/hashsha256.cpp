#include "hashsha256.h"
#define SHA256CALC(a, b, c, d, e, f, g, h, t, data, cd) \
	t = (Rotate(e, 6) ^ Rotate(e, 11) ^ Rotate(e, 25)) + (e & (f ^ g) ^ g) + h + data + cd; \
	d += t; t += (Rotate(a, 2) ^ Rotate(a, 13) ^ Rotate(a, 22)) + (a & (b | c) | b & c);

BYTE HashSHA256::getHashSize() {
	return 32;
}

void HashSHA256::Init() {
	m_len[0] = m_len[1] = 0;
	m_hash[0] = 0x6a09e667;
	m_hash[1] = 0xbb67ae85;
	m_hash[2] = 0x3c6ef372;
	m_hash[3] = 0xa54ff53a;
	m_hash[4] = 0x510e527f;
	m_hash[5] = 0x9b05688c;
	m_hash[6] = 0x1f83d9ab;
	m_hash[7] = 0x5be0cd19;
}

void HashSHA256::getHash(BYTE *pdata) {
	BYTE i;

	for (i = 0; i < 32; i++) pdata[i] = (BYTE)(m_hash[i >> 2] >> 24 - ((i & 3) << 3));
}

void HashSHA256::Calc(const BYTE *pdata) {
	UINT data[64], a, b, c, d, e, f, g, h, t, i;

	for (i = 0; i < 64; i += 4) {
		data[i >> 2] = (pdata[i] << 24) + (pdata[i + 1] << 16) + (pdata[i + 2] << 8) + pdata[i + 3];
	}
	for (i = 16; i < 64; i++) {
		data[i] = (Rotate(data[i - 2], 17) ^ Rotate(data[i - 2], 19) ^ data[i - 2] >> 10)
			+ data[i - 7] + (Rotate(data[i - 15], 7) ^ Rotate(data[i - 15], 18) ^ data[i - 15] >> 3) + data[i - 16];
	}

	a = m_hash[0];
	b = m_hash[1];
	c = m_hash[2];
	d = m_hash[3];
	e = m_hash[4];
	f = m_hash[5];
	g = m_hash[6];
	h = m_hash[7];

	SHA256CALC(a, b, c, d, e, f, g, h, t, data[ 0], 0x428a2f98);
	SHA256CALC(t, a, b, c, d, e, f, g, h, data[ 1], 0x71374491);
	SHA256CALC(h, t, a, b, c, d, e, f, g, data[ 2], 0xb5c0fbcf);
	SHA256CALC(g, h, t, a, b, c, d, e, f, data[ 3], 0xe9b5dba5);
	SHA256CALC(f, g, h, t, a, b, c, d, e, data[ 4], 0x3956c25b);
	SHA256CALC(e, f, g, h, t, a, b, c, d, data[ 5], 0x59f111f1);
	SHA256CALC(d, e, f, g, h, t, a, b, c, data[ 6], 0x923f82a4);
	SHA256CALC(c, d, e, f, g, h, t, a, b, data[ 7], 0xab1c5ed5);
	SHA256CALC(b, c, d, e, f, g, h, t, a, data[ 8], 0xd807aa98);
	SHA256CALC(a, b, c, d, e, f, g, h, t, data[ 9], 0x12835b01);
	SHA256CALC(t, a, b, c, d, e, f, g, h, data[10], 0x243185be);
	SHA256CALC(h, t, a, b, c, d, e, f, g, data[11], 0x550c7dc3);
	SHA256CALC(g, h, t, a, b, c, d, e, f, data[12], 0x72be5d74);
	SHA256CALC(f, g, h, t, a, b, c, d, e, data[13], 0x80deb1fe);
	SHA256CALC(e, f, g, h, t, a, b, c, d, data[14], 0x9bdc06a7);
	SHA256CALC(d, e, f, g, h, t, a, b, c, data[15], 0xc19bf174);
	SHA256CALC(c, d, e, f, g, h, t, a, b, data[16], 0xe49b69c1);
	SHA256CALC(b, c, d, e, f, g, h, t, a, data[17], 0xefbe4786);
	SHA256CALC(a, b, c, d, e, f, g, h, t, data[18], 0x0fc19dc6);
	SHA256CALC(t, a, b, c, d, e, f, g, h, data[19], 0x240ca1cc);
	SHA256CALC(h, t, a, b, c, d, e, f, g, data[20], 0x2de92c6f);
	SHA256CALC(g, h, t, a, b, c, d, e, f, data[21], 0x4a7484aa);
	SHA256CALC(f, g, h, t, a, b, c, d, e, data[22], 0x5cb0a9dc);
	SHA256CALC(e, f, g, h, t, a, b, c, d, data[23], 0x76f988da);
	SHA256CALC(d, e, f, g, h, t, a, b, c, data[24], 0x983e5152);
	SHA256CALC(c, d, e, f, g, h, t, a, b, data[25], 0xa831c66d);
	SHA256CALC(b, c, d, e, f, g, h, t, a, data[26], 0xb00327c8);
	SHA256CALC(a, b, c, d, e, f, g, h, t, data[27], 0xbf597fc7);
	SHA256CALC(t, a, b, c, d, e, f, g, h, data[28], 0xc6e00bf3);
	SHA256CALC(h, t, a, b, c, d, e, f, g, data[29], 0xd5a79147);
	SHA256CALC(g, h, t, a, b, c, d, e, f, data[30], 0x06ca6351);
	SHA256CALC(f, g, h, t, a, b, c, d, e, data[31], 0x14292967);
	SHA256CALC(e, f, g, h, t, a, b, c, d, data[32], 0x27b70a85);
	SHA256CALC(d, e, f, g, h, t, a, b, c, data[33], 0x2e1b2138);
	SHA256CALC(c, d, e, f, g, h, t, a, b, data[34], 0x4d2c6dfc);
	SHA256CALC(b, c, d, e, f, g, h, t, a, data[35], 0x53380d13);
	SHA256CALC(a, b, c, d, e, f, g, h, t, data[36], 0x650a7354);
	SHA256CALC(t, a, b, c, d, e, f, g, h, data[37], 0x766a0abb);
	SHA256CALC(h, t, a, b, c, d, e, f, g, data[38], 0x81c2c92e);
	SHA256CALC(g, h, t, a, b, c, d, e, f, data[39], 0x92722c85);
	SHA256CALC(f, g, h, t, a, b, c, d, e, data[40], 0xa2bfe8a1);
	SHA256CALC(e, f, g, h, t, a, b, c, d, data[41], 0xa81a664b);
	SHA256CALC(d, e, f, g, h, t, a, b, c, data[42], 0xc24b8b70);
	SHA256CALC(c, d, e, f, g, h, t, a, b, data[43], 0xc76c51a3);
	SHA256CALC(b, c, d, e, f, g, h, t, a, data[44], 0xd192e819);
	SHA256CALC(a, b, c, d, e, f, g, h, t, data[45], 0xd6990624);
	SHA256CALC(t, a, b, c, d, e, f, g, h, data[46], 0xf40e3585);
	SHA256CALC(h, t, a, b, c, d, e, f, g, data[47], 0x106aa070);
	SHA256CALC(g, h, t, a, b, c, d, e, f, data[48], 0x19a4c116);
	SHA256CALC(f, g, h, t, a, b, c, d, e, data[49], 0x1e376c08);
	SHA256CALC(e, f, g, h, t, a, b, c, d, data[50], 0x2748774c);
	SHA256CALC(d, e, f, g, h, t, a, b, c, data[51], 0x34b0bcb5);
	SHA256CALC(c, d, e, f, g, h, t, a, b, data[52], 0x391c0cb3);
	SHA256CALC(b, c, d, e, f, g, h, t, a, data[53], 0x4ed8aa4a);
	SHA256CALC(a, b, c, d, e, f, g, h, t, data[54], 0x5b9cca4f);
	SHA256CALC(t, a, b, c, d, e, f, g, h, data[55], 0x682e6ff3);
	SHA256CALC(h, t, a, b, c, d, e, f, g, data[56], 0x748f82ee);
	SHA256CALC(g, h, t, a, b, c, d, e, f, data[57], 0x78a5636f);
	SHA256CALC(f, g, h, t, a, b, c, d, e, data[58], 0x84c87814);
	SHA256CALC(e, f, g, h, t, a, b, c, d, data[59], 0x8cc70208);
	SHA256CALC(d, e, f, g, h, t, a, b, c, data[60], 0x90befffa);
	SHA256CALC(c, d, e, f, g, h, t, a, b, data[61], 0xa4506ceb);
	SHA256CALC(b, c, d, e, f, g, h, t, a, data[62], 0xbef9a3f7);
	SHA256CALC(a, b, c, d, e, f, g, h, t, data[63], 0xc67178f2);

	m_hash[0] += t;
	m_hash[1] += a;
	m_hash[2] += b;
	m_hash[3] += c;
	m_hash[4] += d;
	m_hash[5] += e;
	m_hash[6] += f;
	m_hash[7] += g;
}

UINT HashSHA256::Rotate(UINT a, BYTE b) {
	return a >> b | a << 32 - b;
}
