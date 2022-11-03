#pragma once
#include <windows.h>
#define MD5CALC1(a, b, c, d, e, f, g) a += (b & c | ~b & d) + e + g; a = (a << f | a >> 32 - f) + b;
#define MD5CALC2(a, b, c, d, e, f, g) a += (b & d | c & ~d) + e + g; a = (a << f | a >> 32 - f) + b;
#define MD5CALC3(a, b, c, d, e, f, g) a += (b ^ c ^ d) + e + g; a = (a << f | a >> 32 - f) + b;
#define MD5CALC4(a, b, c, d, e, f, g) a += (c ^ (b | ~d)) + e + g; a = (a << f | a >> 32 - f) + b;

// MD5計算クラス。
class MD5 {
public:
	// 初期化。
	void Init() {
		m_len[0] = m_len[1] = 0;
		m_hash[0] = 0x67452301;
		m_hash[1] = 0xefcdab89;
		m_hash[2] = 0x98badcfe;
		m_hash[3] = 0x10325476;
	}

	// MD5更新。
	// 引数：pdata データ。
	//       len データ長。
	void Update(BYTE *pdata, UINT len) {
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

			Calc((UINT *)m_buf);
			for (; j + 63 < len; j += 64) Calc((UINT *)(pdata + j));
		}

