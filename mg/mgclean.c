#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>
#include "mg.h"
#define CHR_NONE 0
#define CHR_INFO 1
#define CHR_FILES 2
#define CHR_ARCADE 3
#define CHR_DATA 4
#define CHR_SIZE 5
#define CHR_VELOCITY 6
#define CHR_MOVEMENT 7
#define CHR_STATEDEF 8
#define CHR_STATE 9
#define CHR_COMMAND 10
#define CHR_REMAP 11
#define CHR_DEFAULTS 12
#define MAX_SND 256
#define MAX_LINE 1024

char is_command, is_air;

/* ZSoft pcx file header (128 bytes) */
typedef struct {
	char manufacturer;	/* Constant Flag, 10 = ZSoft .pcx */
	char version;	/* Version information
						0 = Version 2.5 of PC Paintbrush
						2 = Version 2.8 w/palette information
						3 = Version 2.8 w/o palette information
						4 = PC Paintbrush for Windows(Plus for Windows uses Ver 5)
						5 = Version 3.0 and > of PC Paintbrush and PC Paintbrush +,
						    includes Publisher's Paintbrush. Includes 24-bit .PCX files */
	char encoding;	/* 1 = .PCX run length encoding */
	char bits_per_pixel;	/* Number of bits to represent a pixel(per Plane) - 1, 2, 4, or 8 */
	struct {
		short x_min;
		short y_min;
		short x_max;
		short y_max;
	} window;	/* Image Dimensions: Xmin,Ymin,Xmax,Ymax */
	short h_dpi;	/* Horizontal Resolution of image in DPI* */
	short v_dpi;	/* Vertical Resolution of image in DPI* */
	char colormap[48];	/* Color palette setting */
	char reserved;	/* Should be set to 0 */
	char n_planes;	/* Number of color planes */
	short bytes_per_line;	/* Number of bytes to allocate for a scanline plane.
								MUST be an EVEN number. Do NOT calculate from Xmax-Xmin. */
	short palette_info;	/* How to interpret palette- 1 = Color/BW, 2 = Grayscale (ignored in PB IV/ IV +) */
	short h_screen_size;	/* Horizontal screen size in pixels. New field found only in PB IV/IV Plus */
	short v_screen_size;	/* Vertical screen size in pixels. New field found only in PB IV/IV Plus */
	char filler[54];	/* Blank to fill out 128 byte header. Set all bytes to 0 */
} PCX_HEADER;

typedef struct {
	unsigned char r, g, b;
} RGB_PALETTE;

typedef struct {
	short left, top;
} CROP;

typedef struct {
	char name[MAX_LINE];
	char value[MAX_LINE];
} PARAM;
typedef struct {
	int no;
	char str[640][64];	/* 改良の余地有り */
} AIR;
typedef struct {
	int group, sample;
	char *data;
} SND;

/* 前後の余白を削除した文字列ポインタを返す */
char *strTrim(char *str)
{
	char *sp;

	if (str == NULL || *str == '\0') return str;

	if ((sp = strchr(str, ';')) != NULL && sp != str) *sp = '\0';

	while (*str == ' ' || *str == '\t' || *str == '\n') str++;
	sp = str;
	while (*sp != '\0') sp++;
	do sp--; while (*sp == ' ' || *sp == '\t' || *sp == '\n');
	sp[1] = '\0';

	return str;
}

/* 小文字に変換 */
char *str2Lower(char *str)
{
	char *sp;

	if (str == NULL) return NULL;

	sp = str;
	do if (__isascii(*sp) && isupper(*sp)) *sp = tolower(*sp); while(*++sp != '\0');

	return str;
}

/* Stateを設定 */
void setState(char *state, char *statestr, char *str)
{
	char buf[MAX_LINE], *sp;

	*statestr = '\0';
	if (str == NULL || *str == '\0') return;

	*statestr = '0';
	if (!stricmp(str, "[info]")) *state = CHR_INFO;
	else if (!stricmp(str, "[files]")) *state = CHR_FILES;
	else if (!stricmp(str, "[arcade]")) *state = CHR_ARCADE;
	else if (!stricmp(str, "[data]")) *state = CHR_DATA;
	else if (!stricmp(str, "[size]")) *state = CHR_SIZE;
	else if (!stricmp(str, "[velocity]")) *state = CHR_VELOCITY;
	else if (!stricmp(str, "[movement]")) *state = CHR_MOVEMENT;
	else if (!_strnicmp(str, "[statedef", 9)) {
		strcpy(buf, str + 9);
		if ((sp = strchr(buf, ']')) == NULL) return;

		*state = CHR_STATEDEF;
		*sp = '\0';
		sp = strTrim(buf);
		strcpy(statestr, _ltoa(strtol(sp, NULL, 10), sp, 10));
	} else if (!_strnicmp(str, "[state", 6)) *state = CHR_STATE;
	else if (!stricmp(str, "[command]")) {
		*state = CHR_COMMAND;
		is_command = 1;
	} else if (!stricmp(str, "[remap]")) *state = CHR_REMAP;
	else if (!stricmp(str, "[defaults]")) *state = CHR_DEFAULTS;
	else if (!_strnicmp(str, "[begin action", 13)) {
		strcpy(buf, str + 13);
		if ((sp = strchr(buf, ']')) == NULL) return;

		*state = 1;
		is_air = 1;
		*sp = '\0';
		sp = strTrim(buf);
		strcpy(statestr, _ltoa(strtol(sp, NULL, 10), sp, 10));
	} else
		*statestr = '\0';
}

/* パラメータを設定 */
PARAM *addParam(PARAM *param, char *str)
{
	char buf[MAX_LINE], *bp;
	int index;
	size_t size = param == NULL ? 0 : _msize(param);

	if ((param = (PARAM *)realloc(param, size + sizeof(PARAM))) == NULL) return NULL;

	index = size / sizeof(PARAM);
	strcpy(buf, str);
	if (strchr(buf, ';') == NULL && (bp = strchr(buf, '=')) != NULL) {
		strcpy(param[index].value, strTrim(bp + 1));

		*bp = '\0';
		bp = strTrim(buf);
		size = strlen(bp);
		strncpy(param[index].name, bp, size);
		param[index].name[size] = '\0';
	} else {
		param[index].name[0] = '\0';
		strcpy(param[index].value, buf);
	}

	return param;
}

/* airを設定 */
AIR *addAir(AIR *air, char *nostr, char *str)
{
	size_t size = air == NULL ? 0 : _msize(air);
	int actionno = strtol(nostr, NULL, 10), index = size / sizeof(AIR), i;

	for (i = 0; i < index; i++)
		if (air[i].no == actionno) break;
	if (i < index) {
		index = i;
		for (i = 0; air[index].str[i][0] != '\0'; i++);
	} else {
		if ((air = (AIR *)realloc(air, size + sizeof(AIR))) == NULL) return NULL;
		air[index].no = actionno;
		air[index].str[0][0] = '\0';
		i = 0;
	}

	if (strchr(str, ';') != NULL) return air;

	air[index].str[i + 1][0] = '\0';
	strcpy(air[index].str[i], str);

	return air;
}

