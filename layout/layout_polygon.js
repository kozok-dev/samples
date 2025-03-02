// ���C�A�E�g���p�`
var LayoutPolygon = Class.create(LayoutObject, {
	POINT_MAX: 20	// �_�̍ő吔

	, initialize: function ($super, point, option) {
		$super(point, option);
		if (!("inner" in this.option)) this.option["inner"] = 0;
	}

	, isValidPoint: function (x, y) {
		var i;

		if (this.point.length >= 4 && this.point[0].x == this.point[this.point.length - 1].x
			&& this.point[0].y == this.point[this.point.length - 1].y) {

			return false;	// �������Ă���̂œ_�͒ǉ��ł��Ȃ�
		}
		if (this.point.length >= 1 && this.point.length <= 2 && this.point[0].x == x && this.point[0].y == y) {
			return false;	// �_��3�ȏ�K�v�Ȃ̂Ɍ��ڂ��Ƃ����̂œ_�͒ǉ��ł��Ȃ�
		}

		if (!LayoutGrid.isValidPoint(x, y)) return false;

		// �J�n�_�̏����A�_�̏d���͋����Ȃ�
		for (i = 1; i < this.point.length; i++) {
			if (this.point[i].x == x && this.point[i].y == y) return false;
		}

		if (this.point.length >= this.POINT_MAX && (this.point[0].x != x || this.point[0].y != y)) {
			// �_�̍ő吔�ɒB���Ă���̂ɐV���ɓ_��ǉ����悤�Ƃ���
			alert("����ȏ�ǉ��ł��܂���B");
			return false;
		}

		return true;
	}

	, setPoint: function (x, y) {
		this.point.push({x: x, y: y});

		var comp_flag = false, i;

		if (this.point.length >= 2) {
			this.context.lineWidth = this.option["line"];
			this.context.strokeStyle = "#000";
			this.context.beginPath();
			this.context.moveTo(LayoutGrid.coord2Pos(this.point[this.point.length - 2].x), LayoutGrid.coord2Pos(this.point[this.point.length - 2].y));
			this.context.lineTo(LayoutGrid.coord2Pos(x), LayoutGrid.coord2Pos(y));
			this.context.stroke();
		}

		if (this.point.length >= 4 && this.point[0].x == x && this.point[0].y == y) {	// ���ԁH
			comp_flag = true;

			this.context.fillStyle = this.option["fill"];
			this.context.beginPath();
			this.context.moveTo(LayoutGrid.coord2Pos(x), LayoutGrid.coord2Pos(y));
			for (i = 1; i < this.point.length; i++) {
				this.context.lineTo(LayoutGrid.coord2Pos(this.point[i].x), LayoutGrid.coord2Pos(this.point[i].y));
			}
			this.context.closePath();
			this.context.fill();
		}

		// �_��`��
		if (!comp_flag && this.option["point"]) {
			this.context.lineWidth = 1;
			this.context.strokeStyle = this.context.fillStyle = this.point.length <= 1 ? "#f00" : "#000";
			this.context.beginPath();
			this.context.arc(LayoutGrid.coord2Pos(x), LayoutGrid.coord2Pos(y), 3, 0, Math.PI * 2, false);
			this.context.closePath();
			this.context.fill();
			this.context.stroke();
		}

		// ����
		if (comp_flag) this.drawInnerLine();
	}

	// �ő啝�ƍő卂�����擾
	// �����Finit_flag true�ɂ���Ə����l�p�̓_��񂪑ΏۂƂȂ�
	// �Ԓl�F�T�C�Y���
	, getMaxSize: function (init_flag) {
		var point = init_flag ? this.point_init : this.point;
		var x_min = 0, x_max = 0, y_min = 0, y_max = 0, pos, i;

		for (i = 0; i < point.length; i++) {
			pos = point[i];
			if (i > 0) {
				if (x_min > pos.x) x_min = pos.x;
				if (x_max < pos.x) x_max = pos.x;
				if (y_min > pos.y) y_min = pos.y;
				if (y_max < pos.y) y_max = pos.y;
			} else {
				x_min = pos.x;
				x_max = pos.x;
				y_min = pos.y;
				y_max = pos.y;
			}
		}

		return {
			width: x_max - x_min
			, height: y_max - y_min
		};
	}

	// �����ɐ���`��
	// ���l�F�_�̐�>=4���I�_=�n�_�Ƃ��A�������W���A�����Ȃ����̂Ƃ���
	, drawInnerLine: function () {
		if (this.option["inner"] == 0) return;

		var point = [], i, j, a, b, al, bl;
		var rad, pos, pos0, pos1, pos_cmp0, pos_cmp1, cross_count;

		// ���p�`�̊e�����𑽊p�`�̓����Ɉړ�
		for (i = 0; i < this.point.length - 1; i++) {
			a = this.point[i];	// ���̓_
			b = this.point[i + 1];	// 1��̓_

			rad = this.getSegmentRadian(a, b);
			rad += Math.PI / 2;	// ���p
			rad %= 2 * Math.PI;

			// rad(Y��)�����C�Ƃ��A���C�̕�����-1�Ƃ��đ��p�`���O������s��(cross_count��0�Ȃ�O�A����ȊO�͓�)
			// (���C�ƒ�����̐����͊܂܂Ȃ�)
			cross_count = 0;
			pos = {x: a.x + (b.x - a.x) / 2, y: a.y + (b.y - a.y) / 2};	// ���O����̊�ʒu
			pos0 = this.point[0];
			pos_cmp0 = this.comparePoint(pos, pos0, rad);
			for (j = 1; j < this.point.length; j++) {
				pos1 = this.point[j];
				pos_cmp1 = this.comparePoint(pos, pos1, rad);

				if (pos_cmp0.x == -1 ^ pos_cmp1.x == -1) {	// ���C����������\���L��H(�_�ڐG�A�����ڐG���l��)
					if (pos_cmp0.y == -1 && pos_cmp1.y == 1 || pos_cmp0.y == 1 && pos_cmp1.y == -1) {	// �������邩�ǂ����͌v�Z����K�v�L��H
						al = this.getNormalYAxisPoint(pos, pos0, rad);
						bl = this.getNormalYAxisPoint(pos, pos1, rad);
						if (pos.y > al.y + (bl.y - al.y) * (pos.x - al.x) / (bl.x - al.x)) {	// ���C�̕����Ɍ�������H
							cross_count += pos_cmp0.x < pos_cmp1.x ? 1 : -1;
						}
					} else if (pos_cmp0.y == -1 || pos_cmp1.y == -1) {	// ���C�̕����Ɍ�������H
						cross_count += pos_cmp0.x < pos_cmp1.x ? 1 : -1;
					}
				}

				pos0 = pos1;
				pos_cmp0 = pos_cmp1;
			}

			j = this.option["inner"] * (cross_count == 0 ? -1 : 1);
			point.push({
				x1: a.x + j * Math.cos(rad)
				, y1: a.y + j * -Math.sin(rad)
				, x2: b.x + j * Math.cos(rad)
				, y2: b.y + j * -Math.sin(rad)
			});
		}

		// ���p�`�̓����Ɉړ�����������1�̑O�̒����̌�_��ݒ�
		for (i = 0; i < point.length; i++) {
			a = point[i];
			b = point[i > 0 ? i - 1 : point.length - 1];

			pos = this.getCrossSegment(a, b, true);
			if (pos == null) {
				// ���肦�Ȃ����O�̂���
				a.x = b.x2;
				a.y = b.y2;
			} else {
				a.x = pos.x;
				a.y = pos.y;
			}
		}

		// ���L���������Ղ��悤�ɔz��̊e�v�f�ɐ�����ݒ肷��
		for (i = 0; i < point.length; i++) {
			a = point[i];
			b = point[i > 0 ? i - 1 : point.length - 1];

			a.x1 = b.x2 = a.x;
			a.y1 = b.y2 = a.y;
			a.e = a.e1 = a.e2 = true;
		}

		// �k����̓_���Ó����ǂ����ݒ�
		for (i = 0; i < point.length; i++) {
			a = {
				x1: this.point[i].x
				, y1: this.point[i].y
				, x2: point[i].x
				, y2: point[i].y
			};

			for (j = 0; j < point.length; j++) {
				if (j == i) continue;

				b = {
					x1: this.point[j].x
					, y1: this.point[j].y
					, x2: point[j].x
					, y2: point[j].y
				};

				if (this.getCrossSegment(a, b) == null) {
					if (i > 0 && j == i - 1 || i <= 0 && j == point.length - 1) continue;
					if (i < point.length - 1 && j == i + 1 || i >= point.length - 1 && j == 0) continue;

					if (this.getCrossSegment(a, point[j]) == null) continue;
				}

				point[i].e = point[i].e1 = point[i > 0 ? i - 1 : point.length - 1].e2 = false;
				break;
			}
		}

		// �K�v�ȓ_��ݒ�
		var point_tmp = [];
		for (i = 0; i < point.length; i++) {
			a = point[i];
			b = point[i < point.length - 1 ? i + 1 : 0];

			for (j = 0; j < point.length; j++) {
				if (j == i) continue;
				if (i > 0 && j == i - 1 || i <= 0 && j == point.length - 1) continue;
				if (i < point.length - 1 && j == i + 1 || i >= point.length - 1 && j == 0) continue;

				pos0 = this.getCrossSegment(a, point[j]);
				if (pos0 != null) break;
			}

			if (j < point.length || a.e || b.e) {
				point_tmp.push(a);
			} else {
				// �s�v�ȓ_�̈�O�̓_�Ƀ}�[�N�����A���L�����Ō�_��ݒ�ł���悤�ɂ���
				point[i > 0 ? i - 1 : point.length - 1].d = true;
			}
		}
		point = eval(Object.toJSON(point_tmp));

		// �K�v�ȓ_�ōēx�A������1�̑O�̒����̌�_�Ɛ�����ݒ�
		for (i = 0; i < point.length; i++) {
			a = point[i];
			b = point[i > 0 ? i - 1 : point.length - 1];
			if (!b.d) continue;

			pos = this.getCrossSegment(a, b, true);
			if (pos == null) {
				// ���肦�Ȃ����O�̂���
				a.x = b.x2;
				a.y = b.y2;
			} else {
				a.x = pos.x;
				a.y = pos.y;
			}
		}
		for (i = 0; i < point.length; i++) {
			a = point[i];
			b = point[i > 0 ? i - 1 : point.length - 1];

			a.x1 = b.x2 = a.x;
			a.y1 = b.y2 = a.y;
		}

		// ���p�`�̓����Ɉړ����������̌�_���v�Z���A�`����s��
		var draw_flag = true;
		this.context.lineWidth = 1;
		this.context.strokeStyle = "#f00";
		this.context.beginPath();
		for (i = 0; i < point.length; i++) {
			a = point[i];

			pos = [];
			for (j = 0; j < point.length; j++) {
				// �Ώۂ��܂ޑO��̐����͏��O
				if (j == i) continue;
				if (i > 0 && j == i - 1 || i <= 0 && j == point.length - 1) continue;
				if (i < point.length - 1 && j == i + 1 || i >= point.length - 1 && j == 0) continue;

				b = point[j];

				pos0 = this.getCrossSegment(a, b);
				if (pos0 == null) continue;

				pos.push(pos0);

				// ��_�܂ł̋����̏����ɑ}���\�[�g�ŕ��בւ�
				for (al = pos.length - 1; al > 0; al--) {
					if (pos[al - 1].d <= pos[al].d) break;

					pos1 = {};
					for (bl in pos[al - 1]) pos1[bl] = pos[al - 1][bl];
					pos[al - 1] = {};
					for (bl in pos[al]) pos[al - 1][bl] = pos[al][bl];
					pos[al] = {};
					for (bl in pos1) pos[al][bl] = pos1[bl];
				}
			}

			pos0 = point[i];
			for (j = 0; j < pos.length; j++) {
				if (draw_flag) {
					this.context.moveTo(
						Math.round(LayoutGrid.coord2Pos(pos0.x))
						, Math.round(LayoutGrid.coord2Pos(pos0.y))
					);
					this.context.lineTo(
						Math.round(LayoutGrid.coord2Pos(pos[j].x))
						, Math.round(LayoutGrid.coord2Pos(pos[j].y))
					);
				}

				pos0 = pos[j];
				draw_flag = !draw_flag;
			}
			if (draw_flag) {
				this.context.moveTo(
					Math.round(LayoutGrid.coord2Pos(pos0.x))
					, Math.round(LayoutGrid.coord2Pos(pos0.y))
				);
				this.context.lineTo(
					Math.round(LayoutGrid.coord2Pos(point[i].x2))
					, Math.round(LayoutGrid.coord2Pos(point[i].y2))
				);
			}
		}

		this.context.stroke();
	}

	// �����̃��W�A����3���̕�����0�x�Ƃ��Ď擾
	// �����Fa ��ƂȂ���W
	//       b ���W
	// �Ԓl�F0�x�ȏ�360�x�����̃��W�A��
	, getSegmentRadian: function (a, b) {
		var ab = Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
		var ac = Math.sqrt(Math.pow(a.x - (a.x + 1), 2));
		var rad = Math.acos(((a.x - b.x) * (a.x - (a.x + 1))) / (ab * ac));	// 3���̕�����0�x�Ƃ����ꍇ�̐����̊p�x
		if (a.y < b.y) rad = 2 * Math.PI - rad;
		return rad;
	}

	// �w�肳�ꂽ���W�A����Y���ɂȂ��Ă�����W����ƂȂ���W�𒆐S�ɒʏ��Y��(0���̕���)�̍��W��
	// �ϊ������ꍇ�̍��W��Ԃ�
	// �����Fa ��ƂȂ���W
	//     �Fb �Ώۂ̍��W
	//     �Frad ���W�A��
	// �Ԓl�F���W
	, getNormalYAxisPoint: function (a, b, rad) {
		rad = Math.PI / 2 - rad;	// 90�x(0���̕���)�܂ł̍�
		return {
			x: a.x + ((b.x - a.x) * Math.cos(rad) - (b.y - a.y) * -Math.sin(rad))
			, y: a.y + ((b.x - a.x) * -Math.sin(rad) + (b.y - a.y) * Math.cos(rad))
		};
	}

	// ��ƂȂ���W�Ǝw�肵�����W�Ƃ̈ʒu�֌W���p�x���l�����ĕԂ�
	// �����Fa ��ƂȂ���W
	//     �Fb �Ώۂ̍��W
	//     �Frad ��ƂȂ郉�W�A��(Y��)
	// �Ԓl�FX�AY�ł��ꂼ��A-1:���菬�����A0:�����A1:����傫��
	, comparePoint: function (a, b, rad) {
		var ret = {x: 0, y: 0}, rad2, rad3;
		var rad_ab = this.round(this.getSegmentRadian(a, b));

		// �v�Z���A�p�x���}�C�i�X�ɂȂ�����360�x�ȏ�ɂȂ�Ɩʓ|�Ȃ̂�rad�ɂ���Čv�Z���@��ς��Ă���

		// X (rad����180�x��菬�����Ȃ�-1�A�傫���Ȃ�1)
		if (rad >= Math.PI) {	// 180�x�ȏ�H
			rad2 = this.round(rad);
			rad3 = this.round(rad - Math.PI);
			if (rad_ab > rad2 || rad_ab < rad3) ret.x = -1;
			else if (rad_ab < rad2 && rad_ab > rad3) ret.x = 1;
		} else {
			rad2 = this.round(rad);
			rad3 = this.round(rad + Math.PI);
			if (rad_ab > rad2 && rad_ab < rad3) ret.x = -1;
			else if (rad_ab < rad2 || rad_ab > rad3) ret.x = 1;
		}

		// Y (rad�̑O��90�x��菬�����Ȃ�-1�A�傫���Ȃ�1)
		if (rad < Math.PI / 2) {	// 90�x�����H
			rad2 = this.round(rad + Math.PI / 2);
			rad3 = this.round(rad + Math.PI * 1.5);
			if (rad_ab < rad2 || rad_ab > rad3) ret.y = -1;
			else if (rad_ab > rad2 && rad_ab < rad3) ret.y = 1;
		} else if (rad >= Math.PI * 1.5) {	// 270�x�ȏ�H
			rad2 = this.round(rad - Math.PI / 2);
			rad3 = this.round(rad - Math.PI * 1.5);
			if (rad_ab > rad2 || rad_ab < rad3) ret.y = -1;
			else if (rad_ab < rad2 && rad_ab > rad3) ret.y = 1;
		} else {
			rad2 = this.round(rad + Math.PI / 2);
			rad3 = this.round(rad - Math.PI / 2);
			if (rad_ab < rad2 && rad_ab > rad3) ret.y = -1;
			else if (rad_ab > rad2 || rad_ab < rad3) ret.y = 1;
		}

		return ret;
	}

	// 2�����̌�_���擾����
	// �����Fa ��ƂȂ����
	//       b �Ώۂ̐���
	//       line_flag true�ɂ���Ɛ����ł͂Ȃ��A�����Ƃ��Čv�Z����
	// �Ԓl�F��_���B�������Ȃ��ꍇ��null
	, getCrossSegment: function (a, b, line_flag) {
		var p = (a.x2 - a.x1) * (b.y2 - b.y1) - (a.y2 - a.y1) * (b.x2 - b.x1);
		var r = (b.y2 - b.y1) * (b.x1 - a.x1) - (b.x2 - b.x1) * (b.y1 - a.y1);
		var s = (a.y2 - a.y1) * (b.x1 - a.x1) - (a.x2 - a.x1) * (b.y1 - a.y1);
		var x, y, flag;

		if (p == 0 && r == 0 && s == 0) {	// �������d�Ȃ荇���Ă���H
			if (!line_flag) {
				r = Math.sqrt(Math.pow(a.x1 - a.x2, 2) + Math.pow(a.y1 - a.y2, 2));
				flag = false;

				s = Math.sqrt(Math.pow(a.x1 - b.x1, 2) + Math.pow(a.y1 - b.y1, 2));
				if (r > s && (a.x2 - a.x1) * (b.x1 - a.x1) + (a.y2 - a.y1) * (b.y1 - a.y1) == r * s) {
					flag = true;	// ����a��b�̎n�_��a�̒[�_�ȊO�ŏd�Ȃ��Ă���
				}
				s = Math.sqrt(Math.pow(a.x1 - b.x2, 2) + Math.pow(a.y1 - b.y2, 2));
				if (r > s && (a.x2 - a.x1) * (b.x2 - a.x1) + (a.y2 - a.y1) * (b.y2 - a.y1) == r * s) {
					flag = true;	// ����a��b�̏I�_��a�̒[�_�ȊO�ŏd�Ȃ��Ă���
				}

				if (!flag) return null;
			}

			x = b.x2;
			y = b.y2;
			r = s = 0;
		} else {
			if (p == 0) return null;	// ���s�Ȃ̂Ō������Ȃ�

			r /= p;
			s /= p;
			if (!line_flag) {
				if (r < 0 || r > 1 || s < 0 || s > 1) return null;	// �������Ă��Ȃ�
			}

			x = a.x1 + r * (a.x2 - a.x1);
			y = a.y1 + r * (a.y2 - a.y1);
		}

		return {
			x: x	// ��_��X���W
			, y: y	// ��_��Y���W
			, d: Math.sqrt(Math.pow(a.x1 - x, 2) + Math.pow(a.y1 - y, 2))	// ��_�܂ł̋���
			, r: r	// ����a����ɂ�����_�̈ʒu�̔䗦
			, s: s	// ����b����ɂ�����_�̈ʒu�̔䗦
			, p: p	// 0�Ȃ畽�s
		};
	}

	// �����_�����̐��x�ɕϊ����ĕԂ�
	// �����Fvalue ���l
	// �Ԓl�F���l
	, round: function (value) {
		return Math.round(value * 10000000000) / 10000000000;
	}
});
