// ���C�A�E�g�I�u�W�F�N�g
var LayoutObject = Class.create({
	context: null	// canvas�R���e�L�X�g
	, point: null	// �_�̍��W���i�[����z��
	, point_init: null	// �_�̍��W���i�[���鏉���l�p�̔z��
	, option: null	// �ݒ�

	// �R���X�g���N�^�[
	// �����Fpoint �_�̍��W���i�[����z��
	//       option �ݒ�
	, initialize: function (point, option) {
		var x, y, i;

		this.option = {
			point: true
			, line: 2
			, fill: "rgba(128, 128, 255, 0.5)"
			, name: "point"
		};
		for (i in option) this.option[i] = option[i];

		this.point = [];
		this.point_init = [];
		point = (point + "").split(":");
		for (i = 0; i < point.length; i++) {
			if (!point[i].match(/^(0|[1-9]\d*),(0|[1-9]\d*)$/)) break;
			x = parseInt(RegExp.$1);
			y = parseInt(RegExp.$2);
			if (!this.isValidPoint(x, y)) break;

			this.point.push({x: x, y: y});
			this.point_init.push({x: x, y: y});
		}
	}

	// �_�����������A�ꍇ�ɂ���ď����`����s��
	// �����Finit_flag �����`�悷��Ȃ�true
	, reset: function (init_flag) {
		var i;

		this.point = [];
		if (!init_flag) return;

		for (i = 0; i < this.point_init.length; i++) this.setPoint(this.point_init[i].x, this.point_init[i].y);
	}

	// canvas�R���e�L�X�g��ݒ肷��
	// �Ԓl�Fcanvas�R���e�L�X�g
	, getContext: function () {
		return this.context;
	}

	// canvas�R���e�L�X�g��ݒ肷��
	// �����Fcontext canvas�R���e�L�X�g
	, setContext: function (context) {
		this.context = context;
	}

	// �_���L�����Ƃ���
	// �����Fx X���W
	//       y Y���W
	// �Ԓl�F�L���Ȃ�true�A����ȊO��false
	, isValidPoint: function (x, y) {
		return LayoutGrid.isValidPoint(x, y);
	}

	// �_�����擾����
	// �Ԓl�F�_�̍��W�̔z��
	, getPoint: function () {
		return this.point;
	}

	// �_��ݒ�ƕ`����s��
	// �����Fx X���W
	//       y Y���W
	, setPoint: function (x, y) {
	}
});