/* キーワードを変換した文字列ポインタを返す */
char *prepareValue(char *str, char ignore)
{
	static char *triggers[] = {
		"Abs",
		"Acos",
		"Alive",
		"Anim",
		"AnimElem",
		"AnimElemNo",
		"AnimElemTime",
		"AnimExist",
		"AnimTime",
		"Asin",
		"Atan",
		"AuthorName",
		"BackEdgeBodyDist",
		"BackEdgeDist",
		"CanRecover",
		"Ceil",
		"Command",
		"Const",
		"Cos",
		"Ctrl",
		"DrawGame",
		"E",
		"Exp",
		"Facing",
		"Floor",
		"FrontEdgeBodyDist",
		"FrontEdgeDist",
		"FVar",
		"GameTime",
		"GetHitVar",
		"HitCount",
		"HitDefAttr",
		"HitFall",
		"HitOver",
		"HitPauseTime",
		"HitShakeOver",
		"HitVel X",
		"HitVel Y",
		"ID",
		"IfElse",
		"InGuardDist",
		"IsHelper",
		"IsHomeTeam",
		"Life",
		"LifeMax",
		"Log",
		"Ln",
		"Lose",
		"LoseKO",
		"LoseTime",
		"MatchNo",
		"MatchOver",
		"MoveContact",
		"MoveGuarded",
		"MoveHit",
		"MoveType",
		"MoveReversed",
		"Name",
		"NumEnemy",
		"NumExplod",
		"NumHelper",
		"NumPartner",
		"NumProj",
		"NumProjID",
		"NumTarget",
		"P1Name",
		"P2BodyDist X",
		"P2BodyDist Y",
		"P2Dist X",
		"P2Dist Y",
		"P2Life",
		"P2MoveType",
		"P2Name",
		"P2StateNo",
		"P2StateType",
		"P3Name",
		"P4Name",
		"PalNo",
		"ParentDist X",
		"ParentDist Y",
		"Pi",
		"Pos X",
		"Pos Y",
		"Power",
		"PowerMax",
		"PlayerIDExist",
		"PrevStateNo",
		"ProjCancelTime",
		"ProjContact",
		"ProjContactTime",
		"ProjGuarded",
		"ProjGuardedTime",
		"ProjHit",
		"ProjHitTime",
		"Random",
		"RootDist X",
		"RootDist Y",
		"RoundNo",
		"RoundsExisted",
		"RoundState",
		"ScreenPos X",
		"ScreenPos Y",
		"SelfAnimExist",
		"Sin",
		"StateNo",
		"StateType",
		"StateTime",
		"SysFVar",
		"SysVar",
		"Tan",
		"TeamMode",
		"TeamSide",
		"TicksPerSecond",
		"Time",
		"TimeMod",
		"UniqHitCount",
		"Var",
		"Vel X",
		"Vel Y",
		"Win",
		"WinKO",
		"WinTime",
		"WinPerfect"
	}, *exps1[] = {
		"+", "-", "*", "/", "%",
		"**", "&&", "||", "^^",
		"&", "|", "^", "=", ":=", "!=",
		"<", "<=", ">", ">=", ",", ".", ":"
	}, *exps2[] = {
		"parent",
		"root",
		"helper",
		"helper(?",
		"target",
		"target(?",
		"partner",
		"enemy",
		"enemy(?",
		"enemyNear",
		"enemyNear(?",
		"playerID(?"
	};
	char *sp1, *sp2, *sp3;
	int cnt, trig_cnt, str_cnt, dq_cnt, i, j, k;

	/* triggerキーワードを変換 */
	cnt = sizeof(triggers) / sizeof(triggers[0]);
	for (i = 0; i < cnt; i++) {
		trig_cnt = strlen(triggers[i]);
		str_cnt = strlen(str) - trig_cnt;
		for (j = 0; j <= str_cnt; j++) {
			/* キーワードを検索し、そのキーワードの前後の文字が適切ならcontinueしない */
			if (_strnicmp(str + j, triggers[i], trig_cnt) ||
			j > 0 && (isalpha(str[j - 1]) || isdigit(str[j - 1])) ||
			j + trig_cnt < str_cnt && isalpha(str[j + trig_cnt])) continue;
			/* キーワードが｢"｣で囲まれていればcontinue */
			dq_cnt = 0;
			for (k = j - 1; k >= 0; k--)
				if (str[k] == '"') dq_cnt++;
			if (dq_cnt & 1) continue;

			for (k = 0; k < trig_cnt; k++) str[j + k] = triggers[i][k];

			j += trig_cnt - 1;
		}
	}

	/* expressionキーワードの前後を変換 */
	cnt = sizeof(exps1) / sizeof(exps1[0]);
	for (i = 0; i < cnt; i++) {
		sp1 = str;
		while ((sp1 = strstr(sp1, exps1[i])) != NULL) {
			/* キーワードが｢"｣で囲まれている、または特殊条件でcontinue */
			dq_cnt = 0;
			for (sp2 = sp1 - 1; sp2 >= str; sp2--)
				if (*sp2 == '"') dq_cnt++;
			if ((dq_cnt & 1) || ignore && (exps1[i][0] == '+' || exps1[i][0] == '/')) {
				sp1++;
				continue;
			}

			if (exps1[i][0] == ',' || exps1[i][0] == ':') {	/* 特殊処理 */
				j = 0;
				for (sp2 = sp1 - 1; sp2 >= str; sp2--)
					if (*sp2 == ' ' || *sp2 == '\t') j--;
					else break;

				/* キーワードの前の不要な半角スペースを変換 */
				if (j < 0) {
					sp2 = sp1;
					sp1 += j;
					while (*sp2 != '\0') sp2[j] = *sp2++;
					sp2[j] = '\0';
				}

				for (sp2 = sp1 - 1, j = 0; sp2 >= str; sp2--) {
					if (*sp2 == ')') j++;
					if (!isalnum(*sp2) && (*sp2 != '(' || j < 1) && *sp2 != ')') break;
					if (*sp2 == '(') j--;
				}
				sp2++;

				/* キーワードがあれば変換して、半角スペースの必要個数を0にする */
				j = 1;
				str_cnt = sizeof(exps2) / sizeof(exps2[0]);
				for (k = 0; k < str_cnt; k++) {
					trig_cnt = (sp3 = strchr(exps2[k], '?')) == NULL ? strlen(exps2[k]) : sp3 - exps2[k];
					if (!_strnicmp(sp2, exps2[k], trig_cnt)) {
						memcpy(sp2, exps2[k], trig_cnt);

						j = 0;
						break;
					}
				}

				trig_cnt = 1;
			} else if (exps1[i][0] == '.') {	/* 特殊処理 */
				j = 0;
				for (sp2 = sp1 - 1; sp2 >= str; sp2--) {
					if (!isdigit(*sp2)) break;
					if (*sp2 == '0')
						j--;
					else {
						j = 0;
						break;
					}
				}

				/* 不要な｢0｣を変換 */
				if (j < 0) {
					sp2 = sp1;
					sp1 += j;
					while (*sp2 != '\0') sp2[j] = *sp2++;
					sp2[j] = '\0';
				}

				sp1++;
				continue;
			} else {
				/* キーワードの前後にキーワードがあればcontinue */
				if (sp1 != str) {
					/* 前 */
					sp2 = sp1 - 1;
					if (*sp2 == '*' || *sp2 == '&' || *sp2 == '|' || *sp2 == '^' || *sp2 == '=' ||
					*sp2 == ':' || *sp2 == '!' || *sp2 == '<' || *sp2 == '>') {
						sp1++;
						continue;
					}
				}
				trig_cnt = strlen(exps1[i]);
				if (str + strlen(str) > sp1 + trig_cnt) {
					/* 後 */
					sp2 = sp1 + trig_cnt;
					if (*sp2 == '*' || *sp2 == '&' || *sp2 == '|' || *sp2 == '^' || *sp2 == '=' ||
					*sp2 == ':' || *sp2 == '!' || *sp2 == '<' || *sp2 == '>') {
						sp1++;
						continue;
					}
				} else {
					sp1++;
					continue;
				}

				/* キーワードの前を変換 */
				if (sp1 != str) {
					j = 1;
					for (sp2 = sp1 - 1; sp2 >= str; sp2--) {
						if (*sp2 == ' ' || *sp2 == '\t') continue;
						if (*sp2 == '[' || *sp2 == '(') j = 0;
						break;
					}
					str_cnt = j;	/* 半角スペースの必要個数 */

					for (sp2 = sp1 - 1; sp2 >= str; sp2--) {
						if (*sp2 == '\t') *sp2 = ' ';
						if (*sp2 == ' ') str_cnt--;
						else break;
					}

					if (str_cnt > 0) {	/* 半角スペースが必要？ */
						sp2 = sp1;
						while (*sp2 != '\0') sp2++;
						for (;; sp2--) {
							sp2[1] = *sp2;
							if (sp2 == sp1) break;
						}
						*sp2 = ' ';
						sp1++;
					} else if (str_cnt < 0) {	/* 半角スペースが多すぎた？ */
						sp2 = sp1;
						sp1 += str_cnt;
						while (*sp2 != '\0') sp2[str_cnt] = *sp2++;
						sp2[str_cnt] = '\0';
					}
				}

				/* キーワードの前にキーワードがあれば半角スペース必要個数を0にする */
				if (sp1 == str)
					j = 0;
				else {
					j = 1;
					for (sp2 = sp1 - 1; sp2 >= str; sp2--) {
						if (*sp2 == ' ' || *sp2 == '\t') continue;
						if (*sp2 == '+' || *sp2 == '-' || *sp2 == '*' || *sp2 == '/' || *sp2 == '%' ||
						*sp2 == '!' || *sp2 == '&' || *sp2 == '|' || *sp2 == '^' || *sp2 == '~' ||
						*sp2 == '=' || *sp2 == '<' || *sp2 == '>' || *sp2 == '[' || *sp2 == '(' ||
						*sp2 == ',' && strcspn(exps1[i], "=!<>")) j = 0;
						break;
					}
				}
			}

			/* キーワードの後を変換 */
			if (str + strlen(str) > sp1 + trig_cnt) {
				str_cnt = j;	/* 半角スペースの必要個数 */

				for (sp2 = sp1 + trig_cnt; sp2 != '\0'; sp2++) {
					if (*sp2 == '\t') *sp2 = ' ';
					if (*sp2 == ' ') str_cnt--;
					else break;
				}

				if (str_cnt > 0) {	/* 半角スペースが必要？ */
					sp2 = sp1 + trig_cnt;
					while (*sp2 != '\0') sp2++;
					for (;; sp2--) {
						sp2[1] = *sp2;
						if (sp2 == sp1 + trig_cnt) break;
					}
					*sp2 = ' ';
				} else if (str_cnt < 0) {	/* 半角スペースが多すぎた？ */
					sp2 = sp1 + trig_cnt;
					str_cnt *= -1;
					for (;; sp2++) {
						*sp2 = sp2[str_cnt];
						if (*sp2 == '\0') break;
					}
				}
			}

			sp1++;
		}
	}

	return str;
}

