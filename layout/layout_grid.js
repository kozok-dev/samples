// ���C�A�E�g�O���b�h
var LayoutGrid = Class.create({
	GRID_NUM_X: 25	// X�O���b�h��
	, GRID_NUM_Y: 20	// Y�O���b�h��
	, GRID_SIZE: 20	// �O���b�hpx�T�C�Y
	, OFFSET: 5	// �O���b�h�̕\���J�npx�ʒu

	, canvas: null	// canvasDOM
	, context: null	// canvas�R���e�L�X�g
	, layout_object: null	// ���C�A�E�g�I�u�W�F�N�g
	, option: null	// �ݒ�
	, hidden: null	// �_�̍��WhiddenDOM
	, init_flag: null	// �������ς݂Ȃ�true

	// �R���X�g���N�^�[
	, initialize: function () {
		this.option = {edit: true};
		this.init_flag = false;
	}

	// ������
	// �����Fcanvas canvasDOM
	//       layout_object ���C�A�E�g�I�u�W�F�N�g�z��
	//       option �ݒ�
	, init: function (canvas, layout_object, option) {
		var i;

		this.canvas = canvas;
		this.context = this.canvas.getContext("2d");
		this.hidden = new Element("div");

		// ���C�A�E�g�I�u�W�F�N�g
		this.layout_object = [];
		for (i = 0; i < layout_object.length; i++) {
			this.layout_object[i] = layout_object[i];
			this.layout_object[i].setContext(this.context);
		}

		// �`��ݒ�
		for (i in option) this.option[i] = option[i];

		this.context.translate(0.5, 0.5);
		Element.setStyle(this.canvas, {cursor: "default"});
		Element.hide(this.hidden);
		Element.insert(Element.up(this.canvas), this.hidden);
		this.reset();
		if (this.option["edit"]) Event.observe(this.canvas, "click", this.setPoint.bind(this));

		this.init_flag = true;
	}

	// �������ς݂��ǂ���
	// �Ԓl�F�������ς݂Ȃ�true�A����ȊO��false
	, isInit: function () {
		return this.init_flag;
	}

	// �`����e�����Z�b�g����
	, reset: function () {
		var i;

		// IE�̈���v���r���[�ŕʂ̕`��̓h��Ԃ��\���ɂȂ�Ȃ����Ƃ�����̂�
		// �\���݂̂ł����Ă��A�����Ȃ��Ă������̂ŃO���b�h�`�������
		this.context.lineWidth = this.option["edit"] ? 1 : 0;
		this.context.strokeStyle = "#666";
		this.context.clearRect(0, 0, this.canvas.getWidth(), this.canvas.getHeight());

		this.context.beginPath();
		for (i = 0; i <= this.GRID_NUM_X; i++) {
			this.context.moveTo(this.coord2Pos(i), this.coord2Pos(0));
			this.context.lineTo(this.coord2Pos(i), this.coord2Pos(this.GRID_NUM_Y));
		}
		for (i = 0; i <= this.GRID_NUM_Y; i++) {
			this.context.moveTo(this.coord2Pos(0), this.coord2Pos(i));
			this.context.lineTo(this.coord2Pos(this.GRID_NUM_X), this.coord2Pos(i));
		}
		this.context.stroke();

		for (i = 0; i < this.layout_object.length; i++) {
			this.layout_object[i].reset(!this.init_flag || i < this.layout_object.length - 1);
		}
		this.setHidden();
	}

	// �}�E�X�̈ʒu����_��ݒ肷��
	// �����Fe �C�x���g�I�u�W�F�N�g
	, setPoint: function (e) {
		if (this.layout_object.length < 1) return;

		var pos = Element.positionedOffset(this.canvas);
		var x = this.pos2Coord(e.pointerX() - pos.left - (document.documentElement.clientLeft || 0));
		var y = this.pos2Coord(e.pointerY() - pos.top - (document.documentElement.clientTop || 0));

		var layout_object = this.layout_object[this.layout_object.length - 1];
		if (!layout_object.isValidPoint(x, y)) return;

		layout_object.setPoint(x, y);
		this.setHidden();
	}

	// �Ō�̓_���폜���ĕ`�悵����
	, undo: function() {
		if (this.layout_object.length < 1) return;

		var layout_object = this.layout_object[this.layout_object.length - 1];
		var point = [], point_tmp = layout_object.getPoint(), i;

		for (i = 0; i < point_tmp.length - 1; i++) point.push(point_tmp[i]);
		this.reset();

		for (i = 0; i < point.length; i++) layout_object.setPoint(point[i].x, point[i].y);
		this.setHidden();
	}

	// �w�肳�ꂽpx�ʒu����A�O���b�h�̍��W��Ԃ�
	// �����Fpos px�ʒu
	// �Ԓl�F���W�B�O���b�h�O�Ȃ�-1
	, pos2Coord: function (pos) {
		pos -= this.OFFSET;	// �v�Z���₷���悤�ɃO���b�h�̕\���J�n�ʒu�ɂ��Y���𖳂���

		var half = Math.floor(this.GRID_SIZE / 2);
		pos += half;	// �O���b�h�̃N���X�ʒu�𒆐S�Ƃ����X�i�b�v�ɂ���
		if (pos < 0) return -1;	// �O���b�h�O�Ȃ̂Ŗ���

		if ((pos % this.GRID_SIZE - half) % half > 6) return -1;	// �O���b�h�̃N���X�ʒu���痣�ꂷ���Ă���ꍇ�͖����Ƃ���

		return Math.floor(pos / this.GRID_SIZE);
	}

	// ���W����px�ʒu��Ԃ�
	// �����Fcoord ���W
	// �Ԓl�Fpx�ʒu
	, coord2Pos: function (coord) {
		return coord * this.GRID_SIZE + this.OFFSET;
	}

	// �_���L�����Ƃ���
	// �����Fx X���W
	//       y Y���W
	// �Ԓl�F�L���Ȃ�true�A����ȊO��false
	, isValidPoint: function (x, y) {
		return x >= 0 && x <= this.GRID_NUM_X && y >= 0 && y <= this.GRID_NUM_Y;
	}

	// �_�̍��Whidden����
	, setHidden: function () {
		var point, str, i, j;

		this.hidden.innerHTML = "";

		for (i = 0; i < this.layout_object.length; i++) {
			point = this.layout_object[i].getPoint();
			str = "";

			for (j = 0; j < point.length; j++) {
				if (j > 0) str += ":";
				str += point[j].x + "," + point[j].y;
			}

			this.hidden.innerHTML += "<input type='hidden' name='" + this.layout_object[i].option["name"] + "' value='" + str + "'>";
		}
	}
});
LayoutGrid = new LayoutGrid();
