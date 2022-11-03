#include "appwindow.cpp"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int ret;
	HANDLE hmutex;
	AppWindow *paw;

	hmutex = CreateMutex(NULL, FALSE, TEXT("warun"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hmutex);
		MessageBox(NULL, TEXT("既に起動しています。"), TEXT("WaruN"), MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	paw = AppWindow::getObject();
	if (!paw->Create(hInstance)) {
		CloseHandle(hmutex);
		MessageBox(NULL, TEXT("起動に失敗しました。"), TEXT("WaruN"), MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	ret = paw->Run();
	CloseHandle(hmutex);
	return ret;
}


/*
シナリオスクリプト

[全体的な仕様]
・一部を除き、1行で1命令。
・大文字小文字は区別しない。
・命令の後にスペース区切りで引数を指定する。
・一部を除き、引数にtimeがある命令は同期的である。つまり、指定した時間が経過したら次の命令が実行される。
・演算はサポートしないのでエラーとなる。
・「;」を1行コメントとする。
・一部の命令で使用する検索ラベル名は「:」で始まる。「:」の後はスペースも含めてラベルとなる。
[引数の仕様]
・文字列を「"」で囲まなくてもよいが、スペースを含む文字列を指定する場合は囲む必要がある。複数行文字列は囲まないこと。
・引数は飽和される。例えば最大値255で300を指定すると255に丸められる。最小値も同様。
・NULLを指定する場合は空文字列を指定する。ただし、NULLを許可する命令は僅かである。
・booleanタイプの引数は「true」と「false」という文字列のみ許可する。大文字小文字はやはり区別しない。
・色に関する引数は0～255まで指定できる。
・timeは0～999まで指定できる。
・座標に関する引数は-999～999まで指定できる。
[エラー仕様]
・空白、不明な命令、構文エラーは無視する。
・引数が少ない場合は構文エラーとして無視し、引数が多い場合は余分な引数は無視する。

●BGCOLOR red green blue time
  単一色の背景スプライトを表示する。すでに表示している背景があればそれを隠すように表示する。

●BG filename time
  背景画像スプライトを表示する。すでに表示している背景があればそれを隠すように表示する。

●CHARA1 filename pos_x time disp_flag
  人物スプライトを表示する。または非表示にする。非表示の場合、time以外は無視される。

●CHARA2 filename pos_x time disp_flag
  CHARA1との違いはCHARA1によって表示された人物スプライトより表示優先順位が高い。

●COLOR red green blue time disp_flag
  単一色スプライトを背景と人物の上に表示する。または非表示にする。非表示の場合、time以外は無視される。

●TEXT name bracket
  text...
  TEXTEND
  一瞬だけ空表示してから文章を表示する。一瞬だけ空表示するのは文章が変わったことを表現するため。
  nameは最大10文字、文章は200文字まで。TEXTENDの前に空白があってはならない。
  TEXTERASEを実行するか、空文字を表示しない限り、文章は表示されたままになる。
  時間は指定できないが同期命令である。

●TEXTERASE
  文章をクリアする。

●SELECT text
  sel1 label1
  sel2 label2
  sel3 label3
  sel4 label4
  SELECTEND
  選択肢を表示する。ユーザが選択肢をクリックするのを待ち、クリックした選択肢のラベルに移動して
  そこから後は通常通り命令を実行する。ラベルを空文字にした場合はラベルへの移動はせず、
  そのまま次の命令を実行する。選択肢の数は2～4個にできる。ラベルの検索は現在位置から下が対象である。
  sel1～sel4は30文字まで。label1～label4は15文字まで。

●WAIT
  パネルのクリック可能目印を表示し、ユーザがパネルの文章をクリックするのを待つ。
  パネルの文章をクリックしないと反応しないので、パネルの文章が表示されている必要がある。

●PAUSE time
  timeの間だけ停止する。

●GOTO label
  指定したラベルに移動してそこから後は通常通り命令を実行する。ラベルの検索は現在位置から下が対象である。
  label1～label4は15文字まで。

●BGNAME text
  背景名を表示する。10文字まで。

●DAY text
  日を表示する。5文字まで。

●SOUND filename idx
  効果音等のループしない音声を再生する。WAITでユーザがパネルの文章をクリックしても停止する。idxは1～5まで。

●BGM filename intro_filename
  音楽等のループする音声を再生する。停止するにはBGMSTOPを使う。

●BGMSTOP
  BGMで再生している音声を停止する。

●VAR SET num
  変数にnumを設定する。numは-100～100まで。

●VAR ADD num
  変数にnumを加算する。numを負数にすると減算になる。変数の範囲は-100～100までであることに注意。

●IF VAR = num GOTO label
  変数をnumを比較し、条件を満たせばGOTO命令を実行する。GOTO命令の仕様は他と同じである。
  numは-100～100まで。比較には=、!=、<、>、<=、>=を使用できる。

●END type
  エンディングに移行する。
*/
