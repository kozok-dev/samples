// パネルオブジェクト
var Panel = Class.create({
	panel: null	// パネルDOM
	, canvas: null	// canvasDOM
	, context: null	// canvasコンテキスト
	, width: null	// 幅
	, height: null	// 高さ
	, x: null	// X位置
	, y: null	// Y位置
	, text: null	// パネルに表示する文字列
	, option: null	// 設定
	, first_draw: null	// 描画が行われたらtrueにする

	// コンストラクター
	// 引数：canvas canvasDOM
	//       width 列
	//       height 高さ
	//       x X位置
	//       y Y位置
	//       option 設定
	, initialize: function (canvas, width, height, x, y, option) {
		var pos, i;

		this.canvas = canvas;
		this.width = width;
		this.height = height;

		this.option = {
			text: ""
			, drag_end: function () {}
			, edit: true
		};
		for (i in option) this.option[i] = option[i];
		this.first_draw = false;

		this.panel = new Element(window.G_vmlCanvasManager != null ? "div" : "canvas");
		this.setSize();
		Element.setStyle(this.panel, {position: "absolute", cursor: "default"});

		if (window.G_vmlCanvasManager != null) G_vmlCanvasManager.initElement(this.panel);
		this.context = this.panel.getContext("2d");
		this.context.translate(0.5, 0.5);

		pos = Element.positionedOffset(this.canvas);
		this.x = x || 0;
		this.y = y || 0;
		jQuery(this.panel).draggable({
			containment: this.canvas
			, disabled: !this.option["edit"]
			, drag: this.adjustTextPosition.bind(this)
			, stop: this.setPosition.bind(this)
		});
		jQuery(this.panel).offset({
			left: pos.left + jQuery(document).scrollLeft() - (document.documentElement.clientLeft || 0) + this.x
			, top: pos.top + jQuery(document).scrollTop() - (document.documentElement.clientTop || 0) + this.y
		});

		var parent = Element.up(this.canvas);

		Element.insert(parent, this.panel);
		if (window.G_vmlCanvasManager != null) {
			// IEでサイズが設定されないことがあるので再度設定しておく
			this.panel.firstChild.style.width = this.panel.clientWidth;
			this.panel.firstChild.style.height = this.panel.clientHeight;
		}

		if (this.option["text"] != "") {
			this.text = new Element("div");
			Element.setStyle(this.text, {position: "absolute", color: "#0f0"});
			this.text.innerHTML = this.option["text"];
			this.adjustTextPosition();
			Element.insert(parent, this.text);
		}
	}

	// パネルのサイズを設定
	, setSize: function () {
		this.panel.width = Math.round(this.width);
		this.panel.height = Math.round(this.height);
	}

	// パネルDOM取得
	// 返値：パネルDOM
	, getPanel: function () {
		return this.panel;
	}

	// パネル情報取得
	// 返値：パネル情報
	, getInfo: function () {
		return {
			width: this.width
			, height: this.height
			, x: this.x
			, y: this.y
		};
	}

	// パネルを描画する
	// 引数：select 選択状態にするならtrue
	, draw: function (select) {
		if (!select || !this.first_draw) {
			this.context.clearRect(0, 0, this.panel.width, this.panel.height);

			this.context.lineWidth = 1;
			this.context.fillStyle = "rgba(64, 64, 255, 0.9)";
			this.context.strokeStyle = "#000";
			this.context.beginPath();
			this.context.rect(0, 0, this.panel.width - 1, this.panel.height - 1);
			this.context.fill();
			this.context.stroke();

			this.first_draw = true;
		}
		if (select) {
			this.context.strokeStyle = "#0f0";
			this.context.strokeRect(0, 0, this.panel.width - 1, this.panel.height - 1);
		}
	}

	// パネル表示位置調整
	, adjustPosition: function () {
		var pos, obj, left, top;

		if (window.G_vmlCanvasManager != null) {
			// IEの互換表示オンでウィンドウサイズ変更中だとうまく取得できないので、自前で取得する
			obj = this.canvas;
			left = top = 0;
			while (obj != null && Element.getStyle(obj, "position") == "static") {
				left += obj.offsetLeft;
				top += obj.offsetTop;
				obj = obj.offsetParent;
			}
			pos = {left: left, top: top};
		} else {
			pos = Element.positionedOffset(this.canvas);
		}
		this.panel.style.left = (Math.floor(pos.left) + this.x) + "px";
		this.panel.style.top = (Math.floor(pos.top) + this.y) + "px";

		this.adjustTextPosition();
	}

	// パネルの内部の位置を設定
	, setPosition: function () {
		var panel_pos = Element.positionedOffset(this.panel);
		var canvas_pos = Element.positionedOffset(this.canvas);
		this.x = Math.floor(panel_pos.left) - Math.floor(canvas_pos.left);
		this.y = Math.floor(panel_pos.top) - Math.floor(canvas_pos.top);

		this.option["drag_end"]();
	}

	// パネルに表示する文字列の位置を調整
	, adjustTextPosition: function() {
		if (this.text == null) return;

		var pos = Element.positionedOffset(this.panel);
		this.text.style.left = Math.floor(pos.left + 3) + "px";
		this.text.style.top = Math.floor(pos.top + 3) + "px";
	}

	// パネル削除
	, remove: function () {
		Element.remove(this.panel);
	}
});
