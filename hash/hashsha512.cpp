#include "hashsha512.h"
#define SHA512CALC(a, b, c, d, e, f, g, h, t, data, cd) \
	t = (Rotate(e, 14) ^ Rotate(e, 18) ^ Rotate(e, 41)) + (e & (f ^ g) ^ g) + h + data + cd; \
	d += t; t += (Rotate(a, 28) ^ Rotate(a, 34) ^ Rotate(a, 39)) + (a & (b | c) | b & c);

BYTE HashSHA512::getHashSize() {
	return 64;
}

BYTE HashSHA512::getBlockSize() {
	return 128;
}

void HashSHA512::Init() {
	m_len[0] = m_len[1] = 0;
	m_hash[0] = 0x6a09e667f3bcc908;
	m_hash[1] = 0xbb67ae8584caa73b;
	m_hash[2] = 0x3c6ef372fe94f82b;
	m_hash[3] = 0xa54ff53a5f1d36f1;
	m_hash[4] = 0x510e527fade682d1;
	m_hash[5] = 0x9b05688c2b3e6c1f;
	m_hash[6] = 0x1f83d9abfb41bd6b;
	m_hash[7] = 0x5be0cd19137e2179;
}

void HashSHA512::Update(const BYTE *pdata, UINT len) {
	UINT i, j;

	i = m_len[0] >> 3 & 127;

	j = len << 3;
	m_len[0] += j;
	if (m_len[0] < j) m_len[1]++;

	j = 128 - i;
	if (len < j) {
		j = 0;
	} else {
		CopyMemory(m_buf + i, pdata, j);
		i = 0;

		Calc(m_buf);
		for (; j + 127 < len; j += 128) Calc(pdata + j);
	}

	CopyMemory(m_buf + i, pdata + j, len - j);
}

void HashSHA512::Final() {
	BYTE pad[128], len[16], i;

	len[ 0] = (BYTE)(m_len[1] >> 56);
	len[ 1] = (BYTE)(m_len[1] >> 48);
	len[ 2] = (BYTE)(m_len[1] >> 40);
	len[ 3] = (BYTE)(m_len[1] >> 32);
	len[ 4] = (BYTE)(m_len[1] >> 24);
	len[ 5] = (BYTE)(m_len[1] >> 16);
	len[ 6] = (BYTE)(m_len[1] >> 8);
	len[ 7] = (BYTE)m_len[1];
	len[ 8] = (BYTE)(m_len[0] >> 56);
	len[ 9] = (BYTE)(m_len[0] >> 48);
	len[10] = (BYTE)(m_len[0] >> 40);
	len[11] = (BYTE)(m_len[0] >> 32);
	len[12] = (BYTE)(m_len[0] >> 24);
	len[13] = (BYTE)(m_len[0] >> 16);
	len[14] = (BYTE)(m_len[0] >> 8);
	len[15] = (BYTE)m_len[0];

	i = m_len[0] >> 3 & 127;
	i = (i < 112 ? 112 : 240) - i;
	pad[0] = 0x80;
	ZeroMemory(pad + 1, i - 1);
	Update(pad, i);

	Update(len, 16);
}

void HashSHA512::getHash(BYTE *pdata) {
	BYTE i;

	for (i = 0; i < 64; i++) pdata[i] = (BYTE)(m_hash[i >> 3] >> 56 - ((i & 7) << 3));
}

