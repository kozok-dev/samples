#include <windows.h>

char dec_resdata(DWORD addr, DWORD offset, DWORD *resname, char d, WORD **id, WORD idx, char flag, const FARPROC *func)
{
	BYTE *p1 = ((BYTE *)func[0]) + addr + offset, *p2;
	WORD cnt1, cnt2, i, *tmp;
	DWORD size, j;
	IMAGE_RESOURCE_DIRECTORY_ENTRY irde;

	if (!flag && d == 0 || flag && d == 1 && resname[0] == (DWORD)RT_ICON) {
		_asm {
			cmp flag, 0
			jne SETCNT
			mov cnt1, 0
			jmp SETCNTEND
SETCNT:
			mov cnt1, 0
SETCNTEND:
		}
		*(WORD *)&p1[14] = cnt1;
		p1 += 16;
		/* リソースルートディレクトリエントリ並べ替え。*/
		for (i = 0; i < cnt1 - 1; i++)
			for (j = i + 1; j < cnt1; j++) {
				if (*(DWORD *)&p1[i * 8] <= *(DWORD *)&p1[j * 8]) continue;
				irde = *(PIMAGE_RESOURCE_DIRECTORY_ENTRY)&p1[i * 8];
				*(PIMAGE_RESOURCE_DIRECTORY_ENTRY)&p1[i * 8] = *(PIMAGE_RESOURCE_DIRECTORY_ENTRY)&p1[j * 8];
				*(PIMAGE_RESOURCE_DIRECTORY_ENTRY)&p1[j * 8] = irde;
			}
	} else if (!flag && d == 1 && resname[0] == (DWORD)RT_GROUP_ICON) {
		_asm mov cnt1, 0
		*(WORD *)&p1[14] = cnt1;
		p1 += 16;
	} else {
		cnt1 = *(WORD *)&p1[14];
		p1 += 16;
	}

	for (i = 0; i < cnt1; i++) {
		resname[d] = *(DWORD *)&p1[i * 8];
		if (flag) {
			if (d == 0) {
				if (resname[0] != (DWORD)RT_ICON) continue;
			} else if (d == 1) {
				size = func[1]((HANDLE)func[5](), 0, *id) / sizeof(WORD);
				for (j = 0; j < size; j++) {
					if (resname[1] == *id[j]) {
						j = 0;
						break;
					}
				}
				if (j == 0) continue;
			}
		} else
			if (d == 0 && (resname[0] == (DWORD)RT_ICON || resname[0] == (DWORD)RT_VERSION ||
			resname[0] == (DWORD)RT_MANIFEST)) continue;

		offset = *(DWORD *)&p1[i * 8 + 4] & 0x7fffffff;

		if (p1[i * 8 + 7] >> 7) {
			if (d < 2) dec_resdata(addr, offset, resname, d + 1, id, i, flag, func);
		} else {
			p2 = ((BYTE *)func[0]) + addr + offset;
			size = *(DWORD *)&p2[4];
			p2 = ((BYTE *)func[0]) + *(DWORD *)p2;

			if (idx == 0 && resname[0] == (DWORD)RT_GROUP_ICON) {
				/* EXEアイコンID設定。*/
				cnt2 = *(WORD *)&p2[4];
				p2 += 6;
				for (size = j = 0; j < cnt2; p2 += 14, j++) {
					if ((tmp = (WORD *)func[2]((HANDLE)func[5](), 0, *id, size + sizeof(WORD))) == NULL) return 0;
					*id = tmp;
					tmp = *id + size / sizeof(WORD);
					size = func[1]((HANDLE)func[5](), 0, *id);
					*tmp = *(WORD *)&p2[12];
				}
			} else
				for (j = 0; j < size; j++) p2[j] = ~p2[j];
		}
	}
	return 1;
}

DWORD dec_res(const FARPROC *func)
{
	BYTE *p = &((BYTE *)func[0])[*(int *)&((BYTE *)func[0])[60]];
	WORD cnt = *(WORD *)&p[6], i, j, *id;
	DWORD resname[3], addr, resaddr = *(DWORD *)&p[136], flag = 1;

	if (resaddr > 0) {
		p += sizeof(IMAGE_NT_HEADERS);
		for (i = 0; i < cnt; p += sizeof(IMAGE_SECTION_HEADER), i++) {
			addr = *(DWORD *)&p[12];
			if (resaddr < addr || resaddr >= addr + *(DWORD *)&p[8]) continue;

			if ((id = (WORD *)func[4]((HANDLE)func[5](), 0, 0)) == NULL) return 0;

			/* リソースデータ複合化。*/
			if (dec_resdata(addr, 0, resname, 0, &id, 0, 0, func))	/* EXEアイコンIDも設定。*/
				dec_resdata(addr, 0, resname, 0, &id, 0, 1, func);	/* EXEアイコン以外のアイコンのみ。*/
			else flag = 0;

			func[3]((HANDLE)func[5](), 0, id);

			break;
		}
	}
	return flag;
}

