#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _WIN32_IE 0x0500
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#define WNDID_EFXNAME 1

struct efxname {
	LPTSTR name, desc;
} g_efxname[] = {
	{TEXT("00�FReverb"),
		TEXT("���o�[�u�͉��Ɏc����]�C��^�����s���̂��鉹����n��o���G�t�F�N�g�ł��B�R���T�[�g�E�z�[���ŉ��t�𒮂��Ă���悤�Ȍ��ʂ�����܂��B")},
	{TEXT("00�FChorus"),
		TEXT("�R�[���X�͉������g�債�A���Ɍ��݂Ə�����^���܂��B")},
	{TEXT("00�FDelay"),
		TEXT("�f�B���C�͉���x�点�A�R�т��̂悤�Ȍ��ʂ����o���܂��B�܂��A�Z���f�B���C���������Əd�˂āA���Ɍ��݂ƍL������������邱�Ƃ��ł��܂��B")},
	{TEXT("01�F���F��ς���(�t�B���^�[�n) Stereo-EQ(�X�e���I�E�C�R���C�U�[)"),
		TEXT("4�o���h(���A����~2�A����)�̃X�e���I�̃C�R���C�U�[�ł��B")},
	{TEXT("02�F���F��ς���(�t�B���^�[�n) Spectrum(�X�y�N�g����)"),
		TEXT("�X�y�N�g�����̓t�B���^�[�̈��ŁA����̎��g���̃��x���𑝌������ĉ��F��ς��܂��B�C�R���C�U�[�Ɠ��������Ă��܂����A���F�̃N�Z������̂ɍœK��8�̎��g�������߂��Ă��܂��̂ŁA�������̂��鉹�����܂��B")},
	{TEXT("03�F���F��ς���(�t�B���^�[�n) Enhancer(�G���n���T�[)"),
		TEXT("�G���n���T�[�͍���̔{���������R���g���[�����邱�Ƃɂ��A���Ƀ����n����������A���k�P���悭�����肵�܂��B")},
	{TEXT("04�F���F��ς���(�t�B���^�[�n) Humanizer(�q���[�}�i�C�U�[)"),
		TEXT("�l�Ԃ̐��̂悤�ɁA���ɕꉹ�����邱�Ƃ��ł��܂��B")},
	{TEXT("05�F����c�܂���(�c�݌n) Overdrive(�I�[�o�[�h���C�u)"),
		TEXT("�I�[�o�[�h���C�u�͐^��ǃA���v�̂悤�Ȃ��炩�Șc�݂������܂��B")},
	{TEXT("06�F����c�܂���(�c�݌n) Distortion(�f�B�X�g�[�V����)"),
		TEXT("�f�B�X�g�[�V�����̓I�[�o�[�h���C�u�����������c�݂������܂��B")},
	{TEXT("07�F����h�炷(���W�����[�V�����n) Phaser(�t�F�C�U�[)"),
		TEXT("�t�F�C�U�[�͌����Ɉʑ������炵�����������邱�Ƃŉ��ɂ��˂��^���A�L���芴�≜�s�����������܂��B")},
	{TEXT("08�F����h�炷(���W�����[�V�����n) Auto Wah(�I�[�g�E���E)"),
		TEXT("�I�[�g�E���E�̓t�B���^�[�������I�ɓ��������ƂŁA���F�������I�ɕω������܂��B")},
	{TEXT("09�F����h�炷(���W�����[�V�����n) Rotary(���[�^���[)"),
		TEXT("���[�^���[�͉��N�̉�]�X�s�[�J�[�E�T�E���h���V�~�����[�g�����G�t�F�N�^�[�ł��B����ƒ��̃��[�^�[�̓�������ꂼ��Ɨ����Đݒ�ł���̂ŁA�Ɠ��̂��˂芴�����A���ɍČ��ł��܂��B�I���K���ɍł����ʂ�����܂��B")},
	{TEXT("10�F����h�炷(���W�����[�V�����n) Stereo Flanger(�X�e���I�E�t�����W���[)"),
		TEXT("�X�e���I�d�l�̃t�����W���[�ł��B�W�F�b�g�@�̏㏸��/���~���̂悤�ȋ����I�ȋ����������܂��B�t�B���^�[���g���ăt�����W���[���̉����𒲐߂ł��܂��B")},
	{TEXT("11�F����h�炷(���W�����[�V�����n) Step Flanger(�X�e�b�v�E�t�����W���[)"),
		TEXT("�X�e�b�v�E�t�����W���[�̓t�����W���[���̃s�b�`���i�K�I�ɕω�����t�����W���[�ł��B")},
	{TEXT("12�F����h�炷(���W�����[�V�����n) Tremolo(�g������)"),
		TEXT("�g�������͉��ʂ������I�ɓ��������ƂŁA���Ƀg���������ʂ�^���܂��B")},
	{TEXT("13�F����h�炷(���W�����[�V�����n) Auto Pan(�I�[�g�E�p��)"),
		TEXT("�I�[�g�E�p���͉��̒�ʂ������I�ɕω������܂��B")},
	{TEXT("14�F���x����ς���(�R���v���b�T�[�n) Compressor(�R���v���b�T�[)"),
		TEXT("�R���v���b�T�[�͑傫�ȃ��x���̉���}���A�����ȃ��x���̉��������グ�邱�ƂŁA���ʂ̃o���c�L��}���܂��B")},
	{TEXT("15�F���x����ς���(�R���v���b�T�[�n) Limiter(���~�b�^�[)"),
		TEXT("���~�b�^�[�͎w��̉��ʃ��x�����傫�ȉ��݂̂����k���A���̘c�݂�}���܂��B")},
	{TEXT("16�F�����g����(�R�[���X�n) Hexa Chorus(�w�L�T�E�R�[���X)"),
		TEXT("�w�L�T�E�R�[���X�͉��Ɍ��݂ƍL�����^����6���R�[���X(6�̃R�[���X�����d�Ȃ�)�ł��B")},
	{TEXT("17�F�����g����(�R�[���X�n) Tremolo Chorus(�g�������E�R�[���X)"),
		TEXT("�g�������E�R�[���X�̓g����������(���ʂ������I�ɗh�炷)��������R�[���X�ł��B")},
	{TEXT("18�F�����g����(�R�[���X�n) Stereo Chorus(�X�e���I�E�R�[���X)"),
		TEXT("�X�e���I�d�l�̃R�[���X�ł��B�t�B���^�[���g���ăR�[���X���̉����𒲐߂ł��܂��B")},
	{TEXT("19�F�����g����(�R�[���X�n) Space D(�X�y�[�XD)"),
		TEXT("�X�y�[�XD��2���̃��W�����[�V�������X�e���I�ł����鑽�d�R�[���X�ł��B�ϒ����͂���܂��񂪁A�������̂���R�[���X���ʂ������܂��B")},
	{TEXT("20�F�����g����(�R�[���X�n) 3D Chorus(3D�R�[���X)"),
		TEXT("�R�[���X����3D���ʂ�������܂��B�R�[���X����90�x����90�x�E�ɒ�ʂ��܂��B")},
	{TEXT("21�F������������(�f�B���C/���o�[�u�n) Stereo Delay(�X�e���I�E�f�B���C)"),
		TEXT("�X�e���I�d�l�̃f�B���C�ł��B")},
	{TEXT("22�F������������(�f�B���C/���o�[�u�n) Mod Delay(���W�����[�V�����E�f�B���C)"),
		TEXT("���W�����[�V�����E�f�B���C�̓f�B���C���ɗh�ꂪ��������G�t�F�N�^�[�ł��B�t�����W���[�̂悤�Ȍ��ʂ������܂��B")},
	{TEXT("23�F������������(�f�B���C/���o�[�u�n) 3 Tap Delay(�g���v���E�^�b�v�E�f�B���C)"),
		TEXT("�g���v���E�^�b�v�E�f�B���C�́A�����A���A�E��3�����Ƀf�B���C�����点��G�t�F�N�^�[�ł��B")},
	{TEXT("24�F������������(�f�B���C/���o�[�u�n) 4 Tap Delay(�N�A�h���v���E�^�b�v�E�f�B���C)"),
		TEXT("�N�A�h���v���E�^�b�v�E�f�B���C��4�̃f�B���C�������Ă��܂��B")},
	{TEXT("25�F������������(�f�B���C/���o�[�u�n) Tm Ctrl Delay(�^�C���E�R���g���[���E�f�B���C)"),
		TEXT("EFX C.Src�Őݒ肵���w��̃R���g���[���[���g���ăf�B���C�E�^�C���ƃs�b�`�����A���^�C���ŃR���g���[���ł��܂��B�f�B���C�E�^�C���𒷂�����ƃs�b�`��������A�Z������ƃs�b�`���オ��܂��B")},
	{TEXT("26�F������������(�f�B���C/���o�[�u�n) Reverb(���o�[�u)"),
		TEXT("���o�[�u�͌����Ɏc�����������A��Ԃ̍L�����V�~�����[�g���܂��B")},
	{TEXT("27�F������������(�f�B���C/���o�[�u�n) Gate Reverb(�Q�[�g�E���o�[�u)"),
		TEXT("�Q�[�g�E���o�[�u�͎c������r���ŃJ�b�g���郊�o�[�u�ł��B")},
	{TEXT("28�F������������(�f�B���C/���o�[�u�n) 3D Delay(3D�f�B���C)"),
		TEXT("�f�B���C����3D���ʂ�������܂��B�f�B���C����90�x����90�x�E�ɒ�ʂ��܂��B")},
	{TEXT("29�F������ς���(�s�b�`�E�V�t�g�n) 2 Pitch Shifter(2�{�C�X�E�s�b�`�E�V�t�^�[)"),
		TEXT("�s�b�`�E�V�t�^�[�͌����̃s�b�`�����炷�G�t�F�N�^�[�ł��B2�{�C�X�E�s�b�`�E�V�t�^�[��2�̃s�b�`�E�V�t�^�[�������A�s�b�`�����点��2�̉��������ɏd�˂Ė炷���Ƃ��ł��܂��B")},
	{TEXT("30�F������ς���(�s�b�`�E�V�t�g�n) Fb P.Shifter(�t�B�[�h�o�b�N�E�s�b�`�E�V�t�^�[)"),
		TEXT("�s�b�`�E�V�t�g������͂ɖ߂����Ƃ��ł���s�b�`�E�V�t�^�[�ł��B")},
	{TEXT("31�F���̑� 3D Auto(3D�I�[�g)"),
		TEXT("3D�I�[�g�͒�ʂ���]������G�t�F�N�g�ł��B")},
	{TEXT("32�F���̑� 3D Manual(3D�}�j���A��)"),
		TEXT("3D�̌��ʂ�C�ӂ̈ʒu�ɒ�ʂ����܂��B")},
	{TEXT("33�F���̑� Lo-Fi 1(���[�E�t�@�C1)"),
		TEXT("���[�E�t�@�C1�͉�����e������G�t�F�N�^�[�ł��B")},
	{TEXT("34�F���̑� Lo-Fi 2(���[�E�t�@�C2)"),
		TEXT("���[�E�t�@�C2�͉�����e���������ɁA���낢��ȃm�C�Y�������邱�Ƃ��ł��܂��B\n�� R.Detune(���W�I�E�f�`���[��)�AW/P Level(�z���C�g/�s���N�E�m�C�Y�E���x��)�ADisc Nz Lev(�f�B�X�N�E�m�C�Y�E���x��)�AHum Level(�n���E�m�C�Y�E���x��)�̒l��傫������ƁA�������Ă��Ȃ��Ƃ��ɂ��m�C�Y������܂��B")},
	{TEXT("35�F2��ނ̃G�t�F�N�g����ڑ�(����2) OD��Chorus(�I�[�o�[�h���C�u���R�[���X)"),
		TEXT("�I�[�o�[�h���C�u�ƃR�[���X�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("36�F2��ނ̃G�t�F�N�g����ڑ�(����2) OD��Flanger(�I�[�o�[�h���C�u���t�����W���[)"),
		TEXT("�I�[�o�[�h���C�u�ƃt�����W���[�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("37�F2��ނ̃G�t�F�N�g����ڑ�(����2) OD��Delay(�I�[�o�[�h���C�u���f�B���C)"),
		TEXT("�I�[�o�[�h���C�u�ƃf�B���C�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("38�F2��ނ̃G�t�F�N�g����ڑ�(����2) DS��Chorus(�f�B�X�g�[�V�������R�[���X)"),
		TEXT("�f�B�X�g�[�V�����ƃR�[���X�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("39�F2��ނ̃G�t�F�N�g����ڑ�(����2) DS��Flanger(�f�B�X�g�[�V�������t�����W���[)"),
		TEXT("�f�B�X�g�[�V�����ƃt�����W���[�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("40�F2��ނ̃G�t�F�N�g����ڑ�(����2) DS��Delay(�f�B�X�g�[�V�������f�B���C)"),
		TEXT("�f�B�X�g�[�V�����ƃf�B���C�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("41�F2��ނ̃G�t�F�N�g����ڑ�(����2) EH��Chorus(�G���n���T�[���R�[���X)"),
		TEXT("�G���n���T�[�ƃR�[���X�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("42�F2��ނ̃G�t�F�N�g����ڑ�(����2) EH��Flanger(�G���n���T�[���t�����W���[)"),
		TEXT("�G���n���T�[�ƃt�����W���[�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("43�F2��ނ̃G�t�F�N�g����ڑ�(����2) EH��Delay(�G���n���T�[���f�B���C)"),
		TEXT("�G���n���T�[�ƃf�B���C�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("44�F2��ނ̃G�t�F�N�g����ڑ�(����2) Cho��Delay(�R�[���X���f�B���C)"),
		TEXT("�R�[���X�ƃf�B���C�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("45�F2��ނ̃G�t�F�N�g����ڑ�(����2) FL��Delay(�t�����W���[���f�B���C)"),
		TEXT("�t�����W���[�ƃf�B���C�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("46�F2��ނ̃G�t�F�N�g����ڑ�(����2) Cho��Flanger(�R�[���X���t�����W���[)"),
		TEXT("�R�[���X�ƃt�����W���[�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("47�F3��ވȏ�̃G�t�F�N�g����ڑ�(����3/����4/����5) Rotary Multi(���[�^���[�E�}���`)"),
		TEXT("�I�[�o�[�h���C�u(OD)��3�o���h�E�C�R���C�U�[(EQ)�ƃ��[�^���[(RT)�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("48�F3��ވȏ�̃G�t�F�N�g����ڑ�(����3/����4/����5) GTR Multi 1(�M�^�[�E�}���`1)"),
		TEXT("�M�^�[�E�}���`1�̓R���v���b�T�[(Cmp)�A�I�[�o�[�h���C�u�܂��̓f�B�X�g�[�V����(OD)�A�R�[���X�܂��̓t�����W���[(CF)�A�f�B���C(Dly)�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("49�F3��ވȏ�̃G�t�F�N�g����ڑ�(����3/����4/����5) GTR Multi 2(�M�^�[�E�}���`2)"),
		TEXT("�M�^�[�E�}���`2�̓R���v���b�T�[(Cmp)�A�I�[�o�[�h���C�u�܂��̓f�B�X�g�[�V����(OD)�A�C�R���C�U�[(EQ)�A�R�[���X�܂��̓t�����W���[(CF)�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("50�F3��ވȏ�̃G�t�F�N�g����ڑ�(����3/����4/����5) GTR Multi 3(�M�^�[�E�}���`3)"),
		TEXT("�M�^�[�E�}���`3�̓��E(Wah)�A�I�[�o�[�h���C�u�܂��̓f�B�X�g�[�V����(OD)�A�R�[���X�܂��̓t�����W���[(CF)�A�f�B���C(Dly)�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("51�F3��ވȏ�̃G�t�F�N�g����ڑ�(����3/����4/����5) Clean Gt Multi1(�N���[���E�M�^�[�E�}���`1)"),
		TEXT("�R���v���b�T�[(Cmp)�A�C�R���C�U�[(EQ)�A�R�[���X�܂��̓t�����W���[(CF)�A�f�B���C(Dly)�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("52�F3��ވȏ�̃G�t�F�N�g����ڑ�(����3/����4/����5) Clean Gt Multi2(�N���[���E�M�^�[�E�}���`2)"),
		TEXT("�I�[�g�E���E(AW)�A�C�R���C�U�[(EQ)�A�R�[���X�܂��̓t�����W���[(CF)�A�f�B���C(Dly)�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("53�F3��ވȏ�̃G�t�F�N�g����ڑ�(����3/����4/����5) Bass Multi(�x�[�X�E�}���`)"),
		TEXT("�R���v���b�T�[(Cmp)�A�I�[�o�[�h���C�u�܂��̓f�B�X�g�[�V����(OD)�A�C�R���C�U�[(EQ)�A�R�[���X�܂��̓t�����W���[(CF)�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("54�F3��ވȏ�̃G�t�F�N�g����ڑ�(����3/����4/����5) Rhodes Multi(���[�Y�E�}���`)"),
		TEXT("�G���n���T�[(EH)�A�t�F�C�U�[(PH)�A�R�[���X�܂��̓t�����W���[(CF)�A�g�������܂��̓p��(TP)�𒼗�ɐڑ����Ă��܂��B")},
	{TEXT("55�F3��ވȏ�̃G�t�F�N�g����ڑ�(����3/����4/����5) Keyboard Multi(�L�[�{�[�h�E�}���`)"),
		TEXT("�����O�E���W�����[�^�[(RM)�A�C�R���C�U�[(EQ)�A�s�b�`�E�V�t�^�[(PS)�A�t�F�C�U�[(PH)�A�f�B���C(Dly)�𒼗�ɐڑ����Ă��܂��B�����O�E���W�����[�^�[�́A���͐M���ɐU���ϒ�(AM�ϒ�)�������邱�Ƃɂ��x���̂悤�ȉ����o�����Ƃ��ł���G�t�F�N�^�[�ł��B")},
	{TEXT("56�F2��ނ̃G�t�F�N�g�����ڑ�(����2) Cho/Delay(�R�[���X�A�f�B���C)"),
		TEXT("�R�[���X�ƃf�B���C�����ɐڑ����Ă��܂��B")},
	{TEXT("57�F2��ނ̃G�t�F�N�g�����ڑ�(����2) FL/Delay(�t�����W���[�A�f�B���C)"),
		TEXT("�t�����W���[�ƃf�B���C�����ɐڑ����Ă��܂��B")},
	{TEXT("58�F2��ނ̃G�t�F�N�g�����ڑ�(����2) Cho/Flanger(�R�[���X�A�t�����W���[)"),
		TEXT("�R�[���X�ƃt�����W���[�����ɐڑ����Ă��܂��B")},
	{TEXT("59�F2��ނ̃G�t�F�N�g�����ڑ�(����2) OD1/OD2(�I�[�o�[�h���C�u/�f�B�X�g�[�V����1�A2)"),
		TEXT("�I�[�o�[�h���C�u�܂��̓f�B�X�g�[�V������I�ׂ�G�t�F�N�^�[2�Z�b�g�����ɐڑ����Ă��܂��B")},
	{TEXT("60�F2��ނ̃G�t�F�N�g�����ڑ�(����2) OD/Rotary(�I�[�o�[�h���C�u/�f�B�X�g�[�V�����A���[�^���[)"),
		TEXT("�I�[�o�[�h���C�u�܂��̓f�B�X�g�[�V�����ƃ��[�^���[�����ɐڑ����Ă��܂��B")},
	{TEXT("61�F2��ނ̃G�t�F�N�g�����ڑ�(����2) OD/Phaser(�I�[�o�[�h���C�u/�f�B�X�g�[�V�����A�t�F�C�U�[)"),
		TEXT("�I�[�o�[�h���C�u�܂��̓f�B�X�g�[�V�����ƃt�F�C�U�[�����ɐڑ����Ă��܂��B")},
	{TEXT("62�F2��ނ̃G�t�F�N�g�����ڑ�(����2) OD/AutoWah(�I�[�o�[�h���C�u/�f�B�X�g�[�V�����A�I�[�g�E���E)"),
		TEXT("�I�[�o�[�h���C�u�܂��̓f�B�X�g�[�V�����ƃI�[�g�E���E�����ɐڑ����Ă��܂��B")},
	{TEXT("63�F2��ނ̃G�t�F�N�g�����ڑ�(����2) PH/Rotary(�t�F�C�U�[�A���[�^���[)"),
		TEXT("�t�F�C�U�[�ƃ��[�^���[�����ɐڑ����Ă��܂��B")},
	{TEXT("64�F2��ނ̃G�t�F�N�g�����ڑ�(����2) PH/Auto Wah(�t�F�C�U�[�A�I�[�g�E���E)"),
		TEXT("�t�F�C�U�[�ƃI�[�g�E���E�����ɐڑ����Ă��܂��B")}
};
HINSTANCE g_hinst;
HFONT g_hfont;

class Efx {
	static const char MAX_WND = 30;	// �ő�p�����[�^���B
	static const char MAX_VALUE = 15;	// �l�̍ő啶�����B
	static const char MAX_SYSEX = 35;	// �V�X�e���G�N�X�N���[�V�u�̍ő啶�����B
	static const short VALUE_RANGE = 128;	// �ݒ�ł���l�͈̔́B
	/*
	* [�R���g���[��ID�d�l]
	*  0001  nnnn             nnnnnnnn
	*  1�Œ� �R���g���[����� �p�����[�^�C���f�b�N�X
	*/
	static const WORD IDMASK_CB_VAL = 0x1100;	// �l�ݒ�R���{�{�b�N�XID�}�X�N�B
	static const WORD IDMASK_EDT_SYSEX = 0x1200;	// �V�X�e���G�N�X�N���[�V�u�e�L�X�g�{�b�N�XID�}�X�N�B
	static const WORD IDMASK_BTN_COPY = 0x1300;	// �V�X�e���G�N�X�N���[�V�u�R�s�[�{�^��ID�}�X�N�B
	static const WORD MASK_CTRLID = 0xff00;	// �R���g���[��ID�}�X�N�B
	static const WORD MASK_INDEX = 0x00ff;	// �p�����[�^�C���f�b�N�X�}�X�N�B

public:
	static struct ParamValue {
		typedef BYTE value_type;
		value_type value;
		LPTSTR text;
	};

private:
	class Param {
		char m_addr, m_def_idx;
		ParamValue::value_type m_value[VALUE_RANGE];
		HWND m_hwstc_name, m_hwtt_name, m_hwcb_value, m_hwedt_sysex, m_hwbtn_copy;

	public:
		Param() {
			m_hwstc_name = m_hwtt_name = m_hwcb_value = m_hwedt_sysex = m_hwbtn_copy = NULL;
		}

		/*
		* �p�����[�^��ݒ�ł���E�B���h�E�Q���쐬���܂��B
		* @param hwnd �e�E�B���h�E�n���h���B
		* @param index �p�����[�^�C���f�b�N�X
		* @param addr �V�X�e���G�N�X�N���[�V�u�A�h���X�B
		* @param pos_y �\���ʒuY
		* @param name �p�����[�^���B
		* @param descname �����p�����[�^���B
		* @param desc �����B
		* @param ppv �p�����[�^���X�g�B
		* @param range �p�����[�^�͈́B
		* @param def_idx �f�t�H���g�l�B
		* @return bool ����:true�A���s:false�B
		*/
		bool Create(HWND hwnd, char index, char addr, short pos_y, LPCTSTR name,
		LPCTSTR descname, LPTSTR desc, const ParamValue *ppv, short range, char def_idx) {
			short i;
			TOOLINFO ti;

			m_addr = addr;
			m_def_idx = def_idx;

			// �p�����[�^���X�^�e�B�b�N�쐬�B
			if ((m_hwstc_name = CreateWindowEx(WS_EX_STATICEDGE, TEXT("STATIC"), name,
			WS_CHILD | WS_VISIBLE, 10, pos_y + 2, 100, 15, hwnd, NULL, g_hinst, NULL)) == NULL) return false;
			SendMessage(m_hwstc_name, WM_SETFONT, (WPARAM)g_hfont, FALSE);

			// �p�����[�^���c�[���`�b�v�쐬�B
			if ((m_hwtt_name = CreateWindow(TOOLTIPS_CLASS, NULL,
			TTS_ALWAYSTIP | TTS_BALLOON | TTS_NOPREFIX,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hwnd, NULL, g_hinst, NULL)) == NULL)
				return false;
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT;
			ti.hwnd = hwnd;
			ti.uId = 0;
			ti.rect.left = 10;
			ti.rect.top = pos_y + 2;
			ti.rect.right = 110;
			ti.rect.bottom = pos_y + 17;
			ti.hinst = g_hinst;
			ti.lpszText = desc;
			ti.lParam = 0;
			SendMessage(m_hwtt_name, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELPARAM(32767, 0));
			SendMessage(m_hwtt_name, TTM_SETDELAYTIME, TTDT_INITIAL, MAKELPARAM(50, 0));
			SendMessage(m_hwtt_name, TTM_SETMAXTIPWIDTH, 0, 300);
			if (!SendMessage(m_hwtt_name, TTM_SETTITLE, 1, (LPARAM)descname)) return false;
			if (!SendMessage(m_hwtt_name, TTM_ADDTOOL, 0, (LPARAM)&ti)) return false;

			// �l�ݒ�R���{�{�b�N�X�쐬�B
			if ((m_hwcb_value = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("COMBOBOX"), NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, 115, pos_y, 70, CW_USEDEFAULT,
			hwnd, (HMENU)(IDMASK_CB_VAL + index), g_hinst, NULL)) == NULL) return false;
			SendMessage(m_hwcb_value, WM_SETFONT, (WPARAM)g_hfont, FALSE);
			for (i = 0; i < range; i++) {
				m_value[i] = ppv[i].value;
				if (SendMessage(m_hwcb_value, CB_ADDSTRING, 0, (LPARAM)ppv[i].text) < 0) return false;
			}
			SendMessage(m_hwcb_value, CB_SETCURSEL, def_idx, 0);

			// �V�X�e���G�N�X�N���[�V�u�e�L�X�g�{�b�N�X�쐬�B
			if ((m_hwedt_sysex = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), NULL,
			WS_CHILD | WS_VISIBLE | ES_READONLY, 190, pos_y + 1, 205, 18,
			hwnd, (HMENU)(IDMASK_EDT_SYSEX + index), g_hinst, NULL)) == NULL) return false;
			SendMessage(m_hwedt_sysex, WM_SETFONT, (WPARAM)g_hfont, FALSE);
			setSysEx();

			// �V�X�e���G�N�X�N���[�V�u�R�s�[�{�^���쐬�B
			if ((m_hwbtn_copy = CreateWindow(TEXT("BUTTON"), TEXT("Copy"),
			WS_CHILD | WS_DISABLED | WS_VISIBLE, 400, pos_y, 50, 20,
			hwnd, (HMENU)(IDMASK_BTN_COPY + index), g_hinst, NULL)) == NULL) return false;
			SendMessage(m_hwbtn_copy, WM_SETFONT, (WPARAM)g_hfont, FALSE);

			return true;
		}

		/*
		* �p�����[�^��ݒ�ł���E�B���h�E�Q��j�����܂��B
		* @return bool ����:true�A���s:false�B
		*/
		bool Delete() {
			bool succ = true;
			if (m_hwstc_name != NULL && DestroyWindow(m_hwstc_name)) m_hwstc_name = NULL;
			else succ = false;
			if (m_hwtt_name != NULL && DestroyWindow(m_hwtt_name)) m_hwtt_name = NULL;
			else succ = false;
			if (m_hwcb_value != NULL && DestroyWindow(m_hwcb_value)) m_hwcb_value = NULL;
			else succ = false;
			if (m_hwedt_sysex != NULL && DestroyWindow(m_hwedt_sysex)) m_hwedt_sysex = NULL;
			else succ = false;
			if (m_hwbtn_copy != NULL && DestroyWindow(m_hwbtn_copy)) m_hwbtn_copy = NULL;
			else succ = false;
			return succ;
		}

		/*
		* �V�X�e���G�N�X�N���[�V�u��ݒ肵�܂��B
		*/
		void setSysEx() {
			short i;
			TCHAR buf[MAX_SYSEX];

			if (m_hwcb_value == NULL || m_hwedt_sysex == NULL) return;
			if ((i = SendMessage(m_hwcb_value, CB_GETCURSEL, 0, 0)) == CB_ERR) return;
			if (_stprintf(buf, TEXT("F0 41 10 42 12 40 03 %02X %02X %02X F7"), m_addr, m_value[i],
			128 - (0x40 + 0x03 + m_addr + m_value[i]) % 128) == -1) return;

			EnableWindow(m_hwbtn_copy, m_def_idx != i);
			SetWindowText(m_hwedt_sysex, buf);
		}

		/*
		* �V�X�e���G�N�X�N���[�V�u���N���b�v�{�[�h�ɃR�s�[���܂��B
		*/
		void copySysEx() {
			char len;
			TCHAR buf[MAX_SYSEX];
			LPTSTR pstr;
			HGLOBAL hg;

			if (!OpenClipboard(NULL)) return;
			if (EmptyClipboard() &&
			(len = GetWindowText(m_hwedt_sysex, buf, sizeof(buf) / sizeof(buf[0]))) > 0 &&
			(hg = GlobalAlloc(GMEM_MOVEABLE, (len + 1) * sizeof(TCHAR))) != NULL)
				if ((pstr = (LPTSTR)GlobalLock(hg)) != NULL) {
					CopyMemory(pstr, buf, len);
					buf[len] = TEXT('\0');
					GlobalUnlock(hg);
					SetClipboardData(CF_TEXT, hg);
				} else
					GlobalFree(hg);
			CloseClipboard();
		}
	};

	char m_param_cnt;
	HWND m_hwnd;
	Param m_param[MAX_WND];

	/*
	* �G���[��\�����Đe�E�B���h�E��j�����܂��B
	* ���̊֐��̎��s��ɏ������p�����Ȃ��ł��������B
	* @return bool ���false�B
	*/
	bool Error() {
		MessageBox(m_hwnd, TEXT("�v���I�G���[���������܂����B�I�����܂��B"), NULL, MB_ICONERROR);
		DestroyWindow(m_hwnd);
		return false;
	}

public:
	Efx(HWND hwnd) {
		m_param_cnt = 0;
		m_hwnd = hwnd;
	}

	/*
	* �p�����[�^��ݒ�ł���E�B���h�E�Q��ǉ����܂��B
	* ���̊֐��̎��s��ɏ������p�����Ȃ��ł��������B
	* @param addr �V�X�e���G�N�X�N���[�V�u�A�h���X�B
	* @param name �p�����[�^���B
	* @param descname �����p�����[�^���B
	* @param desc �����B
	* @param ppv �p�����[�^���X�g�B
	* @param range �p�����[�^�͈́B
	* @param def_idx �f�t�H���g�l�B
	* @return bool ����:true�A���s:false�B
	*/
	bool Add(char addr, LPCTSTR name, LPCTSTR descname, LPTSTR desc,
	const ParamValue *ppv, short range, char def_idx) {
		if (!m_param[m_param_cnt].Create(m_hwnd, m_param_cnt, addr, m_param_cnt * 22 + 50,
		name, descname, desc, ppv, range, def_idx)) return Error();
		m_param_cnt++;
		return true;
	}

	/*
	* �p�����[�^��ݒ�ł���E�B���h�E�Q��͈͎w��Œǉ����܂��B
	* ���̊֐��̎��s��ɏ������p�����Ȃ��ł��������B
	* @param addr �V�X�e���G�N�X�N���[�V�u�A�h���X�B
	* @param name �p�����[�^���B
	* @param descname �����p�����[�^���B
	* @param desc �����B
	* @param def_idx �f�t�H���g�l�B
	* @param val_min �l�̍ŏ��l�B
	* @param val_max �l�̍ő�l�B
	* @param text_min �\���ł̍ŏ��l�B
	* @param text_fmt �\���`���B
	* @param text_step �\���������B
	* @return bool ����:true�A���s:false�B
	*/
	bool Add(char addr, LPCTSTR name, LPCTSTR descname, LPTSTR desc, char def_idx,
	char val_min = 0, char val_max = 127, char text_min = 0, LPCTSTR text_fmt = TEXT("%d"),
	char text_step = 1) {
		short i = 0;
		bool succ;
		ParamValue pv[VALUE_RANGE];

		while (val_min + i <= val_max) {
			pv[i].value = val_min + i;
			try {
				pv[i].text = new TCHAR[MAX_VALUE];
			} catch (...) {
				return Error();
			}
			if (_stprintf(pv[i].text, text_fmt, text_min + i * text_step) == -1) return Error();
			i++;
		}

		succ = Add(addr, name, descname, desc, pv, i, def_idx);
		while (--i >= 0) delete[] pv[i].text;
		return succ;
	}

	/*
	* �p�����[�^��ݒ�ł���E�B���h�E�Q��\���e�L�X�g�ݒ�ςݔ͈͎w��Œǉ����܂��B
	* ���̊֐��̎��s��ɏ������p�����Ȃ��ł��������B
	* @param addr �V�X�e���G�N�X�N���[�V�u�A�h���X�B
	* @param name �p�����[�^���B
	* @param descname �����p�����[�^���B
	* @param desc �����B
	* @param ptext �\���e�L�X�g���X�g�B
	* @param range �p�����[�^�͈́B
	* @param def_idx �f�t�H���g�l�B
	* @param val_min �l�̍ŏ��l�B
	* @return bool ����:true�A���s:false�B
	*/
	bool Add(char addr, LPCTSTR name, LPCTSTR descname, LPTSTR desc, LPTSTR *ptext,
	short range, char def_idx, char val_min = 0) {
		short i;
		ParamValue pv[VALUE_RANGE];

		for (i = 0; i < range; i++) {
			pv[i].value = val_min + i;
			pv[i].text = ptext[i];
		}
		return Add(addr, name, descname, desc, pv, range, def_idx);
	}

	/*
	* �p�����[�^��ݒ�ł���E�B���h�E�Q��S�Ĕj�����܂��B
	* ���̊֐��̎��s��ɏ������p�����Ȃ��ł��������B
	* @return bool ����:true�A���s:false�B
	*/
	bool Delete() {
		while (m_param_cnt > 0) {
			if (!m_param[m_param_cnt - 1].Delete()) return Error();
			m_param_cnt--;
		}
		return true;
	}

	/*
	* �e�E�B���h�E�����WM_COMMAND���������܂��B
	* @param code �R���g���[���R�[�h�B
	* @param id �R���g���[��ID�B
	* @param hwnd �E�B���h�E�n���h���B
	*/
	void Command(WORD code, WORD id, HWND hwnd) {
		switch (id & MASK_CTRLID) {
		case IDMASK_CB_VAL:
			switch (code) {
			case CBN_SELCHANGE:
				m_param[id & MASK_INDEX].setSysEx();
				break;
			}
			break;
		case IDMASK_BTN_COPY:
			switch (code) {
			case BN_CLICKED:
				m_param[id & MASK_INDEX].copySysEx();
				break;
			}
			break;
		}
	}
};

LPTSTR g_efxpv1[] = {
	TEXT("0.0"), TEXT("0.1"), TEXT("0.2"), TEXT("0.3"), TEXT("0.4"), TEXT("0.5"), TEXT("0.6"), TEXT("0.7"),
	TEXT("0.8"), TEXT("0.9"), TEXT("1.0"), TEXT("1.1"), TEXT("1.2"), TEXT("1.3"), TEXT("1.4"), TEXT("1.5"),
	TEXT("1.6"), TEXT("1.7"), TEXT("1.8"), TEXT("1.9"), TEXT("2.0"), TEXT("2.1"), TEXT("2.2"), TEXT("2.3"),
	TEXT("2.4"), TEXT("2.5"), TEXT("2.6"), TEXT("2.7"), TEXT("2.8"), TEXT("2.9"), TEXT("3.0"), TEXT("3.1"),
	TEXT("3.2"), TEXT("3.3"), TEXT("3.4"), TEXT("3.5"), TEXT("3.6"), TEXT("3.7"), TEXT("3.8"), TEXT("3.9"),
	TEXT("4.0"), TEXT("4.1"), TEXT("4.2"), TEXT("4.3"), TEXT("4.4"), TEXT("4.5"), TEXT("4.6"), TEXT("4.7"),
	TEXT("4.8"), TEXT("4.9"), TEXT("5.0"), TEXT("5.5"), TEXT("6.0"), TEXT("6.5"), TEXT("7.0"), TEXT("7.5"),
	TEXT("8.0"), TEXT("8.5"), TEXT("9.0"), TEXT("9.5"), TEXT("10"), TEXT("11"), TEXT("12"), TEXT("13"),
	TEXT("14"), TEXT("15"), TEXT("16"), TEXT("17"), TEXT("18"), TEXT("19"), TEXT("20"), TEXT("21"),
	TEXT("22"), TEXT("23"), TEXT("24"), TEXT("25"), TEXT("26"), TEXT("27"), TEXT("28"), TEXT("29"),
	TEXT("30"), TEXT("31"), TEXT("32"), TEXT("33"), TEXT("34"), TEXT("35"), TEXT("36"), TEXT("37"),
	TEXT("38"), TEXT("39"), TEXT("40"), TEXT("41"), TEXT("42"), TEXT("43"), TEXT("44"), TEXT("45"),
	TEXT("46"), TEXT("47"), TEXT("48"), TEXT("49"), TEXT("50"), TEXT("52"), TEXT("54"), TEXT("56"),
	TEXT("58"), TEXT("60"), TEXT("62"), TEXT("64"), TEXT("66"), TEXT("68"), TEXT("70"), TEXT("72"),
	TEXT("74"), TEXT("76"), TEXT("78"), TEXT("80"), TEXT("82"), TEXT("84"), TEXT("86"), TEXT("88"),
	TEXT("90"), TEXT("92"), TEXT("94"), TEXT("96"), TEXT("98"), TEXT("100")
}, g_efxpv2[] = {
	TEXT("200"), TEXT("205"), TEXT("210"), TEXT("215"), TEXT("220"), TEXT("225"), TEXT("230"), TEXT("235"),
	TEXT("240"), TEXT("245"), TEXT("250"), TEXT("255"), TEXT("260"), TEXT("265"), TEXT("270"), TEXT("275"),
	TEXT("280"), TEXT("285"), TEXT("290"), TEXT("295"), TEXT("300"), TEXT("305"), TEXT("310"), TEXT("315"),
	TEXT("320"), TEXT("325"), TEXT("330"), TEXT("335"), TEXT("340"), TEXT("345"), TEXT("350"), TEXT("355"),
	TEXT("360"), TEXT("365"), TEXT("370"), TEXT("375"), TEXT("380"), TEXT("385"), TEXT("390"), TEXT("395"),
	TEXT("400"), TEXT("405"), TEXT("410"), TEXT("415"), TEXT("420"), TEXT("425"), TEXT("430"), TEXT("435"),
	TEXT("440"), TEXT("445"), TEXT("450"), TEXT("455"), TEXT("460"), TEXT("465"), TEXT("470"), TEXT("475"),
	TEXT("480"), TEXT("485"), TEXT("490"), TEXT("495"), TEXT("500"), TEXT("505"), TEXT("510"), TEXT("515"),
	TEXT("520"), TEXT("525"), TEXT("530"), TEXT("535"), TEXT("540"), TEXT("545"), TEXT("550"), TEXT("560"),
	TEXT("570"), TEXT("580"), TEXT("590"), TEXT("600"), TEXT("610"), TEXT("620"), TEXT("630"), TEXT("640"),
	TEXT("650"), TEXT("660"), TEXT("670"), TEXT("680"), TEXT("690"), TEXT("700"), TEXT("710"), TEXT("720"),
	TEXT("730"), TEXT("740"), TEXT("750"), TEXT("760"), TEXT("770"), TEXT("780"), TEXT("790"), TEXT("800"),
	TEXT("810"), TEXT("820"), TEXT("830"), TEXT("840"), TEXT("850"), TEXT("860"), TEXT("870"), TEXT("880"),
	TEXT("890"), TEXT("900"), TEXT("910"), TEXT("920"), TEXT("930"), TEXT("940"), TEXT("950"), TEXT("960"),
	TEXT("970"), TEXT("980"), TEXT("990"), TEXT("1000")
}, g_efxpv3[] = {
	TEXT("200"), TEXT("205"), TEXT("210"), TEXT("215"), TEXT("220"), TEXT("225"), TEXT("230"), TEXT("235"),
	TEXT("240"), TEXT("245"), TEXT("250"), TEXT("255"), TEXT("260"), TEXT("265"), TEXT("270"), TEXT("275"),
	TEXT("280"), TEXT("285"), TEXT("290"), TEXT("295"), TEXT("300"), TEXT("305"), TEXT("310"), TEXT("315"),
	TEXT("320"), TEXT("325"), TEXT("330"), TEXT("335"), TEXT("340"), TEXT("345"), TEXT("350"), TEXT("355"),
	TEXT("360"), TEXT("365"), TEXT("370"), TEXT("375"), TEXT("380"), TEXT("385"), TEXT("390"), TEXT("395"),
	TEXT("400"), TEXT("405"), TEXT("410"), TEXT("415"), TEXT("420"), TEXT("425"), TEXT("430"), TEXT("435"),
	TEXT("440"), TEXT("445"), TEXT("450"), TEXT("455"), TEXT("460"), TEXT("465"), TEXT("470"), TEXT("475"),
	TEXT("480"), TEXT("485"), TEXT("490"), TEXT("495"), TEXT("500"), TEXT("505"), TEXT("510"), TEXT("515"),
	TEXT("520"), TEXT("525"), TEXT("530"), TEXT("535"), TEXT("540"), TEXT("545"), TEXT("550"), TEXT("555"),
	TEXT("560"), TEXT("565"), TEXT("570"), TEXT("575"), TEXT("580"), TEXT("585"), TEXT("590"), TEXT("595"),
	TEXT("600"), TEXT("610"), TEXT("620"), TEXT("630"), TEXT("640"), TEXT("650"), TEXT("660"), TEXT("670"),
	TEXT("680"), TEXT("690"), TEXT("700"), TEXT("710"), TEXT("720"), TEXT("730"), TEXT("740"), TEXT("750"),
	TEXT("760"), TEXT("770"), TEXT("780"), TEXT("790"), TEXT("800"), TEXT("810"), TEXT("820"), TEXT("830"),
	TEXT("840"), TEXT("850"), TEXT("860"), TEXT("870"), TEXT("880"), TEXT("890"), TEXT("900"), TEXT("910"),
	TEXT("920"), TEXT("930"), TEXT("940"), TEXT("950"), TEXT("960"), TEXT("970"), TEXT("980"), TEXT("990"),
	TEXT("1000")
}, g_efxpv4[] = {
	TEXT("0.0"), TEXT("0.1"), TEXT("0.2"), TEXT("0.3"), TEXT("0.4"), TEXT("0.5"), TEXT("0.6"), TEXT("0.7"),
	TEXT("0.8"), TEXT("0.9"), TEXT("1.0"), TEXT("1.1"), TEXT("1.2"), TEXT("1.3"), TEXT("1.4"), TEXT("1.5"),
	TEXT("1.6"), TEXT("1.7"), TEXT("1.8"), TEXT("1.9"), TEXT("2.0"), TEXT("2.1"), TEXT("2.2"), TEXT("2.3"),
	TEXT("2.4"), TEXT("2.5"), TEXT("2.6"), TEXT("2.7"), TEXT("2.8"), TEXT("2.9"), TEXT("3.0"), TEXT("3.1"),
	TEXT("3.2"), TEXT("3.3"), TEXT("3.4"), TEXT("3.5"), TEXT("3.6"), TEXT("3.7"), TEXT("3.8"), TEXT("3.9"),
	TEXT("4.0"), TEXT("4.1"), TEXT("4.2"), TEXT("4.3"), TEXT("4.4"), TEXT("4.5"), TEXT("4.6"), TEXT("4.7"),
	TEXT("4.8"), TEXT("4.9"), TEXT("5.0"), TEXT("5.5"), TEXT("6.0"), TEXT("6.5"), TEXT("7.0"), TEXT("7.5"),
	TEXT("8.0"), TEXT("8.5"), TEXT("9.0"), TEXT("9.5"), TEXT("10"), TEXT("11"), TEXT("12"), TEXT("13"),
	TEXT("14"), TEXT("15"), TEXT("16"), TEXT("17"), TEXT("18"), TEXT("19"), TEXT("20"), TEXT("21"),
	TEXT("22"), TEXT("23"), TEXT("24"), TEXT("25"), TEXT("26"), TEXT("27"), TEXT("28"), TEXT("29"),
	TEXT("30"), TEXT("31"), TEXT("32"), TEXT("33"), TEXT("34"), TEXT("35"), TEXT("36"), TEXT("37"),
	TEXT("38"), TEXT("39"), TEXT("40"), TEXT("50"), TEXT("60"), TEXT("70"), TEXT("80"), TEXT("90"),
	TEXT("100"), TEXT("110"), TEXT("120"), TEXT("130"), TEXT("140"), TEXT("150"), TEXT("160"), TEXT("170"),
	TEXT("180"), TEXT("190"), TEXT("200"), TEXT("210"), TEXT("220"), TEXT("230"), TEXT("240"), TEXT("250"),
	TEXT("260"), TEXT("270"), TEXT("280"), TEXT("290"), TEXT("300"), TEXT("320"), TEXT("340"), TEXT("360"),
	TEXT("380"), TEXT("400"), TEXT("420"), TEXT("440"), TEXT("460"), TEXT("480"), TEXT("500")
}, g_efxpv5[] = {
	TEXT("0"), TEXT("5"), TEXT("10"), TEXT("15"), TEXT("20"), TEXT("25"), TEXT("30"), TEXT("35"),
	TEXT("40"), TEXT("45"), TEXT("50"), TEXT("55"), TEXT("60"), TEXT("65"), TEXT("70"), TEXT("75"),
	TEXT("80"), TEXT("85"), TEXT("90"), TEXT("95"), TEXT("100"), TEXT("105"), TEXT("110"), TEXT("115"),
	TEXT("120"), TEXT("125"), TEXT("130"), TEXT("135"), TEXT("140"), TEXT("145"), TEXT("150"), TEXT("155"),
	TEXT("160"), TEXT("165"), TEXT("170"), TEXT("175"), TEXT("180"), TEXT("185"), TEXT("190"), TEXT("195"),
	TEXT("200"), TEXT("205"), TEXT("210"), TEXT("215"), TEXT("220"), TEXT("225"), TEXT("230"), TEXT("235"),
	TEXT("240"), TEXT("245"), TEXT("250"), TEXT("255"), TEXT("260"), TEXT("265"), TEXT("270"), TEXT("275"),
	TEXT("280"), TEXT("285"), TEXT("290"), TEXT("295"), TEXT("300"), TEXT("305"), TEXT("310"), TEXT("315"),
	TEXT("320"), TEXT("325"), TEXT("330"), TEXT("335"), TEXT("340"), TEXT("345"), TEXT("350"), TEXT("355"),
	TEXT("360"), TEXT("365"), TEXT("370"), TEXT("375"), TEXT("380"), TEXT("385"), TEXT("390"), TEXT("395"),
	TEXT("400"), TEXT("405"), TEXT("410"), TEXT("415"), TEXT("420"), TEXT("425"), TEXT("430"), TEXT("435"),
	TEXT("440"), TEXT("445"), TEXT("450"), TEXT("455"), TEXT("460"), TEXT("465"), TEXT("470"), TEXT("475"),
	TEXT("480"), TEXT("485"), TEXT("490"), TEXT("495"), TEXT("500"), TEXT("505"), TEXT("510"), TEXT("515"),
	TEXT("520"), TEXT("525"), TEXT("530"), TEXT("535"), TEXT("540"), TEXT("545"), TEXT("550"), TEXT("555"),
	TEXT("560"), TEXT("565"), TEXT("570"), TEXT("575"), TEXT("580"), TEXT("585"), TEXT("590"), TEXT("595"),
	TEXT("600"), TEXT("605"), TEXT("610"), TEXT("615"), TEXT("620"), TEXT("625"), TEXT("630"), TEXT("635")
}, g_efxpv6[] = {
	TEXT("0.05"), TEXT("0.10"), TEXT("0.15"), TEXT("0.20"), TEXT("0.25"), TEXT("0.30"), TEXT("0.35"),
	TEXT("0.40"), TEXT("0.45"), TEXT("0.50"), TEXT("0.55"), TEXT("0.60"), TEXT("0.65"), TEXT("0.70"),
	TEXT("0.75"), TEXT("0.80"), TEXT("0.85"), TEXT("0.90"), TEXT("0.95"), TEXT("1.00"), TEXT("1.05"),
	TEXT("1.10"), TEXT("1.15"), TEXT("1.20"), TEXT("1.25"), TEXT("1.30"), TEXT("1.35"), TEXT("1.40"),
	TEXT("1.45"), TEXT("1.50"), TEXT("1.55"), TEXT("1.60"), TEXT("1.65"), TEXT("1.70"), TEXT("1.75"),
	TEXT("1.80"), TEXT("1.85"), TEXT("1.90"), TEXT("1.95"), TEXT("2.00"), TEXT("2.05"), TEXT("2.10"),
	TEXT("2.15"), TEXT("2.20"), TEXT("2.25"), TEXT("2.30"), TEXT("2.35"), TEXT("2.40"), TEXT("2.45"),
	TEXT("2.50"), TEXT("2.55"), TEXT("2.60"), TEXT("2.65"), TEXT("2.70"), TEXT("2.75"), TEXT("2.80"),
	TEXT("2.85"), TEXT("2.90"), TEXT("2.95"), TEXT("3.00"), TEXT("3.05"), TEXT("3.10"), TEXT("3.15"),
	TEXT("3.20"), TEXT("3.25"), TEXT("3.30"), TEXT("3.35"), TEXT("3.40"), TEXT("3.45"), TEXT("3.50"),
	TEXT("3.55"), TEXT("3.60"), TEXT("3.65"), TEXT("3.70"), TEXT("3.75"), TEXT("3.80"), TEXT("3.85"),
	TEXT("3.90"), TEXT("3.95"), TEXT("4.00"), TEXT("4.05"), TEXT("4.10"), TEXT("4.15"), TEXT("4.20"),
	TEXT("4.25"), TEXT("4.30"), TEXT("4.35"), TEXT("4.40"), TEXT("4.45"), TEXT("4.50"), TEXT("4.55"),
	TEXT("4.60"), TEXT("4.65"), TEXT("4.70"), TEXT("4.75"), TEXT("4.80"), TEXT("4.85"), TEXT("4.90"),
	TEXT("4.95"), TEXT("5.00"), TEXT("5.10"), TEXT("5.20"), TEXT("5.30"), TEXT("5.40"), TEXT("5.50"),
	TEXT("5.60"), TEXT("5.70"), TEXT("5.80"), TEXT("5.90"), TEXT("6.00"), TEXT("6.10"), TEXT("6.20"),
	TEXT("6.30"), TEXT("6.40"), TEXT("6.50"), TEXT("6.60"), TEXT("6.70"), TEXT("6.80"), TEXT("6.90"),
	TEXT("7.00"), TEXT("7.50"), TEXT("8.00"), TEXT("8.50"), TEXT("9.00"), TEXT("9.50"), TEXT("10.00")
}, g_efxpv7[] = {
	TEXT("0.05"), TEXT("0.10"), TEXT("0.15"), TEXT("0.20"), TEXT("0.25"), TEXT("0.30"), TEXT("0.35"),
	TEXT("0.40"), TEXT("0.45"), TEXT("0.50"), TEXT("0.55"), TEXT("0.60"), TEXT("0.65"), TEXT("0.70"),
	TEXT("0.75"), TEXT("0.80"), TEXT("0.85"), TEXT("0.90"), TEXT("0.95"), TEXT("1.00"), TEXT("1.05"),
	TEXT("1.10"), TEXT("1.15"), TEXT("1.20"), TEXT("1.25"), TEXT("1.30"), TEXT("1.35"), TEXT("1.40"),
	TEXT("1.45"), TEXT("1.50"), TEXT("1.55"), TEXT("1.60"), TEXT("1.65"), TEXT("1.70"), TEXT("1.75"),
	TEXT("1.80"), TEXT("1.85"), TEXT("1.90"), TEXT("1.95"), TEXT("2.00"), TEXT("2.05"), TEXT("2.10"),
	TEXT("2.15"), TEXT("2.20"), TEXT("2.25"), TEXT("2.30"), TEXT("2.35"), TEXT("2.40"), TEXT("2.45"),
	TEXT("2.50"), TEXT("2.55"), TEXT("2.60"), TEXT("2.65"), TEXT("2.70"), TEXT("2.75"), TEXT("2.80"),
	TEXT("2.85"), TEXT("2.90"), TEXT("2.95"), TEXT("3.00"), TEXT("3.05"), TEXT("3.10"), TEXT("3.15"),
	TEXT("3.20"), TEXT("3.25"), TEXT("3.30"), TEXT("3.35"), TEXT("3.40"), TEXT("3.45"), TEXT("3.50"),
	TEXT("3.55"), TEXT("3.60"), TEXT("3.65"), TEXT("3.70"), TEXT("3.75"), TEXT("3.80"), TEXT("3.85"),
	TEXT("3.90"), TEXT("3.95"), TEXT("4.00"), TEXT("4.05"), TEXT("4.10"), TEXT("4.15"), TEXT("4.20"),
	TEXT("4.25"), TEXT("4.30"), TEXT("4.35"), TEXT("4.40"), TEXT("4.45"), TEXT("4.50"), TEXT("4.55"),
	TEXT("4.60"), TEXT("4.65"), TEXT("4.70"), TEXT("4.75"), TEXT("4.80"), TEXT("4.85"), TEXT("4.90"),
	TEXT("4.95"), TEXT("5.00"), TEXT("5.05"), TEXT("5.10"), TEXT("5.15"), TEXT("5.20"), TEXT("5.25"),
	TEXT("5.30"), TEXT("5.35"), TEXT("5.40"), TEXT("5.45"), TEXT("5.50"), TEXT("5.55"), TEXT("5.60"),
	TEXT("5.65"), TEXT("5.70"), TEXT("5.75"), TEXT("5.80"), TEXT("5.85"), TEXT("5.90"), TEXT("5.95"),
	TEXT("6.00"), TEXT("6.05"), TEXT("6.10"), TEXT("6.15"), TEXT("6.20"), TEXT("6.25"), TEXT("6.30"),
	TEXT("6.35"), TEXT("6.40")
}, g_efxpv12[] = {
	TEXT("100"), TEXT("110"), TEXT("120"), TEXT("130"), TEXT("140"), TEXT("150"), TEXT("160"),
	TEXT("170"), TEXT("180"), TEXT("190"), TEXT("200"), TEXT("210"), TEXT("220"), TEXT("230"),
	TEXT("240"), TEXT("250"), TEXT("260"), TEXT("270"), TEXT("280"), TEXT("290"), TEXT("300"),
	TEXT("320"), TEXT("340"), TEXT("360"), TEXT("380"), TEXT("400"), TEXT("420"), TEXT("440"),
	TEXT("460"), TEXT("480"), TEXT("500"), TEXT("520"), TEXT("540"), TEXT("560"), TEXT("580"),
	TEXT("600"), TEXT("620"), TEXT("640"), TEXT("660"), TEXT("680"), TEXT("700"), TEXT("720"),
	TEXT("740"), TEXT("760"), TEXT("780"), TEXT("800"), TEXT("820"), TEXT("840"), TEXT("860"),
	TEXT("880"), TEXT("900"), TEXT("920"), TEXT("940"), TEXT("960"), TEXT("980"), TEXT("1000"),
	TEXT("1100"), TEXT("1200"), TEXT("1300"), TEXT("1400"), TEXT("1500"), TEXT("1600"), TEXT("1700"),
	TEXT("1800"), TEXT("1900"), TEXT("2000"), TEXT("2100"), TEXT("2200"), TEXT("2300"), TEXT("2400"),
	TEXT("2500"), TEXT("2600"), TEXT("2700"), TEXT("2800"), TEXT("2900"), TEXT("3000"), TEXT("3100"),
	TEXT("3200"), TEXT("3300"), TEXT("3400"), TEXT("3500"), TEXT("3600"), TEXT("3700"), TEXT("3800"),
	TEXT("3900"), TEXT("4000"), TEXT("4100"), TEXT("4200"), TEXT("4300"), TEXT("4400"), TEXT("4500"),
	TEXT("4600"), TEXT("4700"), TEXT("4800"), TEXT("4900"), TEXT("5000"), TEXT("5100"), TEXT("5200"),
	TEXT("5300"), TEXT("5400"), TEXT("5500"), TEXT("5600"), TEXT("5700"), TEXT("5800"), TEXT("5900"),
	TEXT("6000"), TEXT("6100"), TEXT("6200"), TEXT("6300"), TEXT("6400"), TEXT("6500"), TEXT("6600"),
	TEXT("6700"), TEXT("6800"), TEXT("6900"), TEXT("7000"), TEXT("7100"), TEXT("7200"), TEXT("7300"),
	TEXT("7400"), TEXT("7500"), TEXT("7600"), TEXT("7700"), TEXT("7800"), TEXT("7900"), TEXT("8000")
}, g_efxpv_o1[] = {
	TEXT("L63"), TEXT("L62"), TEXT("L61"), TEXT("L60"), TEXT("L59"), TEXT("L58"), TEXT("L57"), TEXT("L56"),
	TEXT("L55"), TEXT("L54"), TEXT("L53"), TEXT("L52"), TEXT("L51"), TEXT("L50"), TEXT("L49"), TEXT("L48"),
	TEXT("L47"), TEXT("L46"), TEXT("L45"), TEXT("L44"), TEXT("L43"), TEXT("L42"), TEXT("L41"), TEXT("L40"),
	TEXT("L39"), TEXT("L38"), TEXT("L37"), TEXT("L36"), TEXT("L35"), TEXT("L34"), TEXT("L33"), TEXT("L32"),
	TEXT("L31"), TEXT("L30"), TEXT("L29"), TEXT("L28"), TEXT("L27"), TEXT("L26"), TEXT("L25"), TEXT("L24"),
	TEXT("L23"), TEXT("L22"), TEXT("L21"), TEXT("L20"), TEXT("L19"), TEXT("L18"), TEXT("L17"), TEXT("L16"),
	TEXT("L15"), TEXT("L14"), TEXT("L13"), TEXT("L12"), TEXT("L11"), TEXT("L10"), TEXT("L9"), TEXT("L8"),
	TEXT("L7"), TEXT("L6"), TEXT("L5"), TEXT("L4"), TEXT("L3"), TEXT("L2"), TEXT("L1"), TEXT("0"),
	TEXT("0"), TEXT("R1"), TEXT("R2"), TEXT("R3"), TEXT("R4"), TEXT("R5"), TEXT("R6"), TEXT("R7"),
	TEXT("R8"), TEXT("R9"), TEXT("R10"), TEXT("R11"), TEXT("R12"), TEXT("R13"), TEXT("R14"), TEXT("R15"),
	TEXT("R16"), TEXT("R17"), TEXT("R18"), TEXT("R19"), TEXT("R20"), TEXT("R21"), TEXT("R22"), TEXT("R23"),
	TEXT("R24"), TEXT("R25"), TEXT("R26"), TEXT("R27"), TEXT("R28"), TEXT("R29"), TEXT("R30"), TEXT("R31"),
	TEXT("R32"), TEXT("R33"), TEXT("R34"), TEXT("R35"), TEXT("R36"), TEXT("R37"), TEXT("R38"), TEXT("R39"),
	TEXT("R40"), TEXT("R41"), TEXT("R42"), TEXT("R43"), TEXT("R44"), TEXT("R45"), TEXT("R46"), TEXT("R47"),
	TEXT("R48"), TEXT("R49"), TEXT("R50"), TEXT("R51"), TEXT("R52"), TEXT("R53"), TEXT("R54"), TEXT("R55"),
	TEXT("R56"), TEXT("R57"), TEXT("R58"), TEXT("R59"), TEXT("R60"), TEXT("R61"), TEXT("R62"), TEXT("R63")
}, g_efxpv_o2[] = {
	TEXT("D>0E"), TEXT("D>1E"), TEXT("D>2E"), TEXT("D>3E"), TEXT("D>4E"), TEXT("D>5E"), TEXT("D>6E"),
	TEXT("D>7E"), TEXT("D>8E"), TEXT("D>9E"), TEXT("D>10E"), TEXT("D>11E"), TEXT("D>12E"), TEXT("D>13E"),
	TEXT("D>14E"), TEXT("D>15E"), TEXT("D>16E"), TEXT("D>17E"), TEXT("D>18E"), TEXT("D>19E"), TEXT("D>20E"),
	TEXT("D>21E"), TEXT("D>22E"), TEXT("D>23E"), TEXT("D>24E"), TEXT("D>25E"), TEXT("D>26E"), TEXT("D>27E"),
	TEXT("D>28E"), TEXT("D>29E"), TEXT("D>30E"), TEXT("D>31E"), TEXT("D>32E"), TEXT("D>33E"), TEXT("D>34E"),
	TEXT("D>35E"), TEXT("D>36E"), TEXT("D>37E"), TEXT("D>38E"), TEXT("D>39E"), TEXT("D>40E"), TEXT("D>41E"),
	TEXT("D>42E"), TEXT("D>43E"), TEXT("D>44E"), TEXT("D>45E"), TEXT("D>46E"), TEXT("D>47E"), TEXT("D>48E"),
	TEXT("D>49E"), TEXT("D>50E"), TEXT("D>51E"), TEXT("D>52E"), TEXT("D>53E"), TEXT("D>54E"), TEXT("D>55E"),
	TEXT("D>56E"), TEXT("D>57E"), TEXT("D>58E"), TEXT("D>59E"), TEXT("D>60E"), TEXT("D>61E"), TEXT("D>62E"),
	TEXT("D>63E"), TEXT("D=E"), TEXT("D62<E"), TEXT("D61<E"), TEXT("D60<E"), TEXT("D59<E"), TEXT("D58<E"),
	TEXT("D57<E"), TEXT("D56<E"), TEXT("D55<E"), TEXT("D54<E"), TEXT("D53<E"), TEXT("D52<E"), TEXT("D51<E"),
	TEXT("D50<E"), TEXT("D49<E"), TEXT("D48<E"), TEXT("D47<E"), TEXT("D46<E"), TEXT("D45<E"), TEXT("D44<E"),
	TEXT("D43<E"), TEXT("D42<E"), TEXT("D41<E"), TEXT("D40<E"), TEXT("D39<E"), TEXT("D38<E"), TEXT("D37<E"),
	TEXT("D36<E"), TEXT("D35<E"), TEXT("D34<E"), TEXT("D33<E"), TEXT("D32<E"), TEXT("D31<E"), TEXT("D30<E"),
	TEXT("D29<E"), TEXT("D28<E"), TEXT("D27<E"), TEXT("D26<E"), TEXT("D25<E"), TEXT("D24<E"), TEXT("D23<E"),
	TEXT("D22<E"), TEXT("D21<E"), TEXT("D20<E"), TEXT("D19<E"), TEXT("D18<E"), TEXT("D17<E"), TEXT("D16<E"),
	TEXT("D15<E"), TEXT("D14<E"), TEXT("D13<E"), TEXT("D12<E"), TEXT("D11<E"), TEXT("D10<E"), TEXT("D9<E"),
	TEXT("D8<E"), TEXT("D7<E"), TEXT("D6<E"), TEXT("D5<E"), TEXT("D4<E"), TEXT("D3<E"), TEXT("D2<E"),
	TEXT("D1<E"), TEXT("D0<E")
}, g_efxpv_o3[] = {
	TEXT("A>0B"), TEXT("A>1B"), TEXT("A>2B"), TEXT("A>3B"), TEXT("A>4B"), TEXT("A>5B"), TEXT("A>6B"),
	TEXT("A>7B"), TEXT("A>8B"), TEXT("A>9B"), TEXT("A>10B"), TEXT("A>11B"), TEXT("A>12B"), TEXT("A>13B"),
	TEXT("A>14B"), TEXT("A>15B"), TEXT("A>16B"), TEXT("A>17B"), TEXT("A>18B"), TEXT("A>19B"), TEXT("A>20B"),
	TEXT("A>21B"), TEXT("A>22B"), TEXT("A>23B"), TEXT("A>24B"), TEXT("A>25B"), TEXT("A>26B"), TEXT("A>27B"),
	TEXT("A>28B"), TEXT("A>29B"), TEXT("A>30B"), TEXT("A>31B"), TEXT("A>32B"), TEXT("A>33B"), TEXT("A>34B"),
	TEXT("A>35B"), TEXT("A>36B"), TEXT("A>37B"), TEXT("A>38B"), TEXT("A>39B"), TEXT("A>40B"), TEXT("A>41B"),
	TEXT("A>42B"), TEXT("A>43B"), TEXT("A>44B"), TEXT("A>45B"), TEXT("A>46B"), TEXT("A>47B"), TEXT("A>48B"),
	TEXT("A>49B"), TEXT("A>50B"), TEXT("A>51B"), TEXT("A>52B"), TEXT("A>53B"), TEXT("A>54B"), TEXT("A>55B"),
	TEXT("A>56B"), TEXT("A>57B"), TEXT("A>58B"), TEXT("A>59B"), TEXT("A>60B"), TEXT("A>61B"), TEXT("A>62B"),
	TEXT("A>63B"), TEXT("A=B"), TEXT("A62<B"), TEXT("A61<B"), TEXT("A60<B"), TEXT("A59<B"), TEXT("A58<B"),
	TEXT("A57<B"), TEXT("A56<B"), TEXT("A55<B"), TEXT("A54<B"), TEXT("A53<B"), TEXT("A52<B"), TEXT("A51<B"),
	TEXT("A50<B"), TEXT("A49<B"), TEXT("A48<B"), TEXT("A47<B"), TEXT("A46<B"), TEXT("A45<B"), TEXT("A44<B"),
	TEXT("A43<B"), TEXT("A42<B"), TEXT("A41<B"), TEXT("A40<B"), TEXT("A39<B"), TEXT("A38<B"), TEXT("A37<B"),
	TEXT("A36<B"), TEXT("A35<B"), TEXT("A34<B"), TEXT("A33<B"), TEXT("A32<B"), TEXT("A31<B"), TEXT("A30<B"),
	TEXT("A29<B"), TEXT("A28<B"), TEXT("A27<B"), TEXT("A26<B"), TEXT("A25<B"), TEXT("A24<B"), TEXT("A23<B"),
	TEXT("A22<B"), TEXT("A21<B"), TEXT("A20<B"), TEXT("A19<B"), TEXT("A18<B"), TEXT("A17<B"), TEXT("A16<B"),
	TEXT("A15<B"), TEXT("A14<B"), TEXT("A13<B"), TEXT("A12<B"), TEXT("A11<B"), TEXT("A10<B"), TEXT("A9<B"),
	TEXT("A8<B"), TEXT("A7<B"), TEXT("A6<B"), TEXT("A5<B"), TEXT("A4<B"), TEXT("A3<B"), TEXT("A2<B"),
	TEXT("A1<B"), TEXT("A0<B")
};

const Efx::ParamValue g_efxpv8[] = {
	{0x00, TEXT("315")}, {0x08, TEXT("400")}, {0x10, TEXT("500")}, {0x18, TEXT("630")},
	{0x20, TEXT("800")}, {0x28, TEXT("1000")}, {0x30, TEXT("1250")}, {0x38, TEXT("1600")},
	{0x40, TEXT("2000")}, {0x48, TEXT("2500")}, {0x50, TEXT("3150")}, {0x58, TEXT("4000")},
	{0x60, TEXT("5000")}, {0x68, TEXT("6300")}, {0x70, TEXT("8000")}, {0x78, TEXT("Bypass")}
}, g_efxpv9[] = {
	{0x00, TEXT("250")}, {0x08, TEXT("315")}, {0x10, TEXT("400")}, {0x18, TEXT("500")},
	{0x20, TEXT("630")}, {0x28, TEXT("800")}, {0x30, TEXT("1000")}, {0x38, TEXT("1250")},
	{0x40, TEXT("1600")}, {0x48, TEXT("2000")}, {0x50, TEXT("2500")}, {0x58, TEXT("3150")},
	{0x60, TEXT("4000")}, {0x68, TEXT("5000")}, {0x70, TEXT("6300")}, {0x78, TEXT("8000")}
}, g_efxpv10[] = {
	{0x00, TEXT("200")}, {0x08, TEXT("250")}, {0x10, TEXT("315")}, {0x18, TEXT("400")},
	{0x20, TEXT("500")}, {0x28, TEXT("630")}, {0x30, TEXT("800")}, {0x38, TEXT("1000")},
	{0x40, TEXT("1250")}, {0x48, TEXT("1600")}, {0x50, TEXT("2000")}, {0x58, TEXT("2500")},
	{0x60, TEXT("3150")}, {0x68, TEXT("4000")}, {0x70, TEXT("5000")}, {0x78, TEXT("6300")}
}, g_efxpv11[] = {
	{0x00, TEXT("250")}, {0x08, TEXT("315")}, {0x10, TEXT("400")}, {0x18, TEXT("500")},
	{0x20, TEXT("630")}, {0x28, TEXT("800")}, {0x30, TEXT("1000")}, {0x38, TEXT("1250")},
	{0x40, TEXT("1600")}, {0x48, TEXT("2000")}, {0x50, TEXT("2500")}, {0x58, TEXT("3150")},
	{0x60, TEXT("4000")}, {0x68, TEXT("5000")}, {0x70, TEXT("6300")}, {0x78, TEXT("Bypass")}
}, g_efxpv13[] = {
	{0x00, TEXT("L180(=R180)")}, {0x06, TEXT("L168")}, {0x0a, TEXT("L156")}, {0x0e, TEXT("L144")},
	{0x12, TEXT("L132")}, {0x16, TEXT("L120")}, {0x1a, TEXT("L108")}, {0x1e, TEXT("L96")},
	{0x22, TEXT("L84")}, {0x26, TEXT("L72")}, {0x2a, TEXT("L60")}, {0x2e, TEXT("L48")},
	{0x32, TEXT("L36")}, {0x36, TEXT("L24")}, {0x3a, TEXT("L12")}, {0x3e, TEXT("0")},
	{0x42, TEXT("R12")}, {0x46, TEXT("R24")}, {0x4a, TEXT("R36")}, {0x4e, TEXT("R48")},
	{0x52, TEXT("R60")}, {0x56, TEXT("R72")}, {0x5a, TEXT("R84")}, {0x5e, TEXT("R96")},
	{0x62, TEXT("R108")}, {0x66, TEXT("R120")}, {0x6a, TEXT("R132")}, {0x6e, TEXT("R144")},
	{0x72, TEXT("R156")}, {0x76, TEXT("R168")}, {0x7a, TEXT("R180(=L180)")}
}, g_efxpv14[] = {
	{0x00, TEXT("0")}, {0x08, TEXT("1")}, {0x10, TEXT("2")}, {0x18, TEXT("3")},
	{0x20, TEXT("4")}, {0x28, TEXT("5")}, {0x30, TEXT("6")}, {0x38, TEXT("7")},
	{0x40, TEXT("8")}, {0x48, TEXT("9")}, {0x50, TEXT("10")}, {0x58, TEXT("11")},
	{0x60, TEXT("12")}, {0x68, TEXT("13")}, {0x70, TEXT("14")}, {0x78, TEXT("15")}
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char i;
	LPNMTTDISPINFO lpnmtdi;
	TOOLINFO ti;
	static HWND hw_cb_efxname, hw_stc_efxname, hw_tt_efxname;
	static Efx *pefx;

	switch (uMsg) {
	case WM_CREATE:
		// EfxName�R���{�{�b�N�X�쐬�B
		if ((hw_cb_efxname = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("COMBOBOX"), NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, 10, 5, 550, CW_USEDEFAULT,
		hwnd, (HMENU)WNDID_EFXNAME, g_hinst, NULL)) == NULL) return -1;
		SendMessage(hw_cb_efxname, WM_SETFONT, (WPARAM)g_hfont, FALSE);
		for (i = 0; i < sizeof(g_efxname) / sizeof(g_efxname[0]); i++)
			if (SendMessage(hw_cb_efxname, CB_ADDSTRING, 0, (LPARAM)g_efxname[i].name) < 0) return -1;

		// EfxName�X�^�e�B�b�N�쐬�B
		if ((hw_stc_efxname = CreateWindowEx(WS_EX_STATICEDGE, TEXT("STATIC"), TEXT("�����ɐ������\������܂��B"),
		WS_CHILD | WS_VISIBLE, 10, 25, 550, 15, hwnd, NULL, g_hinst, NULL)) == NULL) return -1;
		SendMessage(hw_stc_efxname, WM_SETFONT, (WPARAM)g_hfont, FALSE);

		// EfxName�c�[���`�b�v�쐬�B
		if ((hw_tt_efxname = CreateWindow(TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP | TTS_BALLOON | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, g_hinst, NULL)) == NULL)
			return -1;
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT;
		ti.hwnd = hwnd;
		ti.uId = 0;
		ti.rect.left = 10;
		ti.rect.top = 25;
		ti.rect.right = 560;
		ti.rect.bottom = 40;
		ti.hinst = g_hinst;
		ti.lpszText = NULL;
		ti.lParam = 0;
		SendMessage(hw_tt_efxname, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELPARAM(32767, 0));
		SendMessage(hw_tt_efxname, TTM_SETDELAYTIME, TTDT_INITIAL, MAKELPARAM(50, 0));
		SendMessage(hw_tt_efxname, TTM_SETMAXTIPWIDTH, 0, 300);
		if (!SendMessage(hw_tt_efxname, TTM_ADDTOOL, 0, (LPARAM)&ti)) return -1;

		try {
			pefx = new Efx(hwnd);
		} catch (...) {
			return -1;
		}
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case WNDID_EFXNAME:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				if ((i = SendMessage(hw_cb_efxname, CB_GETCURSEL, 0, 0)) == CB_ERR) return 0;

				ti.cbSize = sizeof(TOOLINFO);
				ti.hwnd = hwnd;
				ti.uId = 0;
				ti.hinst = g_hinst;
				ti.lpszText = g_efxname[i].desc;
				SendMessage(hw_tt_efxname, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);

				if (!pefx->Delete()) return 0;

static LPTSTR pv1[] = {TEXT("200"), TEXT("400")},
pv2[] = {TEXT("4k"), TEXT("8k")},
pv3[] = {TEXT("0.5"), TEXT("1.0"), TEXT("2.0"), TEXT("4.0"), TEXT("9.0")},
pv4[] = {TEXT("Off"), TEXT("On")},
pv5[] = {TEXT("a"), TEXT("i"), TEXT("u"), TEXT("e"), TEXT("o")},
pv6[] = {TEXT("Small"), TEXT("BltIn"), TEXT("2-Stk"), TEXT("3-Stk")},
pv7[] = {TEXT("LPF"), TEXT("BPF")},
pv8[] = {TEXT("Down"), TEXT("Up")},
pv10[] = {TEXT("Off"), TEXT("LPF"), TEXT("HPF")},
pv11[] = {TEXT("Tri"), TEXT("Sqr"), TEXT("Sin"), TEXT("Saw1"), TEXT("Saw2")},
pv12[] = {TEXT("0"), TEXT("+6"), TEXT("+12"), TEXT("+18")},
pv13[] = {TEXT("1/1.5"), TEXT("1/2"), TEXT("1/4"), TEXT("1/100")},
pv14[] = {TEXT("Speaker"), TEXT("Phones")},
pv15[] = {TEXT("Norm"), TEXT("Cross")},
pv16[] = {TEXT("Norm"), TEXT("Invert")},
pv17[] = {TEXT("Room1"), TEXT("Room2"), TEXT("Stage1"), TEXT("Stage2"), TEXT("Hall1"), TEXT("Hall2")},
pv18[] = {TEXT("Norm"), TEXT("Reverse"), TEXT("Sweep1"), TEXT("Sweep2")},
pv19[] = {TEXT("-"), TEXT("+")},
pv20[] = {TEXT("White"), TEXT("Pink")},
pv21[] = {TEXT("LP"), TEXT("EP"), TEXT("SP"), TEXT("RND")},
pv22[] = {TEXT("Mono"), TEXT("Stereo")};
static const Efx::ParamValue pv9[] = {{0x00, TEXT("Slow")}, {0x7f, TEXT("Fast")}};
switch (i) {
case 3:
	if (!pefx->Add(0x03, TEXT("Low Freq"), TEXT("���[�E�t���P���V�["), TEXT("���̊���g��(200Hz/400Hz)��I�т܂��B"), pv1, sizeof(pv1) / sizeof(pv1[0]), 1)) return 0;
	if (!pefx->Add(0x04, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("Low Freq�p�����[�^�[�̃Q�C���𒲐߂��܂��B"), 17, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x05, TEXT("Hi Freq"), TEXT("�n�C�E�t���P���V�["), TEXT("����̊���g��(4kHz/8kHz)��I�т܂��B"), pv2, sizeof(pv2) / sizeof(pv2[0]), 1)) return 0;
	if (!pefx->Add(0x06, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("High Freq�p�����[�^�[�̃Q�C���𒲐߂��܂��B"), 0, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x07, TEXT("M1 Freq"), TEXT("�~�b�h1�E�t���P���V�["), TEXT("�~�b�h1(����1)�̊���g����I�т܂��B"), g_efxpv10, sizeof(g_efxpv10) / sizeof(g_efxpv10[0]), 9)) return 0;
	if (!pefx->Add(0x08, TEXT("M1 Q"), TEXT("�~�b�h1�EQ"), TEXT("M1 Freq�p�����[�^�[�𒆐S�ɂ����Q�C���̕ω��͈͂𒲐߂��܂��B�l��傫������قǕω��͈͂������Ȃ肷�B"), pv3, sizeof(pv3) / sizeof(pv3[0]), 0)) return 0;
	if (!pefx->Add(0x09, TEXT("M1 Gain"), TEXT("�~�b�h1�E�Q�C��"), TEXT("M1 Freq�p�����[�^�[��M1 Q�p�����[�^�[�Őݒ肵���͈͂̃Q�C���𒲐߂��܂��B"), 20, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x0a, TEXT("M2 Freq"), TEXT("�~�b�h2�E�t���P���V�["), TEXT("�~�b�h2(����2)�̊���g����I�т܂��B"), g_efxpv10, sizeof(g_efxpv10) / sizeof(g_efxpv10[0]), 7)) return 0;
	if (!pefx->Add(0x0b, TEXT("M2 Q"), TEXT("�~�b�h2�EQ"), TEXT("M2 Freq�p�����[�^�[�𒆐S�ɂ����Q�C���̕ω��͈͂𒲐߂��܂��B�l��傫������قǕω��͈͂������Ȃ�܂��B"), pv3, sizeof(pv3) / sizeof(pv3[0]), 0)) return 0;
	if (!pefx->Add(0x0c, TEXT("M2 Gain"), TEXT("�~�b�h2�E�Q�C��"), TEXT("M2 Freq�p�����[�^�[��M2 Q�p�����[�^�[�Őݒ肵���͈͂̃Q�C���𒲐߂��܂��B"), 4, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("+ Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 4:
	if (!pefx->Add(0x03, TEXT("Band 1"), TEXT("�o���h1�E�Q�C��"), TEXT("250Hz�̃Q�C���𒲐߂��܂��B"), 8, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x04, TEXT("Band 2"), TEXT("�o���h2�E�Q�C��"), TEXT("500Hz�̃Q�C���𒲐߂��܂��B"), 13, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x05, TEXT("Band 3"), TEXT("�o���h3�E�Q�C��"), TEXT("1000Hz�̃Q�C���𒲐߂��܂��B"), 15, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x06, TEXT("Band 4"), TEXT("�o���h4�E�Q�C��"), TEXT("1250Hz�̃Q�C���𒲐߂��܂��B"), 18, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x07, TEXT("Band 5"), TEXT("�o���h5�E�Q�C��"), TEXT("2000Hz�̃Q�C���𒲐߂��܂��B"), 14, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x08, TEXT("Band 6"), TEXT("�o���h6�E�Q�C��"), TEXT("3150Hz�̃Q�C���𒲐߂��܂��B"), 11, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x09, TEXT("Band 7"), TEXT("�o���h7�E�Q�C��"), TEXT("4000Hz�̃Q�C���𒲐߂��܂��B"), 8, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x0a, TEXT("Band 8"), TEXT("�o���h8�E�Q�C��"), TEXT("8000Hz�̃Q�C���𒲐߂��܂��B"), 7, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x0b, TEXT("Width"), TEXT("�o���h�E�E�B�Y"), TEXT("�Q�C����ω�������ш敝���e�o���h���ʂŒ��߂��܂��B�l��傫������قǑш敝�������Ȃ�܂��B"), pv3, sizeof(pv3) / sizeof(pv3[0]), 2)) return 0;
	if (!pefx->Add(0x15, TEXT("+ Pan"), TEXT("�A�E�g�v�b�g�E�p��"), TEXT("�o�͂���鉹�̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 5:
	if (!pefx->Add(0x03, TEXT("+ Sens"), TEXT("�Z���X"), TEXT("�G���n���T�[�̂������𒲐߂��܂��B"), 64)) return 0;
	if (!pefx->Add(0x04, TEXT("# Mix"), TEXT("�~�b�N�X�E���x��"), TEXT("�G���n���T�[�Ő������ꂽ�{���������ɍ������킹�銄���ɂ��Ē��߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 15, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 6:
	if (!pefx->Add(0x03, TEXT("Drive"), TEXT("�h���C�u"), TEXT("�c�݋�𒲐߂��܂��B"), 48)) return 0;
	if (!pefx->Add(0x04, TEXT("Drive Sw"), TEXT("�h���C�u�E�X�C�b�`"), TEXT("Drive���I��/�I�t���܂��B"), pv4, sizeof(pv4) / sizeof(pv4[0]), 1)) return 0;
	if (!pefx->Add(0x05, TEXT("+ Vowel"), TEXT("���H�E�G��"), TEXT("�ꉹ��I�т܂��B"), pv5, sizeof(pv5) / sizeof(pv5[0]), 0)) return 0;
	if (!pefx->Add(0x06, TEXT("Accel"), TEXT("�A�N�Z�����[�V����"), TEXT("Vowel�Őݒ肵�����ֈڂ�ς�鎞�Ԃ𒲐߂��܂��B�l������������قǎ��Ԃ�������܂��B"), g_efxpv14, sizeof(g_efxpv14) / sizeof(g_efxpv14[0]), 15)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("Pan"), TEXT("�A�E�g�v�b�g�E�p��"), TEXT("�o�͂���鉹�̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 7:
	if (!pefx->Add(0x03, TEXT("+ Drive"), TEXT("�h���C�u"), TEXT("�c�݋�𒲐߂��܂��B"), 48)) return 0;
	if (!pefx->Add(0x04, TEXT("Amp Type"), TEXT("�A���v�E�V�~�����[�^�[�E�^�C�v"), TEXT("�M�^�[�E�A���v�̎�ނ�I�т܂��B\n Small �F���^�A���v\n BltIn �F�r���g�E�C���E�^�C�v�̃A���v\n 2-Stk �F��^2�i�ς݃A���v\n 3-Stk �F��^3�i�ς݃A���v"), pv6, sizeof(pv6) / sizeof(pv6[0]), 1)) return 0;
	if (!pefx->Add(0x05, TEXT("Amp Sw"), TEXT("�A���v�E�X�C�b�`"), TEXT("Amp Type���I��/�I�t���܂��B"), pv4, sizeof(pv4) / sizeof(pv4[0]), 1)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("Pan"), TEXT("�A�E�g�v�b�g�E�p��"), TEXT("�o�͂���鉹�̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 96)) return 0;
	break;
case 8:
	if (!pefx->Add(0x03, TEXT("+ Drive"), TEXT("�h���C�u"), TEXT("�c�݋�𒲐߂��܂��B"), 76)) return 0;
	if (!pefx->Add(0x04, TEXT("Amp Type"), TEXT("�A���v�E�V�~�����[�^�[�E�^�C�v"), TEXT("�M�^�[�E�A���v�̎�ނ�I�т܂��B\n Small �F���^�A���v\n BltIn �F�r���g�E�C���E�^�C�v�̃A���v\n 2-Stk �F��^2�i�ς݃A���v\n 3-Stk �F��^3�i�ς݃A���v"), pv6, sizeof(pv6) / sizeof(pv6[0]), 3)) return 0;
	if (!pefx->Add(0x05, TEXT("Amp Sw"), TEXT("�A���v�E�X�C�b�`"), TEXT("Amp Type���I��/�I�t���܂��B"), pv4, sizeof(pv4) / sizeof(pv4[0]), 1)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 4, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("Pan"), TEXT("�A�E�g�v�b�g�E�p��"), TEXT("�o�͂���鉹�̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 84)) return 0;
	break;
case 9:
	if (!pefx->Add(0x03, TEXT("+ Manual"), TEXT("�}�j���A��"), TEXT("�������˂点���̎��g���𒲐߂��܂��B"), g_efxpv12, sizeof(g_efxpv12) / sizeof(g_efxpv12[0]), 36)) return 0;
	if (!pefx->Add(0x04, TEXT("# Rate"), TEXT("���C�g"), TEXT("���˂�̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 16)) return 0;
	if (!pefx->Add(0x05, TEXT("Depth"), TEXT("�f�v�X"), TEXT("���˂�̐[���𒲐߂��܂��B"), 64)) return 0;
	if (!pefx->Add(0x06, TEXT("Reso"), TEXT("���]�i���X"), TEXT("Manual�p�����[�^�[�Őݒ肵������g���t�߂̎��g���������������܂��B"), 16)) return 0;
	if (!pefx->Add(0x07, TEXT("Mix"), TEXT("�~�b�N�X�E���x��"), TEXT("�ʑ������点�����������ɍ������킹�銄���𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 104)) return 0;
	break;
case 10:
	if (!pefx->Add(0x03, TEXT("Fil Type"), TEXT("�t�B���^�[�E�^�C�v"), TEXT("�t�B���^�[�̎�ނ�I�т܂��B\n LPF�F�L�����g���͈͂Ń��E���ʂ������܂��B\n BPF�F�������g���͈͂Ń��E���ʂ������܂��B"), pv7, sizeof(pv7) / sizeof(pv7[0]), 1)) return 0;
	if (!pefx->Add(0x04, TEXT("Sens"), TEXT("�Z���X"), TEXT("�t�B���^�[��ω������銴�x�𒲐߂��܂��B�l��傫������Ɠ��̓��x���ɉ����ăt�B���^�[�̎��g�����ς��₷���Ȃ�܂��B"), 0)) return 0;
	if (!pefx->Add(0x05, TEXT("+ Manual"), TEXT("�}�j���A��"), TEXT("���ʂ�^�������g���𒲐߂��܂��B"), 68)) return 0;
	if (!pefx->Add(0x06, TEXT("Peak"), TEXT("�s�[�N"), TEXT("����g���t�߂̃��E���ʂ̂������𒲐߂��܂��B�l������������Ɗ���g�����ӂ̍L���͈͂ŁA�l��傫������Ƌ����͈͂Ń��E���ʂ������܂��BLPF�̏ꍇ�ɂ́A�l������������ƃ��E���ʂ̕ω����������������܂��B"), 62)) return 0;
	if (!pefx->Add(0x07, TEXT("# Rate"), TEXT("���C�g"), TEXT("�h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 40)) return 0;
	if (!pefx->Add(0x08, TEXT("Depth"), TEXT("�f�v�X"), TEXT("�h��̐[���𒲐߂��܂��B"), 72)) return 0;
	if (!pefx->Add(0x09, TEXT("Polarity"), TEXT("�|�����e�B�["), TEXT("�t�B���^�[��ω�������ꍇ�ɁA�t�B���^�[���ǂ���̎��g�������ɓ�������ݒ肵�܂��BUp�ł̓t�B���^�[���������g�������ɓ����ADown�ł͒Ⴂ���g�������ɓ����܂��B"), pv8, sizeof(pv8) / sizeof(pv8[0]), 1)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("Pan"), TEXT("�A�E�g�v�b�g�E�p��"), TEXT("�o�͂���鉹�̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 96)) return 0;
	break;
case 11:
	if (!pefx->Add(0x03, TEXT("Low Slow"), TEXT("���[�E�t���P���V�[�E�X���[�E���C�g"), TEXT("��惍�[�^�[�̒ᑬ��]��(Slow)�̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 6)) return 0;
	if (!pefx->Add(0x04, TEXT("Low Fast"), TEXT("���[�E�t���P���V�[�E�t�@�[�X�g�E���C�g"), TEXT("��惍�[�^�[�̍�����]��(Fast)�̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 113)) return 0;
	if (!pefx->Add(0x05, TEXT("Low Accl"), TEXT("���[�E�t���P���V�[�E�A�N�Z�����[�V����"), TEXT("�ᑬ��]���獂����](�܂��͍�����]����ᑬ��])�ɐ؂�ւ����Ƃ��́A��惍�[�^�[�̉�]�������������܂ł̎��Ԃ𒲐߂��܂��B�l������������قǎ��Ԃ�������܂��B"), g_efxpv14, sizeof(g_efxpv14) / sizeof(g_efxpv14[0]), 3)) return 0;
	if (!pefx->Add(0x06, TEXT("Low Level"), TEXT("���[�E�t���P���V�[�E���x��"), TEXT("��惍�[�^�[�̉��ʂ𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x07, TEXT("Hi Slow"), TEXT("�n�C�E�t���P���V�[�E�X���[�E���C�g"), TEXT("���惍�[�^�[�̒ᑬ��]��(Slow)�̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 17)) return 0;
	if (!pefx->Add(0x08, TEXT("Hi Fast"), TEXT("�n�C�E�t���[�P���V�[�E�t�@�[�X�g�E���C�g"), TEXT("���惍�[�^�[�̍�����]��(Fast)�̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 120)) return 0;
	if (!pefx->Add(0x09, TEXT("Hi Accl"), TEXT("�n�C�E�t���P���V�[�E�A�N�Z�����[�V����"), TEXT("�ᑬ��]���獂����](�܂��͍�����]����ᑬ��])�ɐ؂�ւ����Ƃ��́A���惍�[�^�[�̉�]�������������܂ł̎��Ԃ𒲐߂��܂��B�l������������قǎ��Ԃ�������܂��B"), g_efxpv14, sizeof(g_efxpv14) / sizeof(g_efxpv14[0]), 11)) return 0;
	if (!pefx->Add(0x0a, TEXT("Hi Level"), TEXT("�n�C�E�t���P���V�[�E���x��"), TEXT("���惍�[�^�[�̉��ʂ𒲐߂��܂��B"), 64)) return 0;
	if (!pefx->Add(0x0b, TEXT("Separate"), TEXT("�Z�p���[�V����"), TEXT("���̍L�����𒲐߂��܂��B"), 96)) return 0;
	if (!pefx->Add(0x0d, TEXT("+ Speed"), TEXT("�X�s�[�h"), TEXT("��惍�[�^�[�ƍ��惍�[�^�[�̉�]���x�𓯎��ɐ؂�ւ��܂��B\n Slow �F�w��̉�]����(Low Slow�p�����[�^�[/Hi Slow�p�����[�^�[�̒l)�ɉ����܂��B\n Fast �F�w��̉�]����(Low Fast�p�����[�^�[/Hi Fast�p�����[�^�[�̒l)�ɏグ�܂��B"), pv9, sizeof(pv9) / sizeof(pv9[0]), 0)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 12:
	if (!pefx->Add(0x03, TEXT("Pre Filter"), TEXT("�v���E�t�B���^�[�E�^�C�v"), TEXT("�t�B���^�[�̎�ނ�I�т܂��B\n Off �F�t�B���^�[���g���܂���B\n LPF �FCutoff�p�����[�^�[����̑ш���J�b�g���܂��B\n HPF �FCutoff�p�����[�^�[��艺�̑ш���J�b�g���܂��B"), pv10, sizeof(pv10) / sizeof(pv10[0]), 0)) return 0;
	if (!pefx->Add(0x04, TEXT("Cutoff"), TEXT("�J�b�g�I�t�E�t���P���V�["), TEXT("�t�B���^�[�̊���g���𒲐߂��܂��B"), g_efxpv9, sizeof(g_efxpv9) / sizeof(g_efxpv9[0]), 0)) return 0;
	if (!pefx->Add(0x05, TEXT("Pre Dly"), TEXT("�v���E�f�B���C�E�^�C��"), TEXT("���������Ă���t�����W���[������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 16)) return 0;
	if (!pefx->Add(0x06, TEXT("+ Rate"), TEXT("���C�g"), TEXT("�h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 11)) return 0;
	if (!pefx->Add(0x07, TEXT("Depth"), TEXT("�f�v�X"), TEXT("�h��̐[���𒲐߂��܂��B"), 24)) return 0;
	if (!pefx->Add(0x08, TEXT("# Feedback"), TEXT("�t�B�[�h�o�b�N�E���x��"), TEXT("�G�t�F�N�g�����Ăѓ��͂ɖ߂��Ƃ��̊���(%)�𒲐߂��܂��B�}�C�i�X�̒l�ɂ����Ƃ��͈ʑ������]���܂��B"), 89, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x09, TEXT("Phase"), TEXT("�t�F�C�Y"), TEXT("���̍L�����𒲐߂��܂��B"), 90, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x12, TEXT("Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 104)) return 0;
	break;
case 13:
	if (!pefx->Add(0x03, TEXT("Pre Dly"), TEXT("�v���E�f�B���C�E�^�C��"), TEXT("���������Ă���t�����W���[������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 10)) return 0;
	if (!pefx->Add(0x04, TEXT("Rate"), TEXT("���C�g"), TEXT("�h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 5)) return 0;
	if (!pefx->Add(0x05, TEXT("Depth"), TEXT("�f�v�X"), TEXT("�h��̐[���𒲐߂��܂��B"), 95)) return 0;
	if (!pefx->Add(0x06, TEXT("+ Feedback"), TEXT("�t�B�[�h�o�b�N�E���x��"), TEXT("�G�t�F�N�g�����Ăѓ��͂ɖ߂��Ƃ��̊���(%)�𒲐߂��܂��B�}�C�i�X�̒l�ɂ����Ƃ��͈ʑ������]���܂��B"), 64, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x07, TEXT("Phase"), TEXT("�t�F�C�Y"), TEXT("���̍L�����𒲐߂��܂��B"), 90, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x08, TEXT("# Step Rate"), TEXT("�X�e�b�v�E���C�g"), TEXT("�s�b�`�ω��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 54)) return 0;
	if (!pefx->Add(0x12, TEXT("Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 104)) return 0;
	break;
case 14: case 15:
	if (!pefx->Add(0x03, TEXT("Mod Wave"), TEXT("���W�����[�V�����E�E�F�[�u"), TEXT("�h��̕ω��̂�������I�т܂��B\n Tri �F�O�p�g�̂悤�ɕω����܂��B\n Sqr �F��`�g�̂悤�ɕω����܂��B\n Sin �F�����g�̂悤�ɕω����܂��B\n Saw1,2 �F�̂�����g�̂悤�ɕω����܂��BSaw1��Saw2�ł́A�̂�����̐n�̌������t�ɂȂ��Ă��܂��B(Saw1�F�^|�^|�ASaw2�F�_|�_|)"), pv11, sizeof(pv11) / sizeof(pv11[0]), 1)) return 0;
	if (!pefx->Add(0x04, TEXT("+ Mod Rate"), TEXT("���W�����[�V�����E���C�g"), TEXT("�h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 60)) return 0;
	if (!pefx->Add(0x05, TEXT("# Mod Depth"), TEXT("���W�����[�V�����E�f�v�X"), TEXT("�h��̐[���𒲐߂��܂��B"), 96)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 16:
	if (!pefx->Add(0x03, TEXT("Attack"), TEXT("�A�^�b�N"), TEXT("������͂����Ƃ��̉��̗����オ�莞�Ԃ𒲐߂��܂��B"), 72)) return 0;
	if (!pefx->Add(0x04, TEXT("Sustain"), TEXT("�T�X�e�B��"), TEXT("�����ȃ��x���̉��������グ�Ĉ��̉��ʂɒB����܂ł̎��Ԃ𒲐߂��܂��B�l��傫������Ǝ��Ԃ��Z���Ȃ�܂��B�l��ς���ƃ��x�����ω����܂��B"), 100)) return 0;
	if (!pefx->Add(0x05, TEXT("Post Gain"), TEXT("�|�X�g�E�Q�C��"), TEXT("�o�̓Q�C���𒲐߂��܂��B"), pv12, sizeof(pv12) / sizeof(pv12[0]), 0)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("+ Pan"), TEXT("�A�E�g�v�b�g�E�p��"), TEXT("�o�͂���鉹�̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 104)) return 0;
	break;
case 17:
	if (!pefx->Add(0x03, TEXT("Threshold"), TEXT("�X���b�V�����h�E���x��"), TEXT("���k���n�߂鉹�ʃ��x���𒲐߂��܂��B"), 85)) return 0;
	if (!pefx->Add(0x04, TEXT("Ratio"), TEXT("�R���v���b�V�����E���V�I"), TEXT("�X���b�V�����h�E���x���ȏ�̐M���̈��k���𒲐߂��܂��B1/100���ł����k���������A�o�̓��x���͏������Ȃ�܂��B"), pv13, sizeof(pv13) / sizeof(pv13[0]), 3)) return 0;
	if (!pefx->Add(0x05, TEXT("Release"), TEXT("�����[�X�E�^�C��"), TEXT("���ʂ��X���b�V�����h�E���x���ȉ��ɂȂ��Ă�����ʂ��Ȃ��Ȃ�܂ł̎��Ԃ𒲐߂��܂��B"), 16)) return 0;
	if (!pefx->Add(0x06, TEXT("Post Gain"), TEXT("�|�X�g�E�Q�C��"), TEXT("�o�̓Q�C���𒲐߂��܂��B"), pv12, sizeof(pv12) / sizeof(pv12[0]), 0)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("+ Pan"), TEXT("�A�E�g�v�b�g�E�p��"), TEXT("�o�͂���鉹�̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 18:
	if (!pefx->Add(0x03, TEXT("Pre Dly"), TEXT("�v���E�f�B���C�E�^�C��"), TEXT("���������Ă���G�t�F�N�g������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 24)) return 0;
	if (!pefx->Add(0x04, TEXT("+ Rate"), TEXT("���C�g"), TEXT("�h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 8)) return 0;
	if (!pefx->Add(0x05, TEXT("Depth"), TEXT("�f�v�X"), TEXT("�h��̐[���𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x06, TEXT("Pre Dly Dev"), TEXT("�v���E�f�B���C�E�f�r�G�[�V����"), TEXT("�v���E�f�B���C�͌��������Ă���R�[���X������܂ł̎��Ԃł��B�����ł́A6���̊e�R�[���X���̃v���E�f�B���C�̍��𒲐߂��܂��B"), 5, 0x00, 0x14)) return 0;
	if (!pefx->Add(0x07, TEXT("Depth Dev"), TEXT("�f�v�X�E�f�r�G�[�V����"), TEXT("6���̊e�R�[���X���̗h��̐[���̍��𒲐߂��܂��B"), 22, 0x2C, 0x54, -20, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x08, TEXT("Pan Dev"), TEXT("�p���E�f�r�G�[�V����"), TEXT("6���̊e�R�[���X���̒�ʂ̍��𒲐߂��܂��B0�ł��ׂẴR�[���X���̒�ʂ������ɂȂ�܂��B20�ɂ���ƁA�����̒�ʂ���Ɋe�R�[���X����30�x�Ԋu�Œ�ʂ��܂��B"), 16, 0x00, 0x14)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 112)) return 0;
	break;
case 19:
	if (!pefx->Add(0x03, TEXT("Pre Dly"), TEXT("�v���E�f�B���C�E�^�C��"), TEXT("���������Ă���R�[���X������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 16)) return 0;
	if (!pefx->Add(0x04, TEXT("Cho Rate"), TEXT("�R�[���X�E���C�g"), TEXT("�R�[���X���ʂ̗h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 8)) return 0;
	if (!pefx->Add(0x05, TEXT("Cho Depth"), TEXT("�R�[���X�E�f�v�X"), TEXT("�R�[���X���ʂ̗h��̐[���𒲐߂��܂��B"), 40)) return 0;
	if (!pefx->Add(0x06, TEXT("Trem Phase"), TEXT("�g�������E�t�F�C�Y"), TEXT("�g���������̍L�����𒲐߂��܂��B"), 40, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x07, TEXT("+ Trem Rate"), TEXT("�g�������E���C�g"), TEXT("�g���������ʂ̗h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 60)) return 0;
	if (!pefx->Add(0x08, TEXT("Trem Sep"), TEXT("�g�������E�Z�p���[�V����"), TEXT("�g���������ʂ̍L�����𒲐߂��܂��B"), 96)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 127)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 20:
	if (!pefx->Add(0x03, TEXT("Pre Filter"), TEXT("�v���E�t�B���^�[�E�^�C�v"), TEXT("�t�B���^�[�̎�ނ�I�т܂��B\n Off �F�t�B���^�[���g���܂���B\n LPF �FCutoff�p�����[�^�[����̑ш���J�b�g���܂��B\n HPF �FCutoff�p�����[�^�[��艺�̑ш���J�b�g���܂��B"), pv10, sizeof(pv10) / sizeof(pv10[0]), 0)) return 0;
	if (!pefx->Add(0x04, TEXT("Cutoff"), TEXT("�J�b�g�I�t�E�t���P���V�["), TEXT("�R�[���X���̃t�B���^�[�̊���g���𒲐߂��܂��B"), g_efxpv9, sizeof(g_efxpv9) / sizeof(g_efxpv9[0]), 0)) return 0;
	if (!pefx->Add(0x05, TEXT("Pre Dly"), TEXT("�v���E�f�B���C�E�^�C��"), TEXT("���������Ă���R�[���X������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 10)) return 0;
	if (!pefx->Add(0x06, TEXT("+ Rate"), TEXT("���C�g"), TEXT("�h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 8)) return 0;
	if (!pefx->Add(0x07, TEXT("Depth"), TEXT("�f�v�X"), TEXT("�h��̐[���𒲐߂��܂��B"), 111)) return 0;
	if (!pefx->Add(0x08, TEXT("Phase"), TEXT("�t�F�C�Y"), TEXT("���̍L�����𒲐߂��܂��B"), 90, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 104)) return 0;
	break;
case 21:
	if (!pefx->Add(0x03, TEXT("Pre Dly"), TEXT("�v���E�f�B���C�E�^�C��"), TEXT("���������Ă���R�[���X������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 32)) return 0;
	if (!pefx->Add(0x04, TEXT("+ Rate"), TEXT("���C�g"), TEXT("�h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 8)) return 0;
	if (!pefx->Add(0x05, TEXT("Depth"), TEXT("�f�v�X"), TEXT("�h��̐[���𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x06, TEXT("Phase"), TEXT("�t�F�C�Y"), TEXT("���̍L�����𒲐߂��܂��B"), 90, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 96)) return 0;
	break;
case 22:
	if (!pefx->Add(0x03, TEXT("Pre Dly"), TEXT("�v���E�f�B���C�E�^�C��"), TEXT("���������Ă���R�[���X������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 10)) return 0;
	if (!pefx->Add(0x04, TEXT("+ Cho Rate"), TEXT("�R�[���X�E���C�g"), TEXT("�R�[���X���̗h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 8)) return 0;
	if (!pefx->Add(0x05, TEXT("Cho Depth"), TEXT("�R�[���X�E�f�v�X"), TEXT("�R�[���X���̗h��̐[���𒲐߂��܂��B"), 72)) return 0;
	if (!pefx->Add(0x11, TEXT("Out"), TEXT("�A�E�g�v�b�g�E���[�h"), TEXT("OUTPUT�W���b�N����o�͂����鉹���Đ�������@��ݒ肵�܂��B�X�s�[�J�[�Ŗ炷�Ƃ���Speaker���A�w�b�h�z���Ŗ炷�Ƃ���Phones��I�ԂƁA�œK��3D���ʂ������܂��B"), pv14, sizeof(pv14) / sizeof(pv14[0]), 0)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 80)) return 0;
	break;
case 23:
	if (!pefx->Add(0x03, TEXT("Dly Tm L"), TEXT("�f�B���C�E�^�C���E���t�g"), TEXT("���������Ă��獶�̃f�B���C������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv4, sizeof(g_efxpv4) / sizeof(g_efxpv4[0]), 101)) return 0;
	if (!pefx->Add(0x04, TEXT("Dly Tm R"), TEXT("�f�B���C�E�^�C���E���C�g"), TEXT("���������Ă���E�̃f�B���C������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv4, sizeof(g_efxpv4) / sizeof(g_efxpv4[0]), 116)) return 0;
	if (!pefx->Add(0x05, TEXT("+ Feedback"), TEXT("�t�B�[�h�o�b�N�E���x��"), TEXT("�G�t�F�N�g�����Ăѓ��͂ɖ߂��Ƃ��̊���(%)�𒲐߂��܂��B�}�C�i�X�̒l�ɂ����Ƃ��͈ʑ������]���܂��B"), 73, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x06, TEXT("Fb Mode"), TEXT("�t�B�[�h�o�b�N�E���[�h"), TEXT("�G�t�F�N�g������͂ɖ߂��Ƃ��̂Ȃ�������I�т܂��B\n Norm �F���̃f�B���C���͍��ɁA�E�̃f�B���C���͉E�ɖ߂��܂��B\n Cross �F���̃f�B���C���͉E�ɁA�E�̃f�B���C���͍��ɖ߂��܂��B"), pv15, sizeof(pv15) / sizeof(pv15[0]), 1)) return 0;
	if (!pefx->Add(0x07, TEXT("Phase L"), TEXT("�t�F�C�Y�E���t�g"), TEXT("���̃f�B���C���̈ʑ���I�т܂��B\n Norm �F�ʑ��͕ς��܂���B\n Invert �F�ʑ������]���܂��B"), pv16, sizeof(pv16) / sizeof(pv16[0]), 0)) return 0;
	if (!pefx->Add(0x08, TEXT("Phase R"), TEXT("�t�F�C�Y�E���C�g"), TEXT("�E�̃f�B���C���̈ʑ���I�т܂��B\n Norm �F�ʑ��͕ς��܂���B\n Invert �F�ʑ������]���܂��B"), pv16, sizeof(pv16) / sizeof(pv16[0]), 0)) return 0;
	if (!pefx->Add(0x0a, TEXT("HF Damp"), TEXT("HF�_���v"), TEXT("���͂ɖ߂����G�t�F�N�g���ɂ��āA���搬�����J�b�g������g���𒲐߂��܂��B�J�b�g���Ȃ��Ƃ���Bypass�ɐݒ肵�܂��B"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 24:
	if (!pefx->Add(0x03, TEXT("Dly Tm L"), TEXT("�f�B���C�E�^�C���E���t�g"), TEXT("���������Ă��獶�̃f�B���C������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv4, sizeof(g_efxpv4) / sizeof(g_efxpv4[0]), 90)) return 0;
	if (!pefx->Add(0x04, TEXT("Dly Tm R"), TEXT("�f�B���C�E�^�C���E���C�g"), TEXT("���������Ă���E�̃f�B���C������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv4, sizeof(g_efxpv4) / sizeof(g_efxpv4[0]), 108)) return 0;
	if (!pefx->Add(0x05, TEXT("Feedback"), TEXT("�t�B�[�h�o�b�N�E���x��"), TEXT("�G�t�F�N�g�����Ăѓ��͂ɖ߂��Ƃ��̊���(%)�𒲐߂��܂��B�}�C�i�X�̒l�ɂ����Ƃ��͈ʑ������]���܂��B"), 73, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x06, TEXT("Fb Mode"), TEXT("�t�B�[�h�o�b�N�E���[�h"), TEXT("�G�t�F�N�g������͂ɖ߂��Ƃ��̂Ȃ�������I�т܂��B\n Norm �F���̃f�B���C���͍��ɁA�E�̃f�B���C���͉E�ɖ߂��܂��B\n Cross �F���̃f�B���C���͉E�ɁA�E�̃f�B���C���͍��ɖ߂��܂��B"), pv15, sizeof(pv15) / sizeof(pv15[0]), 1)) return 0;
	if (!pefx->Add(0x07, TEXT("+ Mod Rate"), TEXT("���W�����[�V�����E���C�g"), TEXT("�h��̎����𒲐߂��܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 12)) return 0;
	if (!pefx->Add(0x08, TEXT("Mod Depth"), TEXT("���W�����[�V�����E�f�v�X"), TEXT("�h��̐[���𒲐߂��܂��B"), 21)) return 0;
	if (!pefx->Add(0x09, TEXT("Mod Phase"), TEXT("���W�����[�V�����E�t�F�C�Y"), TEXT("���̍L�����𒲐߂��܂��B"), 90, 0x00, 0x5a, 0, TEXT("%d"), 2)) return 0;
	if (!pefx->Add(0x0a, TEXT("HF Damp"), TEXT("HF�_���v"), TEXT("���͂ɖ߂����G�t�F�N�g���ɂ��āA���搬�����J�b�g������g���𒲐߂��܂��B�J�b�g���Ȃ��Ƃ���Bypass�ɐݒ肵�܂��B"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 61)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 25:
	if (!pefx->Add(0x03, TEXT("Dly Tm C"), TEXT("�f�B���C�E�^�C���E�Z���^�["), TEXT("���������Ă��璆���̃f�B���C������܂ł̎��Ԃ����ꂼ�꒲�߂��܂��B"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 20)) return 0;
	if (!pefx->Add(0x04, TEXT("Dly Tm L"), TEXT("�f�B���C�E�^�C���E���t�g"), TEXT("���������Ă��獶�̃f�B���C������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 0)) return 0;
	if (!pefx->Add(0x05, TEXT("Dly Tm R"), TEXT("�f�B���C�E�^�C���E���C�g"), TEXT("���������Ă���E�̃f�B���C������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 7)) return 0;
	if (!pefx->Add(0x06, TEXT("+ Feedback"), TEXT("�t�B�[�h�o�b�N�E���x��"), TEXT("�G�t�F�N�g�E�Z���^�[�����Ăѓ��͂ɖ߂��Ƃ��̊���(%)�𒲐߂��܂��B�}�C�i�X�̒l�ɂ����Ƃ��͈ʑ������]���܂��B"), 65, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x07, TEXT("Dly Lev C"), TEXT("�f�B���C�E���x���E�Z���^�["), TEXT("�����̃f�B���C���̉��ʂ𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x08, TEXT("Dly Lev L"), TEXT("�f�B���C�E���x���E���t�g"), TEXT("���̃f�B���C���̉��ʂ𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x09, TEXT("Dly Lev R"), TEXT("�f�B���C�E���x���E���C�g"), TEXT("�E�̃f�B���C���̉��ʂ𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x0a, TEXT("HF Damp"), TEXT("HF�_���v"), TEXT("�f�B���C�E�Z���^�[������͂ɖ߂����Ƃ��́A���搬�����J�b�g������g���𒲐߂��܂��B�J�b�g���Ȃ��Ƃ���Bypass�ɐݒ肵�܂��B"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 26:
	if (!pefx->Add(0x03, TEXT("Dly Tm 1"), TEXT("�f�B���C�E�^�C��1"), TEXT("���������Ă���A�f�B���C1�̉�����܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 60)) return 0;
	if (!pefx->Add(0x04, TEXT("Dly Tm 2"), TEXT("�f�B���C�E�^�C��2"), TEXT("���������Ă���A�f�B���C2�̉�����܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 20)) return 0;
	if (!pefx->Add(0x05, TEXT("Dly Tm 3"), TEXT("�f�B���C�E�^�C��3"), TEXT("���������Ă���A�f�B���C3�̉�����܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 40)) return 0;
	if (!pefx->Add(0x06, TEXT("Dly Tm 4"), TEXT("�f�B���C�E�^�C��4"), TEXT("���������Ă���A�f�B���C4�̉�����܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 0)) return 0;
	if (!pefx->Add(0x07, TEXT("Dly Lev 1"), TEXT("�f�B���C�E���x��1"), TEXT("�f�B���C1�̉��ʂ𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x08, TEXT("Dly Lev 2"), TEXT("�f�B���C�E���x��2"), TEXT("�f�B���C2�̉��ʂ𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x09, TEXT("Dly Lev 3"), TEXT("�f�B���C�E���x��3"), TEXT("�f�B���C3�̉��ʂ𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x0a, TEXT("Dly Lev 4"), TEXT("�f�B���C�E���x��4"), TEXT("�f�B���C4�̉��ʂ𒲐߂��܂��B"), 127)) return 0;
	if (!pefx->Add(0x0b, TEXT("+ Feedback"), TEXT("�t�B�[�h�o�b�N�E���x��"), TEXT("�f�B���C1���Ăѓ��͂ɖ߂��Ƃ��̊���(%)�𒲐߂��܂��B�}�C�i�X�̒l�ɂ����Ƃ��͈ʑ������]���܂��B"), 65, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x0c, TEXT("HF Damp"), TEXT("HF�_���v"), TEXT("�f�B���C1������͂ɖ߂����Ƃ��́A���搬�����J�b�g������g���𒲐߂��܂��B�J�b�g���Ȃ��Ƃ���Bypass�ɐݒ肵�܂��B"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 27:
	if (!pefx->Add(0x03, TEXT("+ Dly Time"), TEXT("�f�B���C�E�^�C��"), TEXT("���������Ă���f�B���C������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv3, sizeof(g_efxpv3) / sizeof(g_efxpv3[0]), 60)) return 0;
	if (!pefx->Add(0x04, TEXT("Accel"), TEXT("�A�N�Z�����[�V����"), TEXT("�f�B���C�E�^�C����ω��������ꍇ�A���݂̃f�B���C�E�^�C������w��̃f�B���C�E�^�C���ɒB����܂ł̎��Ԃ𒲐߂��܂��B�f�B���C�E�^�C���Ɠ����Ƀs�b�`�ω��̑������ς��܂��B"), g_efxpv14, sizeof(g_efxpv14) / sizeof(g_efxpv14[0]), 10)) return 0;
	if (!pefx->Add(0x05, TEXT("# Feedback"), TEXT("�t�B�[�h�o�b�N�E���x��"), TEXT("�G�t�F�N�g�����Ăѓ��͂ɖ߂��Ƃ��̊���(%)�𒲐߂��܂��B�}�C�i�X�̒l�ɂ����Ƃ��͈ʑ������]���܂��B"), 65, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x06, TEXT("HF Damp"), TEXT("HF�_���v"), TEXT("���͂ɖ߂����G�t�F�N�g���ɂ��āA���搬�����J�b�g������g���𒲐߂��܂��B�J�b�g���Ȃ��Ƃ���Bypass�ɐݒ肵�܂��B"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x07, TEXT("EFX Pan"), TEXT("�G�t�F�N�g�E�A�E�g�v�b�g�E�p��"), TEXT("�G�t�F�N�g���̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x12, TEXT("Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 28:
	if (!pefx->Add(0x03, TEXT("Type"), TEXT("���o�[�u�E�^�C�v"), TEXT("���o�[�u�̎�ނ�I�т܂��B\n Room1 �F�c�����Z���A���x�̔Z�����o�[�u\n Room2 �F�c�����Z���A���x�̔������o�[�u\n Stage1 �F�㕔�c�����̑������o�[�u\n Stage2 �F�������˂̋������o�[�u\n Hall1 �F���񂾋����̃��o�[�u\n Hall2 �F�L���ȋ����̃��o�[�u"), pv17, sizeof(pv17) / sizeof(pv17[0]), 3)) return 0;
	if (!pefx->Add(0x04, TEXT("Pre Dly"), TEXT("�v���E�f�B���C�E�^�C��"), TEXT("���������Ă��烊�o�[�u������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 112)) return 0;
	if (!pefx->Add(0x05, TEXT("+ Time"), TEXT("���o�[�u�E�^�C��"), TEXT("���o�[�u���̗]�C�̒����𒲐߂��܂��B"), 120)) return 0;
	if (!pefx->Add(0x06, TEXT("HF Damp"), TEXT("HF�_���v"), TEXT("�c�����̍��搬�����J�b�g������g���𒲐߂��܂��B���g����Ⴍ����قǍ��搬�����J�b�g����A���炩�Ȏc�����ɂȂ�܂��B���搬�����J�b�g���Ȃ��Ƃ���Bypass�ɂ��܂��B"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 13)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 29:
	if (!pefx->Add(0x03, TEXT("Type"), TEXT("�Q�[�g�E���o�[�u�E�^�C�v"), TEXT("���o�[�u�̎�ނ�I�т܂��B\n Norm �F�ʏ�̃Q�[�g�E���o�[�u�B\n Reverse �F�t��]�̃��o�[�u�B\n Sweep1 �F�c�������E���獶�ֈړ����܂��B\n Sweep2 �F�c������������E�ֈړ����܂��B"), pv18, sizeof(pv18) / sizeof(pv18[0]), 0)) return 0;
	if (!pefx->Add(0x04, TEXT("Pre Dly"), TEXT("�v���E�f�B���C�E�^�C��"), TEXT("���������Ă��烊�o�[�u������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 5)) return 0;
	if (!pefx->Add(0x06, TEXT("Gate Time"), TEXT("�Q�[�g�E�^�C��"), TEXT("���o�[�u������͂��߂Ă��������܂ł̎��Ԃ𒲐߂��܂��B"), 13, 0x00, 0x63, 0, TEXT("%d"), 5)) return 0;
	if (!pefx->Add(0x12, TEXT("+ Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 9, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 112)) return 0;
	break;
case 30:
	if (!pefx->Add(0x03, TEXT("Dly Tm C"), TEXT("�f�B���C�E�^�C���E�Z���^�["), TEXT("���������Ă��璆���̃f�B���C������܂ł̎��Ԃ����ꂼ�꒲�߂��܂��B"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 20)) return 0;
	if (!pefx->Add(0x04, TEXT("Dly Tm L"), TEXT("�f�B���C�E�^�C���E���t�g"), TEXT("���������Ă��獶�̃f�B���C������܂ł̎��Ԃ����ꂼ�꒲�߂��܂��B"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 0)) return 0;
	if (!pefx->Add(0x05, TEXT("Dly Tm R"), TEXT("�f�B���C�E�^�C���E���C�g"), TEXT("���������Ă���E�̃f�B���C������܂ł̎��Ԃ����ꂼ�꒲�߂��܂��B"), g_efxpv2, sizeof(g_efxpv2) / sizeof(g_efxpv2[0]), 8)) return 0;
	if (!pefx->Add(0x06, TEXT("+ Feedback"), TEXT("�t�B�[�h�o�b�N�E���x��"), TEXT("�f�B���C�E�Z���^�[�����Ăѓ��͂ɖ߂��Ƃ��̊���(%)�𒲐߂��܂��B�}�C�i�X�̒l�ɂ����Ƃ��͈ʑ������]���܂��B"), 65, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x07, TEXT("Dly Lev C"), TEXT("�f�B���C�E���x���E�Z���^�["), TEXT("�����̃f�B���C���̉��ʂ𒲐߂��܂��B"), 40)) return 0;
	if (!pefx->Add(0x08, TEXT("Dly Lev L"), TEXT("�f�B���C�E���x���E���t�g"), TEXT("���̃f�B���C���̉��ʂ𒲐߂��܂��B"), 64)) return 0;
	if (!pefx->Add(0x09, TEXT("Dly Lev R"), TEXT("�f�B���C�E���x���E���C�g"), TEXT("�E�̃f�B���C���̉��ʂ𒲐߂��܂��B"), 64)) return 0;
	if (!pefx->Add(0x0a, TEXT("HF Damp"), TEXT("HF�_���v"), TEXT("�f�B���C�E�Z���^�[������͂ɖ߂����Ƃ��́A���搬�����J�b�g������g���𒲐߂��܂��B�J�b�g���Ȃ��Ƃ���Bypass�ɐݒ肵�܂��B"), g_efxpv8, sizeof(g_efxpv8) / sizeof(g_efxpv8[0]), 15)) return 0;
	if (!pefx->Add(0x11, TEXT("Out"), TEXT("�A�E�g�v�b�g�E���[�h"), TEXT("OUTPUT�W���b�N����o�͂����鉹���Đ�������@��ݒ肵�܂��B�X�s�[�J�[�Ŗ炷�Ƃ���Speaker���A�w�b�h�z���Ŗ炷�Ƃ���Phones��I�ԂƁA�œK��3D���ʂ������܂��B"), pv14, sizeof(pv14) / sizeof(pv14[0]), 0)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 31:
	if (!pefx->Add(0x03, TEXT("+ Coarse 1"), TEXT("�R�[�X�E�s�b�`1"), TEXT("�s�b�`�E�V�t�g1�̃s�b�`�𔼉��P�ʂŒ��߂��܂�(-2�`+1�I�N�^�[�u)�B"), 31, 0x28, 0x4c, -24, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x04, TEXT("Fine 1"), TEXT("�t�@�C���E�s�b�`1"), TEXT("�s�b�`�E�V�t�g1�̃s�b�`��2�Z���g�P�ʂŔ����߂��܂�(-100�`+100�Z���g)�B"), 48, 0x0e, 0x72, -100, TEXT("%+d"), 2)) return 0;
	if (!pefx->Add(0x05, TEXT("Pre Dly 1"), TEXT("�v���E�f�B���C�E�^�C��1"), TEXT("���������Ă���s�b�`�E�V�t�g1�̉�����܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 0)) return 0;
	if (!pefx->Add(0x06, TEXT("EFX Pan 1"), TEXT("�G�t�F�N�g�E�A�E�g�v�b�g�E�p��1"), TEXT("�s�b�`�E�V�t�g1�̉��̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 127)) return 0;
	if (!pefx->Add(0x07, TEXT("# Coarse 2"), TEXT("�R�[�X�E�s�b�`2"), TEXT("�s�b�`�E�V�t�g2�̃s�b�`�𔼉��P�ʂŒ��߂��܂�(-2�`+1�I�N�^�[�u)�B"), 19, 0x28, 0x4c, -24, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x08, TEXT("Fine 2"), TEXT("�t�@�C���E�s�b�`2"), TEXT("�s�b�`�E�V�t�g2�̃s�b�`��2�Z���g�P�ʂŔ����߂��܂�(-100�`+100�Z���g)�B"), 52, 0x0e, 0x72, -100, TEXT("%+d"), 2)) return 0;
	if (!pefx->Add(0x09, TEXT("Pre Dly 2"), TEXT("�v���E�f�B���C�E�^�C��2"), TEXT("���������Ă���s�b�`�E�V�t�g2�̉�����܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 0)) return 0;
	if (!pefx->Add(0x0a, TEXT("EFX Pan 2"), TEXT("�G�t�F�N�g�E�A�E�g�v�b�g�E�p��2"), TEXT("�s�b�`�E�V�t�g2�̉��̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 0)) return 0;
	if (!pefx->Add(0x0b, TEXT("Shift Mode"), TEXT("�s�b�`�E�V�t�^�[�E���[�h"), TEXT("���l��傫������قǔ������x���Ȃ�܂����A���h�ꂪ���Ȃ��Ȃ�܂��B"), 3, 0x00, 0x04)) return 0;
	if (!pefx->Add(0x0c, TEXT("L.Bal"), TEXT("���x���E�o�����X"), TEXT("�s�b�`�E�V�t�g1�ƃs�b�`�E�V�t�g2�̉��ʃo�����X�𒲐߂��܂��B"), g_efxpv_o3, sizeof(g_efxpv_o3) / sizeof(g_efxpv_o3[0]), 64)) return 0;
	if (!pefx->Add(0x12, TEXT("Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 63)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 95)) return 0;
	break;
case 32:
	if (!pefx->Add(0x03, TEXT("P.Coarse"), TEXT("�R�[�X�E�s�b�`"), TEXT("�s�b�`�E�V�t�g�̃s�b�`�𔼉��P�ʂŒ��߂��܂�(-2�`+1�I�N�^�[�u)�B"), 31, 0x28, 0x4c, -24, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x04, TEXT("P.Fine"), TEXT("�t�@�C���E�s�b�`"), TEXT("�s�b�`�E�V�t�g�̃s�b�`��2�Z���g�P�ʂŔ����߂��܂�(-100�`+100�Z���g)�B"), 50, 0x0e, 0x72, -100, TEXT("%+d"), 2)) return 0;
	if (!pefx->Add(0x05, TEXT("# Feedback"), TEXT("�t�B�[�h�o�b�N�E���x��"), TEXT("�s�b�`�E�V�t�g�����Ăѓ��͂ɖ߂��Ƃ��̊���(%)�𒲐߂��܂��B�}�C�i�X�̒l�ɂ����Ƃ��͈ʑ������]���܂��B"), 69, 0x0f, 0x71, -98, TEXT("%+d%%"), 2)) return 0;
	if (!pefx->Add(0x06, TEXT("Pre Dly"), TEXT("�v���E�f�B���C�E�^�C��"), TEXT("���������Ă���s�b�`�E�V�t�g������܂ł̎��Ԃ𒲐߂��܂��B"), g_efxpv1, sizeof(g_efxpv1) / sizeof(g_efxpv1[0]), 95)) return 0;
	if (!pefx->Add(0x07, TEXT("Mode"), TEXT("�s�b�`�E�V�t�^�[�E���[�h"), TEXT("���l��傫������قǔ������x���Ȃ�܂����A���h�ꂪ���Ȃ��Ȃ�܂��B"), 3, 0x00, 0x04)) return 0;
	if (!pefx->Add(0x08, TEXT("EFX Pan"), TEXT("�G�t�F�N�g�E�A�E�g�v�b�g�E�p��"), TEXT("�s�b�`�E�V�t�g���̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x12, TEXT("Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 64)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 6, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 33:
	if (!pefx->Add(0x03, TEXT("Azimuth"), TEXT("�A�W�}�X"), TEXT("��]���~�߂��Ƃ��Ɏ~�܂�ʒu��ݒ肵�܂��B0�Œ����ɒ�ʂ��܂��B"), g_efxpv13, sizeof(g_efxpv13) / sizeof(g_efxpv13[0]), 15)) return 0;
	if (!pefx->Add(0x04, TEXT("+ Speed"), TEXT("�X�s�[�h"), TEXT("��]���鑬����ݒ肵�܂��B"), g_efxpv6, sizeof(g_efxpv6) / sizeof(g_efxpv6[0]), 25)) return 0;
	if (!pefx->Add(0x05, TEXT("Clockwise"), TEXT("�N���b�N���C�Y"), TEXT("��]������ݒ肵�܂��B-�Ŕ����v����ɁA+�Ŏ��v����ɉ�]���܂��B"), pv19, sizeof(pv19) / sizeof(pv19[0]), 1)) return 0;
	if (!pefx->Add(0x06, TEXT("# Turn"), TEXT("�^�[��"), TEXT("��]���~�߂���A���������肵�܂��BOn�ɂ���Ɖ�]���܂��BOff�ɂ���Ɖ�]��Azimuth�Őݒ肵���ʒu�Ŏ~�܂�܂��B"), pv4, sizeof(pv4) / sizeof(pv4[0]), 1)) return 0;
	if (!pefx->Add(0x11, TEXT("Out"), TEXT("�A�E�g�v�b�g�E���[�h"), TEXT("OUTPUT�W���b�N����o�͂����鉹���Đ�������@��ݒ肵�܂��B�X�s�[�J�[�Ŗ炷�Ƃ���Speaker���A�w�b�h�z���Ŗ炷�Ƃ���Phones��I�ԂƁA�œK��3D���ʂ������܂��B"), pv14, sizeof(pv14) / sizeof(pv14[0]), 0)) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 34:
	if (!pefx->Add(0x03, TEXT("+ Azimuth"), TEXT("�A�W�}�X"), TEXT("��ʂ�ݒ肵�܂��B0�Œ����ɒ�ʂ��܂��B"), g_efxpv13, sizeof(g_efxpv13) / sizeof(g_efxpv13[0]), 15)) return 0;
	if (!pefx->Add(0x11, TEXT("Out"), TEXT("�A�E�g�v�b�g�E���[�h"), TEXT("OUTPUT�W���b�N����o�͂����鉹���Đ�������@��ݒ肵�܂��B�X�s�[�J�[�Ŗ炷�Ƃ���Speaker���A�w�b�h�z���Ŗ炷�Ƃ���Phones��I�ԂƁA�œK��3D���ʂ������܂��B"), pv14, sizeof(pv14) / sizeof(pv14[0]), 0)) return 0;
	if (!pefx->Add(0x16, TEXT("# Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 35:
	if (!pefx->Add(0x03, TEXT("Pre Filter"), TEXT("�v���E�t�B���^�[�E�^�C�v"), TEXT("Lo-Fi��ʂ�O�̃t�B���^�[�̃^�C�v��ݒ肵�܂��B"), 1, 0x00, 0x05, 1)) return 0;
	if (!pefx->Add(0x04, TEXT("Lo-Fi Type"), TEXT("���[�E�t�@�C�E�^�C�v"), TEXT("������e�����܂��B�l���傫���قǉ������e���Ȃ�܂��B"), 5, 0x00, 0x08, 1)) return 0;
	if (!pefx->Add(0x05, TEXT("Post Filter"), TEXT("�|�X�g�E�t�B���^�[�E�^�C�v"), TEXT("Lo-Fi��ʂ�����̃t�B���^�[�̃^�C�v��ݒ肵�܂��B"), 1, 0x00, 0x05, 1)) return 0;
	if (!pefx->Add(0x12, TEXT("+ Balance"), TEXT("�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 127)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("# Pan"), TEXT("�A�E�g�v�b�g�E�p��"), TEXT("�o�͂���鉹�̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
case 36:
	if (!pefx->Add(0x03, TEXT("Lo-Fi Type"), TEXT("���[�E�t�@�C�E�^�C�v"), TEXT("������e�����܂��B�l���傫���قǉ������e���Ȃ�܂��B"), 1, 0x00, 0x05, 1)) return 0;
	if (!pefx->Add(0x04, TEXT("Fil Type"), TEXT("�t�B���^�[�E�^�C�v"), TEXT("Lo-Fi��ʂ�����̃t�B���^�[�̎�ނ�I�т܂��B"), pv10, sizeof(pv10) / sizeof(pv10[0]), 1)) return 0;
	if (!pefx->Add(0x05, TEXT("Cutoff"), TEXT("�J�b�g�I�t�E�t���P���V�["), TEXT("Lo-Fi��ʂ�����̃t�B���^�[�̊���g���𒲐߂��܂��B"), g_efxpv9, sizeof(g_efxpv9) / sizeof(g_efxpv9[0]), 4)) return 0;
	if (!pefx->Add(0x06, TEXT("+ R.Detune"), TEXT("���W�I�E�f�`���[��"), TEXT("���W�I�̃`���[�j���O�̃m�C�Y���V�~�����[�g���܂��B�l���傫���Ȃ�قǃ`���[�j���O������܂��B"), 0)) return 0;
	if (!pefx->Add(0x07, TEXT("R.Nz Lev"), TEXT("���W�I�E�m�C�Y�E���x��"), TEXT("���W�I�E�m�C�Y�̉��ʂ𒲐߂��܂��B"), 64)) return 0;
	if (!pefx->Add(0x08, TEXT("W/P Sel"), TEXT("�z���C�g/�s���N�E�m�C�Y�E�Z���N�g"), TEXT("�z���C�g�E�m�C�Y�܂��̓s���N�E�m�C�Y��I�����܂��B"), pv20, sizeof(pv20) / sizeof(pv20[0]), 1)) return 0;
	if (!pefx->Add(0x09, TEXT("W/P LPF"), TEXT("�z���C�g/�s���N�E�m�C�YLPF"), TEXT("�z���C�g�E�m�C�Y�܂��̓s���N�E�m�C�Y�ɂ����郍�[�E�p�X�E�t�B���^�[�̊���g���𒲐߂��܂��B"), g_efxpv11, sizeof(g_efxpv11) / sizeof(g_efxpv11[0]), 15)) return 0;
	if (!pefx->Add(0x0a, TEXT("W/P Level"), TEXT("�z���C�g/�s���N�E�m�C�Y�E���x��"), TEXT("�z���C�g�E�m�C�Y�܂��̓s���N�E�m�C�Y�̉��ʂ𒲐߂��܂��B"), 0)) return 0;
	if (!pefx->Add(0x0b, TEXT("Disc Type"), TEXT("�f�B�X�N�E�m�C�Y�E�^�C�v"), TEXT("���R�[�h�E�m�C�Y�̎�ނ�I�т܂��B�^�C�v�ɂ���ăm�C�Y�̏o��p�x���ς��܂��B"), pv21, sizeof(pv21) / sizeof(pv21[0]), 0)) return 0;
	if (!pefx->Add(0x0c, TEXT("Disc LPF"), TEXT("�f�B�X�N�E�m�C�YLPF"), TEXT("���R�[�h�E�m�C�Y�ɂ����郍�[�E�p�X�E�t�B���^�[�̊���g���𒲐߂��܂��B"), g_efxpv11, sizeof(g_efxpv11) / sizeof(g_efxpv11[0]), 15)) return 0;
	if (!pefx->Add(0x0d, TEXT("Disc Nz Lev"), TEXT("�f�B�X�N�E�m�C�Y�E���x��"), TEXT("���R�[�h�E�m�C�Y�̉��ʂ𒲐߂��܂��B"), 0)) return 0;
	if (!pefx->Add(0x0e, TEXT("Hum Type"), TEXT("�n���E�m�C�Y�E�^�C�v"), TEXT("�n���E�m�C�Y�̎�ނ�I�т܂��B"), 0, 0x00, 0x01, 50, TEXT("%dHz"), 10)) return 0;
	if (!pefx->Add(0x0f, TEXT("Hum LPF"), TEXT("�n���E�m�C�YLPF"), TEXT("�n���E�m�C�Y�ɂ����郍�[�E�p�X�E�t�B���^�[�̊���g���𒲐߂��܂��B"), g_efxpv11, sizeof(g_efxpv11) / sizeof(g_efxpv11[0]), 15)) return 0;
	if (!pefx->Add(0x10, TEXT("Hum Level"), TEXT("�n���E�m�C�Y�E���x��"), TEXT("�n���E�m�C�Y�̉��ʂ𒲐߂��܂��B"), 0)) return 0;
	if (!pefx->Add(0x11, TEXT("M/S"), TEXT("���m����/�X�e���I�E�X�C�b�`"), TEXT("�G�t�F�N�g�������m�����ɂ��邩�X�e���I�ɂ��邩��I�т܂��B"), pv22, sizeof(pv22) / sizeof(pv22[0]), 1)) return 0;
	if (!pefx->Add(0x12, TEXT("# Balance"), TEXT("�G�t�F�N�g�E�o�����X"), TEXT("�����ƃG�t�F�N�g���̉��ʃo�����X�𒲐߂��܂��B�\���́gD�h�A�gE�h�́AD(�h���C��)�AE(�G�t�F�N�g��)�̒l�����ꂼ��100�ł��邱�Ƃ�\���܂��B"), g_efxpv_o2, sizeof(g_efxpv_o2) / sizeof(g_efxpv_o2[0]), 127)) return 0;
	if (!pefx->Add(0x13, TEXT("Low Gain"), TEXT("���[�E�Q�C��"), TEXT("���̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x14, TEXT("Hi Gain"), TEXT("�n�C�E�Q�C��"), TEXT("����̃Q�C���𒲐߂��܂��B"), 12, 0x34, 0x4c, -12, TEXT("%+d"))) return 0;
	if (!pefx->Add(0x15, TEXT("Pan(Mono)"), TEXT("�A�E�g�v�b�g�E�p���E���m����"), TEXT("���m�����E���[�h�̂Ƃ��ɏo�͂����G�t�F�N�g���̒�ʂ𒲐߂��܂��BL63�ōł����A0�Œ����AR63�ōł��E�ɒ�ʂ��܂��B"), g_efxpv_o1, sizeof(g_efxpv_o1) / sizeof(g_efxpv_o1[0]), 63)) return 0;
	if (!pefx->Add(0x16, TEXT("Level"), TEXT("�A�E�g�v�b�g�E���x��"), TEXT("�o�͉��ʂ𒲐߂��܂��B"), 127)) return 0;
	break;
}
				break;
			}
			break;
		}
		pefx->Command(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
		return 0;
	case WM_DESTROY:
		delete pefx;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	MSG msg;
	INITCOMMONCONTROLSEX iccex;

	// ���d�N���h�~�B
	CreateMutex(NULL, FALSE, TEXT("sc88pefx"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;

	// �E�B���h�E�N���X�o�^�B
	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("sc88pefx");
	if (RegisterClass(&wc) == 0) return 0;

	g_hinst = hInstance;

	// �t�H���g�쐬�B
	if ((g_hfont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
	OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL)) == NULL) return 0;

	// �R�����R���g���[���̎g�p�����B
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_BAR_CLASSES;
	if (!InitCommonControlsEx(&iccex)) return 0;

	// �g�b�v�E�B���h�E�쐬�B
	if (CreateWindow(TEXT("sc88pefx"), TEXT("SC-88ProEfxEdit"), WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE,
	100, 100, 580, 530, NULL, NULL, hInstance, NULL) == NULL) return 0;

	// ���b�Z�[�W���[�v�B
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(g_hfont);
	return msg.wParam;
}