void HashSHA512::Calc(const BYTE *pdata) {
	UINT i;
	UINT64 data[80], a, b, c, d, e, f, g, h, t1, t2, t;

	for (i = 0; i < 128; i += 8) {
		data[i >> 3] = ((UINT64)pdata[i] << 56) + ((UINT64)pdata[i + 1] << 48) + ((UINT64)pdata[i + 2] << 40)
			+ ((UINT64)pdata[i + 3] << 32) + ((UINT64)pdata[i + 4] << 24) + ((UINT64)pdata[i + 5] << 16)
			+ ((UINT64)pdata[i + 6] << 8) + (UINT64)pdata[i + 7];
	}
	for (i = 16; i < 80; i++) {
		data[i] = (Rotate(data[i - 2], 19) ^ Rotate(data[i - 2], 61) ^ (data[i - 2] >> 6)) + data[i - 7]
			+ (Rotate(data[i - 15], 1) ^ Rotate(data[i - 15], 8) ^ (data[i - 15] >> 7)) + data[i - 16];
	}

	a = m_hash[0];
	b = m_hash[1];
	c = m_hash[2];
	d = m_hash[3];
	e = m_hash[4];
	f = m_hash[5];
	g = m_hash[6];
	h = m_hash[7];

	SHA512CALC(a, b, c, d, e, f, g, h, t, data[ 0], 0x428a2f98d728ae22);
	SHA512CALC(t, a, b, c, d, e, f, g, h, data[ 1], 0x7137449123ef65cd);
	SHA512CALC(h, t, a, b, c, d, e, f, g, data[ 2], 0xb5c0fbcfec4d3b2f);
	SHA512CALC(g, h, t, a, b, c, d, e, f, data[ 3], 0xe9b5dba58189dbbc);
	SHA512CALC(f, g, h, t, a, b, c, d, e, data[ 4], 0x3956c25bf348b538);
	SHA512CALC(e, f, g, h, t, a, b, c, d, data[ 5], 0x59f111f1b605d019);
	SHA512CALC(d, e, f, g, h, t, a, b, c, data[ 6], 0x923f82a4af194f9b);
	SHA512CALC(c, d, e, f, g, h, t, a, b, data[ 7], 0xab1c5ed5da6d8118);
	SHA512CALC(b, c, d, e, f, g, h, t, a, data[ 8], 0xd807aa98a3030242);
	SHA512CALC(a, b, c, d, e, f, g, h, t, data[ 9], 0x12835b0145706fbe);
	SHA512CALC(t, a, b, c, d, e, f, g, h, data[10], 0x243185be4ee4b28c);
	SHA512CALC(h, t, a, b, c, d, e, f, g, data[11], 0x550c7dc3d5ffb4e2);
	SHA512CALC(g, h, t, a, b, c, d, e, f, data[12], 0x72be5d74f27b896f);
	SHA512CALC(f, g, h, t, a, b, c, d, e, data[13], 0x80deb1fe3b1696b1);
	SHA512CALC(e, f, g, h, t, a, b, c, d, data[14], 0x9bdc06a725c71235);
	SHA512CALC(d, e, f, g, h, t, a, b, c, data[15], 0xc19bf174cf692694);
	SHA512CALC(c, d, e, f, g, h, t, a, b, data[16], 0xe49b69c19ef14ad2);
	SHA512CALC(b, c, d, e, f, g, h, t, a, data[17], 0xefbe4786384f25e3);
	SHA512CALC(a, b, c, d, e, f, g, h, t, data[18], 0x0fc19dc68b8cd5b5);
	SHA512CALC(t, a, b, c, d, e, f, g, h, data[19], 0x240ca1cc77ac9c65);
	SHA512CALC(h, t, a, b, c, d, e, f, g, data[20], 0x2de92c6f592b0275);
	SHA512CALC(g, h, t, a, b, c, d, e, f, data[21], 0x4a7484aa6ea6e483);
	SHA512CALC(f, g, h, t, a, b, c, d, e, data[22], 0x5cb0a9dcbd41fbd4);
	SHA512CALC(e, f, g, h, t, a, b, c, d, data[23], 0x76f988da831153b5);
	SHA512CALC(d, e, f, g, h, t, a, b, c, data[24], 0x983e5152ee66dfab);
	SHA512CALC(c, d, e, f, g, h, t, a, b, data[25], 0xa831c66d2db43210);
	SHA512CALC(b, c, d, e, f, g, h, t, a, data[26], 0xb00327c898fb213f);
	SHA512CALC(a, b, c, d, e, f, g, h, t, data[27], 0xbf597fc7beef0ee4);
	SHA512CALC(t, a, b, c, d, e, f, g, h, data[28], 0xc6e00bf33da88fc2);
	SHA512CALC(h, t, a, b, c, d, e, f, g, data[29], 0xd5a79147930aa725);
	SHA512CALC(g, h, t, a, b, c, d, e, f, data[30], 0x06ca6351e003826f);
	SHA512CALC(f, g, h, t, a, b, c, d, e, data[31], 0x142929670a0e6e70);
	SHA512CALC(e, f, g, h, t, a, b, c, d, data[32], 0x27b70a8546d22ffc);
	SHA512CALC(d, e, f, g, h, t, a, b, c, data[33], 0x2e1b21385c26c926);
	SHA512CALC(c, d, e, f, g, h, t, a, b, data[34], 0x4d2c6dfc5ac42aed);
	SHA512CALC(b, c, d, e, f, g, h, t, a, data[35], 0x53380d139d95b3df);
	SHA512CALC(a, b, c, d, e, f, g, h, t, data[36], 0x650a73548baf63de);
	SHA512CALC(t, a, b, c, d, e, f, g, h, data[37], 0x766a0abb3c77b2a8);
	SHA512CALC(h, t, a, b, c, d, e, f, g, data[38], 0x81c2c92e47edaee6);
	SHA512CALC(g, h, t, a, b, c, d, e, f, data[39], 0x92722c851482353b);
	SHA512CALC(f, g, h, t, a, b, c, d, e, data[40], 0xa2bfe8a14cf10364);
	SHA512CALC(e, f, g, h, t, a, b, c, d, data[41], 0xa81a664bbc423001);
	SHA512CALC(d, e, f, g, h, t, a, b, c, data[42], 0xc24b8b70d0f89791);
	SHA512CALC(c, d, e, f, g, h, t, a, b, data[43], 0xc76c51a30654be30);
	SHA512CALC(b, c, d, e, f, g, h, t, a, data[44], 0xd192e819d6ef5218);
	SHA512CALC(a, b, c, d, e, f, g, h, t, data[45], 0xd69906245565a910);
	SHA512CALC(t, a, b, c, d, e, f, g, h, data[46], 0xf40e35855771202a);
	SHA512CALC(h, t, a, b, c, d, e, f, g, data[47], 0x106aa07032bbd1b8);
	SHA512CALC(g, h, t, a, b, c, d, e, f, data[48], 0x19a4c116b8d2d0c8);
	SHA512CALC(f, g, h, t, a, b, c, d, e, data[49], 0x1e376c085141ab53);
	SHA512CALC(e, f, g, h, t, a, b, c, d, data[50], 0x2748774cdf8eeb99);
	SHA512CALC(d, e, f, g, h, t, a, b, c, data[51], 0x34b0bcb5e19b48a8);
	SHA512CALC(c, d, e, f, g, h, t, a, b, data[52], 0x391c0cb3c5c95a63);
	SHA512CALC(b, c, d, e, f, g, h, t, a, data[53], 0x4ed8aa4ae3418acb);
	SHA512CALC(a, b, c, d, e, f, g, h, t, data[54], 0x5b9cca4f7763e373);
	SHA512CALC(t, a, b, c, d, e, f, g, h, data[55], 0x682e6ff3d6b2b8a3);
	SHA512CALC(h, t, a, b, c, d, e, f, g, data[56], 0x748f82ee5defb2fc);
	SHA512CALC(g, h, t, a, b, c, d, e, f, data[57], 0x78a5636f43172f60);
	SHA512CALC(f, g, h, t, a, b, c, d, e, data[58], 0x84c87814a1f0ab72);
	SHA512CALC(e, f, g, h, t, a, b, c, d, data[59], 0x8cc702081a6439ec);
	SHA512CALC(d, e, f, g, h, t, a, b, c, data[60], 0x90befffa23631e28);
	SHA512CALC(c, d, e, f, g, h, t, a, b, data[61], 0xa4506cebde82bde9);
	SHA512CALC(b, c, d, e, f, g, h, t, a, data[62], 0xbef9a3f7b2c67915);
	SHA512CALC(a, b, c, d, e, f, g, h, t, data[63], 0xc67178f2e372532b);
	SHA512CALC(t, a, b, c, d, e, f, g, h, data[64], 0xca273eceea26619c);
	SHA512CALC(h, t, a, b, c, d, e, f, g, data[65], 0xd186b8c721c0c207);
	SHA512CALC(g, h, t, a, b, c, d, e, f, data[66], 0xeada7dd6cde0eb1e);
	SHA512CALC(f, g, h, t, a, b, c, d, e, data[67], 0xf57d4f7fee6ed178);
	SHA512CALC(e, f, g, h, t, a, b, c, d, data[68], 0x06f067aa72176fba);
	SHA512CALC(d, e, f, g, h, t, a, b, c, data[69], 0x0a637dc5a2c898a6);
	SHA512CALC(c, d, e, f, g, h, t, a, b, data[70], 0x113f9804bef90dae);
	SHA512CALC(b, c, d, e, f, g, h, t, a, data[71], 0x1b710b35131c471b);
	SHA512CALC(a, b, c, d, e, f, g, h, t, data[72], 0x28db77f523047d84);
	SHA512CALC(t, a, b, c, d, e, f, g, h, data[73], 0x32caab7b40c72493);
	SHA512CALC(h, t, a, b, c, d, e, f, g, data[74], 0x3c9ebe0a15c9bebc);
	SHA512CALC(g, h, t, a, b, c, d, e, f, data[75], 0x431d67c49c100d4c);
	SHA512CALC(f, g, h, t, a, b, c, d, e, data[76], 0x4cc5d4becb3e42b6);
	SHA512CALC(e, f, g, h, t, a, b, c, d, data[77], 0x597f299cfc657e2a);
	SHA512CALC(d, e, f, g, h, t, a, b, c, data[78], 0x5fcb6fab3ad6faec);
	SHA512CALC(c, d, e, f, g, h, t, a, b, data[79], 0x6c44198c4a475817);

	m_hash[0] += b;
	m_hash[1] += c;
	m_hash[2] += d;
	m_hash[3] += e;
	m_hash[4] += f;
	m_hash[5] += g;
	m_hash[6] += h;
	m_hash[7] += t;
}

UINT64 HashSHA512::Rotate(UINT64 a, BYTE b) {
	return a >> b | a << 64 - b;
}