/* 指定出力する */
void printPreparedCommParams(PARAM *param, int cnt, char *name, int name_cnt, FILE *f)
{
	int i;

	for (i = 0; i < cnt; i++) {
		if (name_cnt == 0 && stricmp(param[i].name, name) ||
		name_cnt > 0 && _strnicmp(param[i].name, name, name_cnt)) continue;

		fprintf(f, "%s = %s\n", str2Lower(param[i].name), prepareValue(param[i].value, 0));
		param[i].name[0] = '\1';
	}
}

/* 順番通りに出力する */
void printPreparedParams(PARAM *param, char state, char *stateno, FILE *f)
{
	static char *chr[][29] = {
		{/* Info */
			"name",
			"displayname",
			"versiondate",
			"mugenversion",
			"author",
			"pal.defaults"
		},
		{/* Files */
			"cmd",
			"cns",
			"st", "st1", "st2", "st3", "st4", "st5", "st6", "st7", "st8", "st9",
			"stcommon",
			"sprite",
			"anim",
			"sound",
			"pal1", "pal2", "pal3", "pal4", "pal5", "pal6",
			"pal7", "pal8", "pal9", "pal10", "pal11", "pal12"
		},
		{/* Arcade */
			"intro.storyboard",
			"ending.storyboard"
		}
	}, *cns[][17] = {
		{/* Data */
			"life",
			"attack",
			"defence",
			"power",
			"fall.defence_up",
			"liedown.time",
			"airjuggle",
			"sparkno",
			"guard.sparkno",
			"KO.echo",
			"volume",
			"IntPersistIndex",
			"FloatPersistIndex"
		},
		{/* Size */
			"xscale",
			"yscale",
			"ground.back",
			"ground.front",
			"air.back",
			"air.front",
			"z.width",
			"height",
			"attack.width",
			"attack.dist",
			"proj.attack.dist",
			"proj.doscale",
			"head.pos",
			"mid.pos",
			"shadowoffset",
			"draw.offset"
		},
		{/* Velocity */
			"walk.fwd",
			"walk.back",
			"run.fwd",
			"run.back",
			"jump.neu",
			"jump.back",
			"jump.fwd",
			"runjump.back",
			"runjump.fwd",
			"airjump.neu",
			"airjump.back",
			"airjump.fwd"
		},
		{/* Movement */
			"airjump.num",
			"airjump.height",
			"yaccel",
			"stand.friction",
			"crouch.friction"
		},
		{/* Command */
			"name",
			"command",
			"time"
		},
		{/* Remap */
			"x", "y", "z", "a", "b", "c", "s"
		},
		{/* Defaults */
			"command.time",
			"command.buffer.time"
		}
	}, *sctrl_param[] = {
		"type",
		"movetype",
		"physics",
		"anim",
		"velset",
		"ctrl",
		"poweradd",
		"juggle",
		"facep2",
		"hitdefpersist",
		"movehitpersist",
		"hitcountpersist",
		"sprpriority"
	}, *sctrls[] = {
		"AfterImage",
		"AfterImageTime",
		"AllPalFX",
		"AngleAdd",
		"AngleDraw",
		"AngleMul",
		"AngleSet",
		"AppendToClipboard",
		"AssertSpecial",
		"AttackDist",
		"AttackMulSet",
		"BGPalFX",
		"BindToParent",
		"BindToRoot",
		"BindToTarget",
		"ChangeAnim",
		"ChangeAnim2",
		"ChangeState",
		"ClearClipboard",
		"CtrlSet",
		"DefenceMulSet",
		"DestroySelf",
		"DisplayToClipboard",
		"EnvColor",
		"EnvShake",
		"Explod",
		"ExplodBindTime",
		"ForceFeedback",
		"FallEnvShake",
		"GameMakeAnim",
		"Gravity",
		"Helper",
		"HitAdd",
		"HitBy",
		"HitDef",
		"HitFallDamage",
		"HitFallSet",
		"HitFallVel",
		"HitOverride",
		"HitVelSet",
		"LifeAdd",
		"LifeSet",
		"MakeDust",
		"ModifyExplod",
		"MoveHitReset",
		"NotHitBy",
		"Null",
		"Offset",
		"PalFX",
		"ParentVarAdd",
		"ParentVarSet",
		"Pause",
		"PlayerPush",
		"PlaySnd",
		"PosAdd",
		"PosFreeze",
		"PosSet",
		"PowerAdd",
		"PowerSet",
		"Projectile",
		"RemoveExplod",
		"ReversalDef",
		"ScreenBound",
		"SelfState",
		"SprPriority",
		"StateTypeSet",
		"SndPan",
		"StopSnd",
		"SuperPause",
		"TargetBind",
		"TargetDrop",
		"TargetFacing",
		"TargetLifeAdd",
		"TargetPowerAdd",
		"TargetState",
		"TargetVelAdd",
		"TargetVelSet",
		"Trans",
		"Turn",
		"VarAdd",
		"VarRandom",
		"VarRangeSet",
		"VarSet",
		"VelAdd",
		"VelMul",
		"VelSet",
		"Width"
	}, *params[][120] = {
		{/* AfterImage */
			"time",
			"length",
			"palcolor",
			"palinvertall",
			"palbright",
			"palcontrast",
			"palpostbright",
			"paladd",
			"palmul",
			"timegap",
			"framegap",
			"trans"
		},
		{/* AfterImageTime */
			"time",
			"value"
		},
		{/* AllPalFX */
			"time",
			"add",
			"mul",
			"sinadd",
			"invertall",
			"color"
		},
		{/* AngleAdd */
			"value"
		},
		{/* AngleDraw */
			"value",
			"scale"
		},
		{/* AngleMul */
			"value"
		},
		{/* AngleSet */
			"value"
		},
		{/* AppendToClipboard */
			"text",
			"params"
		},
		{/* AssertSpecial */
			"flag",
			"flag2",
			"flag3"
		},
		{/* AttackDist */
			"value"
		},
		{/* AttackMulSet */
			"value"
		},
		{/* BGPalFX */
			"time",
			"add",
			"mul",
			"sinadd",
			"invertall",
			"color"
		},
		{/* BindToParent */
			"time",
			"facing",
			"pos"
		},
		{/* BindToRoot */
			"time",
			"facing",
			"pos"
		},
		{/* BindToTarget */
			"time",
			"ID",
			"pos"
		},
		{/* ChangeAnim */
			"value",
			"elem"
		},
		{/* ChangeAnim2 */
			"value",
			"elem"
		},
		{/* ChangeState */
			"value",
			"ctrl",
			"anim"
		},
		{/* ClearClipboard */
			NULL
		},
		{/* CtrlSet */
			"value"
		},
		{/* DefenceMulSet */
			"value"
		},
		{/* DestroySelf */
			NULL
		},
		{/* DisplayToClipboard */
			"text",
			"params"
		},
		{/* EnvColor */
			"value",
			"time",
			"under"
		},
		{/* EnvShake */
			"time",
			"freq",
			"ampl",
			"phase"
		},
		{/* Explod */
			"anim",
			"ID",
			"pos",
			"postype",
			"facing",
			"vfacing",
			"bindtime",
			"vel",
			"velocity",
			"accel",
			"random",
			"removetime",
			"supermove",
			"supermovetime",
			"pausemovetime",
			"scale",
			"sprpriority",
			"ontop",
			"shadow",
			"ownpal",
			"removeongethit",
			/* "ignorehitpause", */
			"trans"
		},
		{/* ExplodBindTime */
			"ID",
			"time",
			"value"
		},
		{/* ForceFeedback */
			"waveform",
			"time",
			"freq",
			"ampl",
			"self"
		},
		{/* FallEnvShake */
			NULL
		},
		{/* GameMakeAnim */
			"value",
			"under",
			"pos",
			"random"
		},
		{/* Gravity */
			NULL
		},
		{/* Helper */
			"helpertype",
			"name",
			"ID",
			"pos",
			"postype",
			"facing",
			"stateno",
			"keyctrl",
			"ownpal",
			"supermovetime",
			"pausemovetime",
			"size.xscale",
			"size.yscale",
			"size.ground.back",
			"size.ground.front",
			"size.height",
			"size.proj.doscale",
			"size.head.pos",
			"size.mid.pos",
			"size.shadowoffset"
		},
		{/* HitAdd */
			"value"
		},
		{/* HitBy */
			"value",
			"value2",
			"time"
		},
		{/* HitDef */
			"attr",
			"hitflag",
			"guardflag",
			"affectteam",
			"animtype",
			"air.animtype",
			"fall.animtype",
			"priority",
			"damage",
			"pausetime",
			"guard.pausetime",
			"sparkno",
			"guard.sparkno",
			"sparkxy",
			"hitsound",
			"guardsound",
			"ground.type",
			"air.type",
			"ground.slidetime",
			"guard.slidetime",
			"ground.hittime",
			"guard.hittime",
			"air.hittime",
			"guard.ctrltime",
			"guard.dist",
			"yaccel",
			"ground.velocity",
			"guard.velocity",
			"air.velocity",
			"airguard.velocity",
			"ground.cornerpush.veloff",
			"down.cornerpush.veloff",
			"guard.cornerpush.veloff",
			"airguard.cornerpush.veloff",
			"airguard.ctrltime",
			"air.juggle",
			"mindist",
			"maxdist",
			"snap",
			"p1sprpriority",
			"p2sprpriority",
			"p1facing",
			"p1getp2facing",
			"p2facing",
			"p1stateno",
			"p2stateno",
			"p2getp1state",
			"forcestand",
			"fall",
			"fall.xvelocity",
			"fall.yvelocity",
			"fall.recover",
			"fall.recovertime",
			"fall.damage",
			"air.fall",
			"down.velocity",
			"down.hittime",
			"down.bounce",
			"id",
			"chainID",
			"nochainID",
			"hitonce",
			"kill",
			"guard.kill",
			"fall.kill",
			"numhits",
			"getpower",
			"givepower",
			"palfx.time",
			"palfx.mul",
			"palfx.add",
			"palfx.sinadd",
			"palfx.invertall",
			"palfx.color",
			"envshake.time",
			"envshake.freq",
			"envshake.ampl",
			"envshake.phase",
			"fall.envshake.time",
			"fall.envshake.freq",
			"fall.envshake.ampl",
			"fall.envshake.phase",
			"attack.width"
		},
		{/* HitFallDamage */
			NULL
		},
		{/* HitFallSet */
			"value",
			"xvel",
			"yvel"
		},
		{/* HitFallVel */
			NULL
		},
		{/* HitOverride */
			"attr",
			"slot",
			"stateno",
			"time",
			"forceair"
		},
		{/* HitVelSet */
			"x",
			"y"
		},
		{/* LifeAdd */
			"value",
			"kill",
			"absolute"
		},
		{/* LifeSet */
			"value"
		},
		{/* MakeDust */
			"pos",
			"pos2",
			"spacing"
		},
		{/* ModifyExplod */
			"anim",
			"ID",
			"pos",
			"postype",
			"facing",
			"vfacing",
			"bindtime",
			"vel",
			"accel",
			"random",
			"removetime",
			"supermove",
			"supermovetime",
			"pausemovetime",
			"scale",
			"sprpriority",
			"ontop",
			"shadow",
			"ownpal",
			"removeongethit",
			/* "ignorehitpause", */
			"trans"
		},
		{/* MoveHitReset */
			NULL
		},
		{/* NotHitBy */
			"value",
			"value2",
			"time"
		},
		{/* Null */
			NULL
		},
		{/* Offset */
			"x",
			"y"
		},
		{/* PalFX */
			"time",
			"add",
			"mul",
			"sinadd",
			"invertall",
			"color"
		},
		{/* ParentVarAdd */
			"v",
			"fv",
			"value",
			"var(?",
			"fvar(?",
			"sysvar(?",
			"sysfvar(?"
		},
		{/* ParentVarSet */
			"v",
			"fv",
			"value",
			"var(?",
			"fvar(?",
			"sysvar(?",
			"sysfvar(?"
		},
		{/* Pause */
			"time",
			"endcmdbuftime",
			"movetime",
			"pausebg"
		},
		{/* PlayerPush */
			"value"
		},
		{/* PlaySnd */
			"value",
			"volume",
			"channel",
			"lowpriority",
			"freqmul",
			"loop",
			"pan",
			"abspan"
		},
		{/* PosAdd */
			"x",
			"y"
		},
		{/* PosFreeze */
			"value"
		},
		{/* PosSet */
			"x",
			"y"
		},
		{/* PowerAdd */
			"value"
		},
		{/* PowerSet */
			"value"
		},
		{/* Projectile */
			"ProjID",
			"projanim",
			"projhitanim",
			"projremanim",
			"projcancelanim",
			"projscale",
			"projremove",
			"projremovetime",
			"velocity",
			"remvelocity",
			"accel",
			"velmul",
			"projhits",
			"projmisstime",
			"projpriority",
			"projsprpriority",
			"projedgebound",
			"projstagebound",
			"projheightbound",
			"offset",
			"postype",
			"projshadow",
			"supermovetime",
			"pausemovetime",
			"afterimage.time",
			"afterimage.length",
			"afterimage.palcolor",
			"afterimage.palinvertall",
			"afterimage.palbright",
			"afterimage.palcontrast",
			"afterimage.palpostbright",
			"afterimage.paladd",
			"afterimage.palmul",
			"afterimage.timegap",
			"afterimage.framegap",
			"afterimage.trans",
			"attr",
			"hitflag",
			"guardflag",
			"affectteam",
			"animtype",
			"air.animtype",
			"fall.animtype",
			"priority",
			"damage",
			"pausetime",
			"guard.pausetime",
			"sparkno",
			"guard.sparkno",
			"sparkxy",
			"hitsound",
			"guardsound",
			"ground.type",
			"air.type",
			"ground.slidetime",
			"guard.slidetime",
			"ground.hittime",
			"guard.hittime",
			"air.hittime",
			"guard.ctrltime",
			"guard.dist",
			"yaccel",
			"ground.velocity",
			"guard.velocity",
			"air.velocity",
			"airguard.velocity",
			"ground.cornerpush.veloff",
			"down.cornerpush.veloff",
			"guard.cornerpush.veloff",
			"airguard.cornerpush.veloff",
			"airguard.ctrltime",
			"air.juggle",
			"mindist",
			"maxdist",
			"snap",
			"p1sprpriority",
			"p2sprpriority",
			"p1facing",
			"p1getp2facing",
			"p2facing",
			"p1stateno",
			"p2stateno",
			"p2getp1state",
			"forcestand",
			"fall",
			"fall.xvelocity",
			"fall.yvelocity",
			"fall.recover",
			"fall.recovertime",
			"fall.damage",
			"air.fall",
			"down.velocity",
			"down.hittime",
			"down.bounce",
			"id",
			"chainID",
			"nochainID",
			"hitonce",
			"kill",
			"guard.kill",
			"fall.kill",
			"numhits",
			"getpower",
			"givepower",
			"palfx.time",
			"palfx.mul",
			"palfx.add",
			"palfx.sinadd",
			"palfx.invertall",
			"palfx.color",
			"envshake.time",
			"envshake.freq",
			"envshake.ampl",
			"envshake.phase",
			"fall.envshake.time",
			"fall.envshake.freq",
			"fall.envshake.ampl",
			"fall.envshake.phase",
			"attack.width"
		},
		{/* RemoveExplod */
			"ID"
		},
		{/* ReversalDef */
			"reversal.attr",
			"pausetime",
			"sparkno",
			"hitsound",
			"p1stateno",
			"p2stateno"
		},
		{/* ScreenBound */
			"value",
			"movecamera"
		},
		{/* SelfState */
			"value",
			"ctrl",
			"anim"
		},
		{/* SprPriority */
			"value"
		},
		{/* StateTypeSet */
			"statetype",
			"value",
			"movetype",
			"physics"
		},
		{/* SndPan */
			"channel",
			"pan",
			"abspan"
		},
		{/* StopSnd */
			"channel"
		},
		{/* SuperPause */
			"time",
			"movetime",
			"anim",
			"sound",
			"pos",
			"darken",
			"p2defmul",
			"poweradd",
			"unhittable"
		},
		{/* TargetBind */
			"time",
			"ID",
			"pos"
		},
		{/* TargetDrop */
			"excludeID",
			"keepone"
		},
		{/* TargetFacing */
			"value",
			"ID"
		},
		{/* TargetLifeAdd */
			"value",
			"ID",
			"kill",
			"absolute"
		},
		{/* TargetPowerAdd */
			"value",
			"ID"
		},
		{/* TargetState */
			"value",
			"ID"
		},
		{/* TargetVelAdd */
			"x",
			"y",
			"ID"
		},
		{/* TargetVelSet */
			"x",
			"y",
			"ID"
		},
		{/* Trans */
			"trans",
			"alpha"
		},
		{/* Turn */
			NULL
		},
		{/* VarAdd */
			"v",
			"fv",
			"value",
			"var(?",
			"fvar(?",
			"sysvar(?",
			"sysfvar(?"
		},
		{/* VarRandom */
			"v",
			"range"
		},
		{/* VarRangeSet */
			"value",
			"fvalue",
			"first",
			"last"
		},
		{/* VarSet */
			"v",
			"fv",
			"value",
			"var(?",
			"fvar(?",
			"sysvar(?",
			"sysfvar(?"
		},
		{/* VelAdd */
			"x",
			"y"
		},
		{/* VelMul */
			"x",
			"y"
		},
		{/* VelSet */
			"x",
			"y"
		},
		{/* Width */
			"edge",
			"player",
			"value"
		}
	};
	char flag, *str;
	int cnt, param_cnt = param == NULL ? 0 : _msize(param) / sizeof(PARAM), i, j, k;

	/* 順番通りに出力する */
	switch (state) {
	case CHR_INFO:
		i = 0;
		str = "Info";
		goto chr;
	case CHR_FILES:
		i = 1;
		str = "Files";
		goto chr;
	case CHR_ARCADE:
		i = 2;
		str = "Arcade";
chr:
		fprintf(f, "\n[%s]\n", str);
		if (param_cnt == 0) return;

		for (j = 0; chr[i][j] != NULL; j++)
			for (k = 0; k < param_cnt; k++) {
				if (stricmp(chr[i][j], param[k].name)) continue;

				fprintf(f, "%s =", chr[i][j]);
				if (param[k].value[0] != '\0') fprintf(f, " %s", param[k].value);
				fprintf(f, "\n");
				param[k].name[0] = '\1';
			}

		break;
	case CHR_DATA:
		i = 0;
		str = "Data";
		goto cns;
	case CHR_SIZE:
		i = 1;
		str = "Size";
		goto cns;
	case CHR_VELOCITY:
		i = 2;
		str = "Velocity";
		goto cns;
	case CHR_MOVEMENT:
		i = 3;
		str = "Movement";
		goto cns;
	case CHR_COMMAND:
		i = 4;
		str = "Command";
		goto cns;
	case CHR_REMAP:
		i = 5;
		str = "Remap";
		goto cns;
	case CHR_DEFAULTS:
		i = 6;
		str = "Defaults";
cns:
		fprintf(f, "\n[%s]\n", str);
		if (param_cnt == 0) return;

		for (j = 0; cns[i][j] != NULL; j++)
			for (k = 0; k < param_cnt; k++) {
				if (stricmp(cns[i][j], param[k].name)) continue;

				fprintf(f, "%s =", cns[i][j]);
				if (param[k].value[0] != '\0') fprintf(f, " %s", prepareValue(param[k].value, 1));
				fprintf(f, "\n");
				param[k].name[0] = '\1';
			}

		break;
	case CHR_STATEDEF:
		fprintf(f, "\n[Statedef %s]\n", stateno);
		if (param_cnt == 0) return;

		cnt = sizeof(sctrl_param) / sizeof(sctrl_param[0]);
		for (i = 0; i < cnt; i++)
			for (j = 0; j < param_cnt; j++) {
				if (stricmp(sctrl_param[i], param[j].name)) continue;

				fprintf(f, "%s =", sctrl_param[i]);
				if (param[j].value[0] != '\0') fprintf(f, " %s", prepareValue(param[j].value, 0));
				fprintf(f, "\n");
				param[j].name[0] = '\1';
			}

		break;
	default:	/* case CHR_STATE: */
		fprintf(f, "\n[State ]\n");
		if (param_cnt == 0) return;

		for (j = 0; j < param_cnt; j++)
			if (!stricmp(param[j].name, "type")) break;
		if (j < param_cnt) {	/* 見つかった？ */
			cnt = sizeof(sctrls) / sizeof(sctrls[0]);
			for (i = 0; i < cnt; i++)
				if (!stricmp(sctrls[i], param[j].value)) break;
			if (i < cnt) {	/* 見つかった？ */
				param[j].name[0] = '\1';
				fprintf(f, "type = %s\n", sctrls[i]);

				if (is_command && !stricmp(sctrls[i], "ChangeState"))
					flag = 1;
				else {
					flag = 0;
					printPreparedCommParams(param, param_cnt, "trigger", 7, f);
				}

				for (j = 0; params[i][j] != NULL; j++) {
					cnt = (str = strchr(params[i][j], '?')) == NULL ? 0 : str - params[i][j];

					for (k = 0; k < param_cnt; k++) {
						if (cnt == 0 && stricmp(params[i][j], param[k].name) ||
						cnt > 0 && _strnicmp(params[i][j], param[k].name, cnt)) continue;

						fprintf(f, "%s =", cnt > 0 ? str2Lower(param[k].name) : params[i][j]);
						if (param[k].value[0] != '\0') fprintf(f, " %s", prepareValue(param[k].value, 0));
						fprintf(f, "\n");
						param[k].name[0] = '\1';
					}
				}

				if (flag) printPreparedCommParams(param, param_cnt, "trigger", 7, f);
				printPreparedCommParams(param, param_cnt, "persistent", 0, f);
				printPreparedCommParams(param, param_cnt, "ignorehitpause", 0, f);
			}
		}
	}

	/* 前処理で出力されなかったのを出力する */
	for (i = 0; i < param_cnt; i++)
		if (param[i].name[0] == '\0') {
			if (param[i].value[0] != ';') fprintf(f, ";");
			fprintf(f, "%s\n", param[i].value);
		} else if (param[i].name[0] != '\1')
			fprintf(f, ";;%s = %s\n", param[i].name, param[i].value);
}

