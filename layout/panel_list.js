// パネル管理
var PanelList = Class.create({
	canvas: null	// canvasDOM
	, hidden: null	// パネル情報hiddenDOM
	, panel_list: null	// Panelオブジェクト配列
	, panel_select_index: null	// 選択しているパネルのインデックス
	, option: null	// 設定

	// コンストラクター
	, initialize: function () {
		this.panel_list = [];
		this.panel_select_index = -1;
	}

	// 初期化
	// 引数：canvas canvasDOM
	//       option 設定
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

	// パネル追加
	// 引数：width 幅
	//       height 高さ
	//       x X位置
	//       y Y位置
	//       no_select_flag 選択状態にしないならtrue
	// 返値：追加したPanel
	, add: function (width, height, x, y, no_select_flag) {
		var panel_obj, i;

		width = parseInt(width);
		height = parseInt(height);
		if (isNaN(width) || isNaN(height)) {
			alert("半角数字で入力してください。");
			return;
		}
		if (width < 30 || width > 300) {
			alert("幅は30～300で入力してください。");
			return;
		}
		if (height < 30 || height > 300) {
			alert("高さは30～300で入力してください。");
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

	// パネルを選択する
	// 引数：e イベントオブジェクト
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

	// 選択しているパネルを削除する
	, removeSelected: function () {
		if (this.panel_select_index < 0 || this.panel_select_index >= this.panel_list.length) return;

		var panel_obj = this.panel_list[this.panel_select_index];
		panel_obj.remove();
		this.panel_list.splice(this.panel_select_index, 1);
		this.panel_select_index = -1;

		this.setHidden();
	}

	// 全てのパネルを削除する
	, removeAll: function () {
		this.panel_select_index = -1;
		for (i = 0; i < this.panel_list.length; i++) this.panel_list[i].remove();
		this.panel_list = [];
		this.setHidden();
	}

	// パネル情報取得
	// 引数：panel パネル情報文字列。省略すると現在のパネル情報が対象となる
	// 返値：パネル情報
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

	// パネルの位置を調整する
	, adjustPosition: function () {
		var i;
		for (i = 0; i < this.panel_list.length; i++) this.panel_list[i].adjustPosition();
	}

	// パネル情報hidden生成
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