		CopyMemory(m_buf + i, pdata + j, len - j);
	}

	// データからMD5計算。
	// 引数：pdata データ。
	void Calc(UINT *pdata) {
		UINT a = m_hash[0], b = m_hash[1], c = m_hash[2], d = m_hash[3];

		MD5CALC1(a, b, c, d, pdata[ 0],  7, 0xd76aa478);
		MD5CALC1(d, a, b, c, pdata[ 1], 12, 0xe8c7b756);
		MD5CALC1(c, d, a, b, pdata[ 2], 17, 0x242070db);
		MD5CALC1(b, c, d, a, pdata[ 3], 22, 0xc1bdceee);
		MD5CALC1(a, b, c, d, pdata[ 4],  7, 0xf57c0faf);
		MD5CALC1(d, a, b, c, pdata[ 5], 12, 0x4787c62a);
		MD5CALC1(c, d, a, b, pdata[ 6], 17, 0xa8304613);
		MD5CALC1(b, c, d, a, pdata[ 7], 22, 0xfd469501);
		MD5CALC1(a, b, c, d, pdata[ 8],  7, 0x698098d8);
		MD5CALC1(d, a, b, c, pdata[ 9], 12, 0x8b44f7af);
		MD5CALC1(c, d, a, b, pdata[10], 17, 0xffff5bb1);
		MD5CALC1(b, c, d, a, pdata[11], 22, 0x895cd7be);
		MD5CALC1(a, b, c, d, pdata[12],  7, 0x6b901122);
		MD5CALC1(d, a, b, c, pdata[13], 12, 0xfd987193);
		MD5CALC1(c, d, a, b, pdata[14], 17, 0xa679438e);
		MD5CALC1(b, c, d, a, pdata[15], 22, 0x49b40821);

		MD5CALC2(a, b, c, d, pdata[ 1],  5, 0xf61e2562);
		MD5CALC2(d, a, b, c, pdata[ 6],  9, 0xc040b340);
		MD5CALC2(c, d, a, b, pdata[11], 14, 0x265e5a51);
		MD5CALC2(b, c, d, a, pdata[ 0], 20, 0xe9b6c7aa);
		MD5CALC2(a, b, c, d, pdata[ 5],  5, 0xd62f105d);
		MD5CALC2(d, a, b, c, pdata[10],  9, 0x02441453);
		MD5CALC2(c, d, a, b, pdata[15], 14, 0xd8a1e681);
		MD5CALC2(b, c, d, a, pdata[ 4], 20, 0xe7d3fbc8);
		MD5CALC2(a, b, c, d, pdata[ 9],  5, 0x21e1cde6);
		MD5CALC2(d, a, b, c, pdata[14],  9, 0xc33707d6);
		MD5CALC2(c, d, a, b, pdata[ 3], 14, 0xf4d50d87);
		MD5CALC2(b, c, d, a, pdata[ 8], 20, 0x455a14ed);
		MD5CALC2(a, b, c, d, pdata[13],  5, 0xa9e3e905);
		MD5CALC2(d, a, b, c, pdata[ 2],  9, 0xfcefa3f8);
		MD5CALC2(c, d, a, b, pdata[ 7], 14, 0x676f02d9);
		MD5CALC2(b, c, d, a, pdata[12], 20, 0x8d2a4c8a);

		MD5CALC3(a, b, c, d, pdata[ 5],  4, 0xfffa3942);
		MD5CALC3(d, a, b, c, pdata[ 8], 11, 0x8771f681);
		MD5CALC3(c, d, a, b, pdata[11], 16, 0x6d9d6122);
		MD5CALC3(b, c, d, a, pdata[14], 23, 0xfde5380c);
		MD5CALC3(a, b, c, d, pdata[ 1],  4, 0xa4beea44);
		MD5CALC3(d, a, b, c, pdata[ 4], 11, 0x4bdecfa9);
		MD5CALC3(c, d, a, b, pdata[ 7], 16, 0xf6bb4b60);
		MD5CALC3(b, c, d, a, pdata[10], 23, 0xbebfbc70);
		MD5CALC3(a, b, c, d, pdata[13],  4, 0x289b7ec6);
		MD5CALC3(d, a, b, c, pdata[ 0], 11, 0xeaa127fa);
		MD5CALC3(c, d, a, b, pdata[ 3], 16, 0xd4ef3085);
		MD5CALC3(b, c, d, a, pdata[ 6], 23, 0x04881d05);
		MD5CALC3(a, b, c, d, pdata[ 9],  4, 0xd9d4d039);
		MD5CALC3(d, a, b, c, pdata[12], 11, 0xe6db99e5);
		MD5CALC3(c, d, a, b, pdata[15], 16, 0x1fa27cf8);
		MD5CALC3(b, c, d, a, pdata[ 2], 23, 0xc4ac5665);

		MD5CALC4(a, b, c, d, pdata[ 0],  6, 0xf4292244);
		MD5CALC4(d, a, b, c, pdata[ 7], 10, 0x432aff97);
		MD5CALC4(c, d, a, b, pdata[14], 15, 0xab9423a7);
		MD5CALC4(b, c, d, a, pdata[ 5], 21, 0xfc93a039);
		MD5CALC4(a, b, c, d, pdata[12],  6, 0x655b59c3);
		MD5CALC4(d, a, b, c, pdata[ 3], 10, 0x8f0ccc92);
		MD5CALC4(c, d, a, b, pdata[10], 15, 0xffeff47d);
		MD5CALC4(b, c, d, a, pdata[ 1], 21, 0x85845dd1);
		MD5CALC4(a, b, c, d, pdata[ 8],  6, 0x6fa87e4f);
		MD5CALC4(d, a, b, c, pdata[15], 10, 0xfe2ce6e0);
		MD5CALC4(c, d, a, b, pdata[ 6], 15, 0xa3014314);
		MD5CALC4(b, c, d, a, pdata[13], 21, 0x4e0811a1);
		MD5CALC4(a, b, c, d, pdata[ 4],  6, 0xf7537e82);
		MD5CALC4(d, a, b, c, pdata[11], 10, 0xbd3af235);
		MD5CALC4(c, d, a, b, pdata[ 2], 15, 0x2ad7d2bb);
		MD5CALC4(b, c, d, a, pdata[ 9], 21, 0xeb86d391);

		m_hash[0] += a;
		m_hash[1] += b;
		m_hash[2] += c;
		m_hash[3] += d;
	}

	// MD5最終更新後、MD5を取得。
	// 引数：pdata MD5格納先。
	void Final(BYTE *pdata) {
		BYTE buf[64], i;
		UINT len[2];

		len[0] = m_len[0];
		len[1] = m_len[1];

		i = m_len[0] >> 3 & 63;
		i = (i < 56 ? 56 : 120) - i;
		buf[0] = 0x80;
		ZeroMemory(buf + 1, i - 1);
		Update(buf, i);

		Update((BYTE *)len, 8);
		CopyMemory(pdata, m_hash, 16);
	}

protected:
	UINT m_hash[4], m_len[2];
	BYTE m_buf[64];
};