/* 並べ替えて出力 */
void printPreparedAir(AIR *air, FILE *f)
{
	char buf[10], k;
	int index, cnt, i, j;
	AIR tmp;

	if (air == NULL) return;
	index = _msize(air) / sizeof(AIR);
	cnt = 0;

	for (i = 0; i < index; i++) {
		for (j = i + 1; j < index; j++) {
			if (air[i].no <= air[j].no) continue;
			tmp = air[i];
			air[i] = air[j];
			air[j] = tmp;
		}

		fprintf(f, "\n[Begin Action %d]\n", air[i].no);
		for (j = 0; air[i].str[j][0] != '\0'; j++) {
			if (strchr(air[i].str[j], '=') != NULL) fprintf(f, " ");
			fprintf(f, "%s\n", prepareValue(air[i].str[j], 0));
		}

		/* カウント表示 */
		if (cnt > 0) {
			j = strlen(_ltoa(cnt, buf, 10));
			for (k = 0; k < j; k++) printf("\b");
		} else
			printf(" ");
		printf("%d", ++cnt);
	}
}

/* SFFのPCXデータをクロップする。*/
char crop_sff(FILE *rf, FILE *wf)
{
	unsigned char buf[10], c1, c2, flag = 0, *data1 = NULL, *data2 = NULL, *tmp;
	short w1, h1, w2, h2;
	int offset, next, size1, size2, i, j, k, l;
	SFF_HEADER sh;
	SFF_SUBFILE_HEADER ssh;
	PCX_HEADER ph;
	RGB_PALETTE palette[256];
	CROP *crop = NULL, *pcrop;

	/* SFFヘッダを取得して始めのサブファイルヘッダへ移動。*/
	if (fread(&sh, sizeof(sh), 1, rf) < 1 || fseek(rf, sh.subfile_offset, SEEK_SET)) goto procend;

	/* SFFヘッダ書き込み。*/
	sh.subfile_offset = sizeof(sh);
	sh.size_of_subheader = sizeof(ssh);
	memset(sh.blank, 0, sizeof(sh.blank));
	memset(sh.comment, 0, sizeof(sh.comment));
	if (fwrite(&sh, sizeof(sh), 1, wf) < 1) goto procend;

	for (offset = sizeof(sh), size1 = i = 0; i < sh.number_of_images; ) {
		/* クロップ情報領域確保。*/
		if ((pcrop = realloc(crop, sizeof(CROP) + size1)) == NULL) goto procend;
		crop = pcrop;
		pcrop = &crop[size1 / sizeof(CROP)];
		size1 += sizeof(CROP);

		/* SFFサブファイルヘッダ取得。*/
		if (fread(&ssh, sizeof(ssh), 1, rf) < 1) goto procend;
		next = ssh.next_subfile_offset;

		if (ssh.subfile_length == 0) {	/* 実データがない？ */
			/* 次のサブファイルのオフセット計算。*/
			offset += sizeof(ssh);
			ssh.next_subfile_offset = offset;

			/* クロップ情報設定。*/
			pcrop->left = crop[ssh.prev_cpy_spr_idx].left;
			pcrop->top = crop[ssh.prev_cpy_spr_idx].top;

			/* クロップした分だけずらす。*/
			ssh.x -= pcrop->left;
			ssh.y -= pcrop->top;

			/* サブファイルヘッダ書き込み。*/
			if (fwrite(&ssh, sizeof(ssh), 1, wf) < 1) goto procend;
		} else {	/* 実データがある。*/
			if (!ssh.is_same_prev_pal) {
				/* PCXパレット取得。*/
				if (fseek(rf, ssh.subfile_length - 768, SEEK_CUR) ||
				fread(palette, 768, 1, rf) < 1 ||
				fseek(rf, -ssh.subfile_length, SEEK_CUR)) goto procend;
			}

			/* PCXヘッダ取得。*/
			if (fread(&ph, sizeof(ph), 1, rf) < 1) goto procend;

			/* PCXデータサイズ計算。*/
			w1 = ph.window.x_max - ph.window.x_min + 1;
			h1 = ph.window.y_max - ph.window.y_min + 1;

			/* PCXデータ領域確保。*/
			if ((tmp = realloc(data1, w1 * h1)) == NULL) goto procend;
			data1 = tmp;

			/* PCXデータのRLEをデコード。*/
			for (j = 0; j < h1; j++) {
				k = 0;
				while (k < w1) {
					if (fread(&c1, 1, 1, rf) < 1) goto procend;
					if (c1 > 192) {
						l = c1 - 192;
						if (fread(&c1, 1, 1, rf) < 1) goto procend;
						k += l;
						for (; l > 0; l--) *tmp++ = c1;
					} else {
						*tmp++ = c1;
						k++;
					}
				}
			}

			size2 = 0;
			for (pcrop->top = 0; pcrop->top < h1; pcrop->top++) {	/* 上クロップ位置取得。*/
				for (k = 0; k < w1 && data1[pcrop->top * w1 + k] == 0; k++);
				if (k < w1) break;
			}
			if (pcrop->top < h1) {
				for (pcrop->left = 0; pcrop->left < w1; pcrop->left++) {	/* 左クロップ位置取得。*/
					for (k = 0; k < h1 && data1[k * w1 + pcrop->left] == 0; k++);
					if (k < h1) break;
				}
				for (j = w1 - 1; j >= 0; j--) {	/* 右クロップ位置取得。*/
					for (k = 0; k < h1 && data1[k * w1 + j] == 0; k++);
					if (k < h1) break;
				}
				for (k = h1 - 1; k >= 0; k--) {	/* 下クロップ位置取得。*/
					for (l = 0; l < w1 && data1[k * w1 + l] == 0; l++);
					if (l < w1) break;
				}

				/* クロップしたときのPCXデータサイズ計算。*/
				w2 = j - pcrop->left + 1;
				h2 = k - pcrop->top + 1;

				/* クロップしたときのPCXデータ領域確保。*/
				if ((tmp = realloc(data2, w2 * h2)) == NULL) goto procend;
				data2 = tmp;

				/* クロップ処理。*/
				for (j = 0, l = pcrop->top; l <= k; j++, l++)
					memcpy(&data2[j * w2], &data1[l * w1 + pcrop->left], w2);

				/* クロップ後のPCXサイズ計算。*/
				j = w1 - w2;
				k = h1 - h2;
				ph.window.x_max -= j;
				ph.window.y_max -= k;
				ph.h_dpi -= j;
				ph.v_dpi -= k;
				ph.bytes_per_line -= j;

				/* PCXデータをRLE処理。*/
				for (j = 0; j < h2; j++) {
					k = j * w2;
					l = 0;
					while (l < w2) {
						c1 = data2[k + l];
						l++;
						c2 = 1;
						while (l < w2 && c1 == data2[k + l] && c2 < 63) {
							l++;
							c2++;
						}
						if (c2 > 1 || c1 >= 192) {
							c2 += 192;
							if ((tmp = realloc(data1, size2 + 2)) == NULL) goto procend;
							data1 = tmp;
							tmp = &data1[size2];
							size2 += 2;
							tmp[0] = c2;
							tmp[1] = c1;
						} else {
							if ((tmp = realloc(data1, size2 + 1)) == NULL) goto procend;
							data1 = tmp;
							data1[size2] = c1;
							size2++;
						}
					}
				}

				/* クロップした分だけずらす。*/
				ssh.x -= pcrop->left;
				ssh.y -= pcrop->top;
			} else	/* すべてクロップできる場合。*/
				ph.window.x_max = ph.window.y_max = ph.h_dpi = ph.v_dpi = ph.bytes_per_line = 0;

			/* PCXデータの最後の処理。*/
			if ((tmp = realloc(data1, size2 + 1)) == NULL) goto procend;
			data1 = tmp;
			data1[size2] = 12;
			size2++;

			/* サブファイルサイズ計算。*/
			ssh.subfile_length = sizeof(ph) + size2;
			if (!ssh.is_same_prev_pal) ssh.subfile_length += 768;

			/* 次のサブファイルのオフセット計算。*/
			if (i < sh.number_of_images - 1) {
				offset += sizeof(ssh) + ssh.subfile_length;
				ssh.next_subfile_offset = offset;
			} else
				ssh.next_subfile_offset = 0;

			/* サブファイル書き込み。*/
			if (fwrite(&ssh, sizeof(ssh), 1, wf) < 1 || fwrite(&ph, sizeof(ph), 1, wf) < 1 ||
			fwrite(data1, size2, 1, wf) < 1) goto procend;
			if (!ssh.is_same_prev_pal && fwrite(palette, 768, 1, wf) < 1) goto procend;
		}

		if (next == 0) break;
		if (fseek(rf, next, SEEK_SET)) goto procend;

		/* カウント表示。*/
		if (i > 0)
			for (j = strlen(_ltoa(i, buf, 10)); j > 0; j--) printf("\b");
		printf("%d", ++i);
	}

	flag = 1;

procend:
	free(data1);
	free(data2);
	free(crop);
	return flag;
}

