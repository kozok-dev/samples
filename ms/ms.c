#include <stdio.h>
#include <search.h>
#define CNT 10
#define SORTRANGE 2

char g_key_idx;

int cmp(const void *v1, const void *v2) {
	return ((const char *)v1)[g_key_idx] < ((const char *)v2)[g_key_idx] ? -1 :
	((const char *)v1)[g_key_idx] > ((const char *)v2)[g_key_idx] ? 1 : 0;
}

void main()
{
	char data[][3] = {
		{5, 'b', 15},
		{7, 'a', 30},
		{4, 'g', 30},
		{4, 'h', 32},
		{1, 'n', 15},
		{3, 'b', 15},
		{2, 'b', 32},
		{5, 'b', 21},
		{2, 'z', 15},
		{5, 'a', 30}
	}, start_idx, end_idx, i, j;

	for (g_key_idx = 0; g_key_idx < SORTRANGE; g_key_idx++) {
		start_idx = 0;	/* 並べ替え範囲の開始位置を設定。*/
		do {
			/* 並べ替え範囲の終了位置を設定。*/
			if (g_key_idx > 0)
				for (end_idx = start_idx + 1;
				end_idx < CNT && data[start_idx][g_key_idx - 1] == data[end_idx][g_key_idx - 1];
				end_idx++);
			else
				end_idx = CNT;

			qsort(data + start_idx, end_idx - start_idx, sizeof(data[0]), cmp);	/* 並べ替え。*/
			start_idx = end_idx;	/* 並べ替え範囲の開始位置を再設定。*/
		} while (start_idx < CNT);	/* 並べ替え範囲の開始位置がデータの終わりまで繰り返す。*/
	}

	for (i = 0; i < CNT; i++) printf("%d %c %d\n", data[i][0], data[i][1], data[i][2]);
	getch();
}
