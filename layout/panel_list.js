// �p�l���Ǘ�
var PanelList = Class.create({
	canvas: null	// canvasDOM
	, hidden: null	// �p�l�����hiddenDOM
	, panel_list: null	// Panel�I�u�W�F�N�g�z��
	, panel_select_index: null	// �I�����Ă���p�l���̃C���f�b�N�X
	, option: null	// �ݒ�

	// �R���X�g���N�^�[
	, initialize: function () {
		this.panel_list = [];
		this.panel_select_index = -1;
	}

	// ������
	// �����Fcanvas canvasDOM
	//       option �ݒ�
	, init: function (canvas, option) {
		var i;

		this.canvas = canvas;
		var parent = Element.up(this.canvas);

		this.option = {name: "panel", edit: true};
		for (i in option) this.option[i] = option[i];

		this.hidden = new Element("input", {type: "hidden", name: this.option["name"]});
		Element.hide(this.hidden);
		Element.insert(parent, this.hidden);

		if (this.option["edit"]) {
			Event.observe(parent, "mousedown", this.select.bind(this));
			this.option["drag_end"] = this.setHidden.bind(this);
		}
		Event.observe(window, "resize", this.adjustPosition.bind(this));
	}

	// �p�l���ǉ�
	// �����Fwidth ��
	//       height ����
	//       x X�ʒu
	//       y Y�ʒu
	//       no_select_flag �I����Ԃɂ��Ȃ��Ȃ�true
	// �Ԓl�F�ǉ�����Panel
	, add: function (width, height, x, y, no_select_flag) {
		var panel_obj, i;

		width = parseInt(width);
		height = parseInt(height);
		if (isNaN(width) || isNaN(height)) {
			alert("���p�����œ��͂��Ă��������B");
			return;
		}
		if (width < 30 || width > 300) {
			alert("����30�`300�œ��͂��Ă��������B");
			return;
		}
		if (height < 30 || height > 300) {
			alert("������30�`300�œ��͂��Ă��������B");
			return;
		}

		if (!this.option["edit"]) this.option["text"] = this.panel_list.length + 1;
		panel_obj = new Panel(this.canvas, width, height, x, y, this.option);
		this.panel_list.push(panel_obj);

		if (no_select_flag) {
			panel_obj.draw(false);
		} else {
			for (i = 0; i < this.panel_list.length - 1; i++) this.panel_list[i].draw(false);
			this.panel_select_index = this.panel_list.length - 1;
			panel_obj.draw(true);
		}

		this.setHidden();
		return panel_obj;
	}

	// �p�l����I������
	// �����Fe �C�x���g�I�u�W�F�N�g
	, select: function (e) {
		var panel_select = e.element();
		var panel, panel_select_index, i;

		while (panel_select != null && panel_select.getContext == null) panel_select = panel_select.parentNode;

		panel_select_index = this.panel_select_index;
		this.panel_select_index = -1;
		for (i = 0; i < this.panel_list.length; i++) {
			panel = this.panel_list[i].getPanel();
			if (panel != panel_select) continue;

			if (panel_select_index != i) this.panel_list[i].draw(true);
			this.panel_select_index = i;
			break;
		}
		if (panel_select_index >= 0 && panel_select_index < this.panel_list.length && panel_select_index != i) {
			this.panel_list[panel_select_index].draw(false);
		}
	}

	// �I�����Ă���p�l�����폜����
	, removeSelected: function () {
		if (this.panel_select_index < 0 || this.panel_select_index >= this.panel_list.length) return;

		var panel_obj = this.panel_list[this.panel_select_index];
		panel_obj.remove();
		this.panel_list.splice(this.panel_select_index, 1);
		this.panel_select_index = -1;

		this.setHidden();
	}

	// �S�Ẵp�l�����폜����
	, removeAll: function () {
		this.panel_select_index = -1;
		for (i = 0; i < this.panel_list.length; i++) this.panel_list[i].remove();
		this.panel_list = [];
		this.setHidden();
	}

	// �p�l�����擾
	// �����Fpanel �p�l����񕶎���B�ȗ�����ƌ��݂̃p�l����񂪑ΏۂƂȂ�
	// �Ԓl�F�p�l�����
	, getInfo: function (panel) {
		var info = [], i;

		if (panel == null) {
			for (i = 0; i < this.panel_list.length; i++) info.push(this.panel_list[i].getInfo());
		} else {
			panel = (panel + "").split(":");
			for (i = 0; i < panel.length; i++) {
				if (!panel[i].match(/^(0|[1-9]\d*),(0|[1-9]\d*),(0|[1-9]\d*),(0|[1-9]\d*)$/)) continue;

				info.push({
					width: RegExp.$1
					, height: RegExp.$2
					, x: parseInt(RegExp.$3)
					, y: parseInt(RegExp.$4)
				});
			}
		}
		return info;
	}

	// �p�l���̈ʒu�𒲐�����
	, adjustPosition: function () {
		var i;
		for (i = 0; i < this.panel_list.length; i++) this.panel_list[i].adjustPosition();
	}

	// �p�l�����hidden����
	, setHidden: function () {
		var info, str = "", i;

		for (i = 0; i < this.panel_list.length; i++) {
			info = this.panel_list[i].getInfo();
			if (i > 0) str += ":";
			str += info.width + "," + info.height + "," + info.x + "," + info.y;
		}

		this.hidden.value = str;
	}
});
PanelList = new PanelList();