/* 並べ替えてsndを出力
SND file structure ver1.01
HEADER
 0- 11[ 12] "ElecbyteSnd\0" signature
12- 15[  4] 4 verhi, 4 verlo
16- 19[  4] Number of sounds
20- 23[  4] File offset where first subfile is located.
24-511[488] Blank; can be used for comments.
SUBFILEHEADER
 0- 3[4] File offset where next subfile in the linked list is located.
          Null if last subfile.
 4- 7[4] Subfile length (not including header.)
 8-11[4] Group number
12-15[4] Sample number
16-      Sound data (WAV)
*/
char printPreparedSnd(FILE *rf, FILE *wf)
{
	char buf[10];
	int cnt, loc, len, i, j;
	SND snd[MAX_SND], tmp;

	if (fseek(rf, 20, SEEK_SET) || fread(&loc, 4, 1, rf) < 1) return 0;

	for (cnt = 0; cnt < MAX_SND && loc > 0 && !fseek(rf, loc, SEEK_SET) && fread(&loc, 4, 1, rf) == 1;) {
		if (fread(&len, 4, 1, rf) < 1 || fread(&snd[cnt].group, 4, 1, rf) < 1 ||
		fread(&snd[cnt].sample, 4, 1, rf) < 1 ||
		(snd[cnt].data = (char *)malloc(len * sizeof(char))) == NULL ||
		fread(snd[cnt].data, 1, len, rf) < len) return 0;

		i = cnt;
		for (j = i - 1; j >= 0 && snd[i].group < snd[j].group; j--) {
			tmp = snd[i];
			snd[i] = snd[j];
			snd[j] = tmp;
			i = j;
		}
		for (j = i - 1; j >= 0 && snd[i].group == snd[j].group && snd[i].sample < snd[j].sample; j--) {
			tmp = snd[i];
			snd[i] = snd[j];
			snd[j] = tmp;
			i = j;
		}

		/* カウント表示 */
		if (cnt > 0) {
			len = strlen(_ltoa(cnt, buf, 10));
			for (i = 0; i < len; i++) printf("\b");
		}
		printf("%d", ++cnt);
	}

	if (cnt == MAX_SND && fread(&i, 1, 1, rf) == 1) return 0;	/* 上限に達してもまだデータがある場合は出力不可とする */

	if (fwrite("ElecbyteSnd\0", 1, 12, wf) < 12) return 0;
	i = 4;
	if (fwrite(&i, 4, 1, wf) < 1 || fwrite(&cnt, 4, 1, wf) < 1) return 0;
	i = 512;
	if (fwrite(&i, 4, 1, wf) < 1) return 0;
	for (i = 0; i < 488; i++) fputc('\0', wf);
	loc = 1;
	for (i = 512, j = 0; j < cnt; j++) {
		if (loc) {
			len = _msize(snd[j].data);
			i = (j < cnt - 1) ? i + len + 16 : 0;
			if (fwrite(&i, 4, 1, wf) < 1 || fwrite(&len, 4, 1, wf) < 1 ||
			fwrite(&snd[j].group, 4, 1, wf) < 1 || fwrite(&snd[j].sample, 4, 1, wf) < 1 ||
			fwrite(snd[j].data, 1, len, wf) < len) loc = 0;
		}
		free(snd[j].data);
	}

	return loc;
}