__declspec(naked) void main()
{
	_asm {
	; kernel32.dllのアドレス取得。
	;  ebxに先頭アドレス。
	;  edxにPEヘッダのアドレス。
		push ebx
		mov ebx, [esp + 4]
		and ebx, 0ffff0000h
		mov ecx, 5
		push ebp
		push esi
		push edi
		mov ebp, esp
		sub esp, 56
		mov eax, 0
CHK_MZ:
		cmp word ptr [ebx], 5a4dh
		je CHK_PE
NOTFOUND:
		dec ecx
		cmp ecx, 0
		jle PROCEND
		sub ebx, 10000h
		jmp CHK_MZ
CHK_PE:
		mov edx, ebx
		add edx, [ebx + 60]
		cmp dword ptr [edx], 4550h
		jne NOTFOUND

	; "GetProcAddress"
		mov dword ptr [ebp - 16], 50746547h
		mov dword ptr [ebp - 12], 41636f72h
		mov dword ptr [ebp - 8], 65726464h
		mov dword ptr [ebp - 4], 7373h

	; エクスポート関数情報取得。
	;  [ebp - 20]に関数の数。
	;  [ebp - 24]に関数のアドレス。
	;  [ebp - 28]に序数表のアドレス。
	;  edxに関数表のアドレス。
		mov eax, ebx
		add eax, [edx + 120]
		add eax, 18h
		mov edx, [eax]
		mov [ebp - 20], edx
		mov edx, [eax + 4]
		add edx, ebx
		mov [ebp - 24], edx
		mov edx, [eax + 12]
		add edx, ebx
		mov [ebp - 28], edx
		mov edx, [eax + 8]
		add edx, ebx

	; GetProcAddress関数のアドレスを[ebp - 32]に取得。
		mov eax, 0
		mov ecx, 0
CMP_FUNC:
		mov esi, ebx
		add esi, [edx + eax]
		lea edi, [ebp - 16]
CHK_FUNC:
		cmpsb
		jne NEXT_FUNC1
		cmp [esi], 0
		je GOT_FUNC
		jmp CHK_FUNC
NEXT_FUNC1:
		inc ecx
		cmp ecx, [ebp - 20]
		jl NEXT_FUNC2
		mov eax, 0
		jmp PROCEND
NEXT_FUNC2:
		add eax, 4
		jmp CMP_FUNC
GOT_FUNC:
		shl ecx, 1
		mov edx, [ebp - 28]
		add edx, ecx
		mov eax, 0
		mov ax, [edx]
		shl eax, 2
		mov edx, [ebp - 24]
		add edx, eax
		mov edx, [edx]
		add edx, ebx
		mov [ebp - 32], edx

	; GetProcessHeap関数のアドレスを取得。
		mov dword ptr [ebp - 16], 50746547h
		mov dword ptr [ebp - 12], 65636f72h
		mov dword ptr [ebp - 8], 65487373h
		mov dword ptr [ebp - 4], 7061h
		lea eax, [ebp - 16]
		push eax
		push ebx
		call [ebp - 32]
		cmp eax, 0
		je PROCEND
		mov [ebp - 36], eax
	; HeapAlloc関数のアドレスを取得。
		mov dword ptr [ebp - 12], 70616548h
		mov dword ptr [ebp - 8], 6f6c6c41h
		mov dword ptr [ebp - 4], 63h
		lea eax, [ebp - 12]
		push eax
		push ebx
		call [ebp - 32]
		cmp eax, 0
		je PROCEND
		mov [ebp - 40], eax
	; HeapFree関数のアドレスを取得。
		mov dword ptr [ebp - 12], 70616548h
		mov dword ptr [ebp - 8], 65657246h
		mov dword ptr [ebp - 4], 0
		lea eax, [ebp - 12]
		push eax
		push ebx
		call [ebp - 32]
		cmp eax, 0
		je PROCEND
		mov [ebp - 44], eax
	; HeapReAlloc関数のアドレスを取得。
		mov dword ptr [ebp - 12], 70616548h
		mov dword ptr [ebp - 8], 6c416552h
		mov dword ptr [ebp - 4], 636f6ch
		lea eax, [ebp - 12]
		push eax
		push ebx
		call [ebp - 32]
		cmp eax, 0
		je PROCEND
		mov [ebp - 48], eax
	; HeapSize関数のアドレスを取得。
		mov dword ptr [ebp - 12], 70616548h
		mov dword ptr [ebp - 8], 657a6953h
		mov dword ptr [ebp - 4], 0
		lea eax, [ebp - 12]
		push eax
		push ebx
		call [ebp - 32]
		cmp eax, 0
		je PROCEND
		mov [ebp - 52], eax
	; VirtualQueryで自身の先頭アドレスを取得。
		mov dword ptr [ebp - 16], 74726956h
		mov dword ptr [ebp - 12], 516c6175h
		mov dword ptr [ebp - 8], 79726575h
		mov dword ptr [ebp - 4], 0
		lea eax, [ebp - 16]
		push eax
		push ebx
		call [ebp - 32]
		cmp eax, 0
		je PROCEND
		push 28
		lea ecx, [ebp - 28]
		push ecx
		call GET_EIP
GET_EIP:
		call eax
		mov eax, [ebp - 24]
		mov [ebp - 56], eax

	; リソースデータ複合化。
		lea eax, [ebp - 56]
		push eax
		call dec_res
		add esp, 4

	; 処理終了
PROCEND:
		mov esp, ebp
		pop edi
		pop esi
		pop ebp
		pop ebx
		cmp eax, 0
		jne PROCCON
		ret
PROCCON:
		ret
	}
}
