#include "appwindow.cpp"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int ret;
	HANDLE hmutex;
	AppWindow *paw;

	hmutex = CreateMutex(NULL, FALSE, TEXT("warun"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hmutex);
		MessageBox(NULL, TEXT("���ɋN�����Ă��܂��B"), TEXT("WaruN"), MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	paw = AppWindow::getObject();
	if (!paw->Create(hInstance)) {
		CloseHandle(hmutex);
		MessageBox(NULL, TEXT("�N���Ɏ��s���܂����B"), TEXT("WaruN"), MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	ret = paw->Run();
	CloseHandle(hmutex);
	return ret;
}


/*
�V�i���I�X�N���v�g

[�S�̓I�Ȏd�l]
�E�ꕔ�������A1�s��1���߁B
�E�啶���������͋�ʂ��Ȃ��B
�E���߂̌�ɃX�y�[�X��؂�ň������w�肷��B
�E�ꕔ�������A������time�����閽�߂͓����I�ł���B�܂�A�w�肵�����Ԃ��o�߂����玟�̖��߂����s�����B
�E���Z�̓T�|�[�g���Ȃ��̂ŃG���[�ƂȂ�B
�E�u;�v��1�s�R�����g�Ƃ���B
�E�ꕔ�̖��߂Ŏg�p���錟�����x�����́u:�v�Ŏn�܂�B�u:�v�̌�̓X�y�[�X���܂߂ă��x���ƂȂ�B
[�����̎d�l]
�E��������u"�v�ň͂܂Ȃ��Ă��悢���A�X�y�[�X���܂ޕ�������w�肷��ꍇ�͈͂ޕK�v������B�����s������͈͂܂Ȃ����ƁB
�E�����͖O�a�����B�Ⴆ�΍ő�l255��300���w�肷���255�Ɋۂ߂���B�ŏ��l�����l�B
�ENULL���w�肷��ꍇ�͋󕶎�����w�肷��B�������ANULL�������閽�߂͋͂��ł���B
�Eboolean�^�C�v�̈����́utrue�v�Ɓufalse�v�Ƃ���������̂݋�����B�啶���������͂�͂��ʂ��Ȃ��B
�E�F�Ɋւ��������0�`255�܂Ŏw��ł���B
�Etime��0�`999�܂Ŏw��ł���B
�E���W�Ɋւ��������-999�`999�܂Ŏw��ł���B
[�G���[�d�l]
�E�󔒁A�s���Ȗ��߁A�\���G���[�͖�������B
�E���������Ȃ��ꍇ�͍\���G���[�Ƃ��Ė������A�����������ꍇ�͗]���Ȉ����͖�������B

��BGCOLOR red green blue time
  �P��F�̔w�i�X�v���C�g��\������B���łɕ\�����Ă���w�i������΂�����B���悤�ɕ\������B

��BG filename time
  �w�i�摜�X�v���C�g��\������B���łɕ\�����Ă���w�i������΂�����B���悤�ɕ\������B

��CHARA1 filename pos_x time disp_flag
  �l���X�v���C�g��\������B�܂��͔�\���ɂ���B��\���̏ꍇ�Atime�ȊO�͖��������B

��CHARA2 filename pos_x time disp_flag
  CHARA1�Ƃ̈Ⴂ��CHARA1�ɂ���ĕ\�����ꂽ�l���X�v���C�g���\���D�揇�ʂ������B

��COLOR red green blue time disp_flag
  �P��F�X�v���C�g��w�i�Ɛl���̏�ɕ\������B�܂��͔�\���ɂ���B��\���̏ꍇ�Atime�ȊO�͖��������B

��TEXT name bracket
  text...
  TEXTEND
  ��u������\�����Ă��當�͂�\������B��u������\������͕̂��͂��ς�������Ƃ�\�����邽�߁B
  name�͍ő�10�����A���͂�200�����܂ŁBTEXTEND�̑O�ɋ󔒂������Ă͂Ȃ�Ȃ��B
  TEXTERASE�����s���邩�A�󕶎���\�����Ȃ�����A���͕͂\�����ꂽ�܂܂ɂȂ�B
  ���Ԃ͎w��ł��Ȃ����������߂ł���B

��TEXTERASE
  ���͂��N���A����B

��SELECT text
  sel1 label1
  sel2 label2
  sel3 label3
  sel4 label4
  SELECTEND
  �I������\������B���[�U���I�������N���b�N����̂�҂��A�N���b�N�����I�����̃��x���Ɉړ�����
  ���������͒ʏ�ʂ薽�߂����s����B���x�����󕶎��ɂ����ꍇ�̓��x���ւ̈ړ��͂����A
  ���̂܂܎��̖��߂����s����B�I�����̐���2�`4�ɂł���B���x���̌����͌��݈ʒu���牺���Ώۂł���B
  sel1�`sel4��30�����܂ŁBlabel1�`label4��15�����܂ŁB

��WAIT
  �p�l���̃N���b�N�\�ڈ��\�����A���[�U���p�l���̕��͂��N���b�N����̂�҂B
  �p�l���̕��͂��N���b�N���Ȃ��Ɣ������Ȃ��̂ŁA�p�l���̕��͂��\������Ă���K�v������B

��PAUSE time
  time�̊Ԃ�����~����B

��GOTO label
  �w�肵�����x���Ɉړ����Ă��������͒ʏ�ʂ薽�߂����s����B���x���̌����͌��݈ʒu���牺���Ώۂł���B
  label1�`label4��15�����܂ŁB

��BGNAME text
  �w�i����\������B10�����܂ŁB

��DAY text
  ����\������B5�����܂ŁB

��SOUND filename idx
  ���ʉ����̃��[�v���Ȃ��������Đ�����BWAIT�Ń��[�U���p�l���̕��͂��N���b�N���Ă���~����Bidx��1�`5�܂ŁB

��BGM filename intro_filename
  ���y���̃��[�v���鉹�����Đ�����B��~����ɂ�BGMSTOP���g���B

��BGMSTOP
  BGM�ōĐ����Ă��鉹�����~����B

��VAR SET num
  �ϐ���num��ݒ肷��Bnum��-100�`100�܂ŁB

��VAR ADD num
  �ϐ���num�����Z����Bnum�𕉐��ɂ���ƌ��Z�ɂȂ�B�ϐ��͈̔͂�-100�`100�܂łł��邱�Ƃɒ��ӁB

��IF VAR = num GOTO label
  �ϐ���num���r���A�����𖞂�����GOTO���߂����s����BGOTO���߂̎d�l�͑��Ɠ����ł���B
  num��-100�`100�܂ŁB��r�ɂ�=�A!=�A<�A>�A<=�A>=���g�p�ł���B

��END type
  �G���f�B���O�Ɉڍs����B
*/