void main(int argc, char **argv)
{
	char buf[MAX_LINE], str[MAX_LINE], state, prevstate, stateno[10], statestr[10], *sp;
	char f[256], is_err, j, k, *fp;
	int cnt, i;
	FILE *f1, *f2;
	PARAM *param;
	AIR *air;

	if (argc < 2) return;

	param = NULL;
	air = NULL;

	for (i = 1; i < argc; i++) {
		/* カレントディレクトリを設定する */
		if ((fp = strrchr(argv[i], '\\')) == NULL) {
			fp = argv[i];
		} else {
			*fp = '\0';
			if (chdir(argv[i])) {
				printf("%s > Error\n", argv[i]);
				continue;
			}
			fp++;
		}
		/* ファイルを開く */
		printf("%s > ", fp);
		if ((f1 = fopen(fp, "rb")) == NULL) {
			printf("Read error\n");
			continue;
		}
		strcpy(f, fp);
		strcat(f, ".tmp");
		if ((f2 = fopen(f, "wb")) == NULL) {
			fclose(f1);
			printf("Write error\n");
			continue;
		}
		if (fread(buf, 1, 12, f1) == 12)
			if (!memcmp(buf, "ElecbyteSpr\0", 12)) {
				if (fseek(f1, 0, SEEK_SET) || !crop_sff(f1, f2)) {
					fclose(f1);
					fclose(f2);
					remove(f);
					printf(" Sff error\n");
					continue;
				}
			} else if (!memcmp(buf, "ElecbyteSnd\0", 12)) {
				if (!printPreparedSnd(f1, f2)) {
					fclose(f1);
					fclose(f2);
					remove(f);
					printf(" Snd error\n");
					continue;
				}
			} else
				goto otherproc;
		else {
otherproc:
			fclose(f1);
			fclose(f2);
			if ((f1 = fopen(fp, "r")) == NULL) {
				printf("Read error\n");
				continue;
			}
			if ((f2 = fopen(f, "w")) == NULL) {
				fclose(f1);
				printf("Write error\n");
				continue;
			}

			is_command = is_air = cnt = is_err = 0;
			state = prevstate = CHR_NONE;

			while (fgets(buf, sizeof(buf) / sizeof(buf[0]), f1) != NULL) {
				strcpy(str, buf);
				sp = strTrim(str);
				if (*sp == '\0') continue;

				prevstate = state;
				setState(&state, statestr, sp);
				if (state == CHR_NONE)
					fprintf(f2, "%s\n", sp);
				else if (statestr[0] == '\0') {
					if (is_air == 0 && (param = addParam(param, sp)) == NULL ||
					is_air == 1 && (air = addAir(air, stateno, sp)) == NULL) {
						is_err = 1;
						remove(f);
						printf(" Memory error\n");
						break;
					}
				} else {
					if (is_air == 0 && prevstate != CHR_NONE) {
						printPreparedParams(param, prevstate, stateno, f2);
						free(param);
						param = NULL;
					}

					memcpy(stateno, statestr, 10);

					/* カウント表示 */
					if (cnt > 0) {
						j = strlen(_ltoa(cnt, buf, 10));
						for (k = 0; k < j; k++) printf("\b");
					}
					printf("%d", ++cnt);
				}
			}

			/* 最後のデータを処理 */
			if (!is_err)
				if (is_air == 0) {
					if (prevstate != CHR_NONE) {
						printPreparedParams(param, prevstate, stateno, f2);
						free(param);
						param = NULL;
					}
				} else {
					printPreparedAir(air, f2);
					free(air);
					air = NULL;
				}
		}
		fclose(f1);
		fclose(f2);

		strcpy(str, fp);
		strcat(str, ".bak");
		rename(fp, str);
		rename(f, fp);

		if (!is_err) printf(" OK\n");
	}
}
