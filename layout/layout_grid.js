// レイアウトグリッド
var LayoutGrid = Class.create({
	GRID_NUM_X: 25	// Xグリッド数
	, GRID_NUM_Y: 20	// Yグリッド数
	, GRID_SIZE: 20	// グリッドpxサイズ
	, OFFSET: 5	// グリッドの表示開始px位置

	, canvas: null	// canvasDOM
	, context: null	// canvasコンテキスト
	, layout_object: null	// レイアウトオブジェクト
	, option: null	// 設定
	, hidden: null	// 点の座標hiddenDOM
	, init_flag: null	// 初期化済みならtrue

	// コンストラクター
	, initialize: function () {
		this.option = {edit: true};
		this.init_flag = false;
	}

	// 初期化
	// 引数：canvas canvasDOM
	//       layout_object レイアウトオブジェクト配列
	//       option 設定
	, init: function (canvas, layout_object, option) {
		var i;

		this.canvas = canvas;
		this.context = this.canvas.getContext("2d");
		this.hidden = new Element("div");

		// レイアウトオブジェクト
		this.layout_object = [];
		for (i = 0; i < layout_object.length; i++) {
			this.layout_object[i] = layout_object[i];
			this.layout_object[i].setContext(this.context);
		}

		// 描画設定
		for (i in option) this.option[i] = option[i];

		this.context.translate(0.5, 0.5);
		Element.setStyle(this.canvas, {cursor: "default"});
		Element.hide(this.hidden);
		Element.insert(Element.up(this.canvas), this.hidden);
		this.reset();
		if (this.option["edit"]) Event.observe(this.canvas, "click", this.setPoint.bind(this));

		this.init_flag = true;
	}

	// 初期化済みかどうか
	// 返値：初期化済みならtrue、それ以外はfalse
	, isInit: function () {
		return this.init_flag;
	}

	// 描画内容をリセットする
	, reset: function () {
		var i;

		// IEの印刷プレビューで別の描画の塗りつぶし表示にならないことがあるので
		// 表示のみであっても、見えなくてもいいのでグリッド描画をする
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

	// マウスの位置から点を設定する
	// 引数：e イベントオブジェクト
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

	// 最後の点を削除して描画し直す
	, undo: function() {
		if (this.layout_object.length < 1) return;

		var layout_object = this.layout_object[this.layout_object.length - 1];
		var point = [], point_tmp = layout_object.getPoint(), i;

		for (i = 0; i < point_tmp.length - 1; i++) point.push(point_tmp[i]);
		this.reset();

		for (i = 0; i < point.length; i++) layout_object.setPoint(point[i].x, point[i].y);
		this.setHidden();
	}

	// 指定されたpx位置から、グリッドの座標を返す
	// 引数：pos px位置
	// 返値：座標。グリッド外なら-1
	, pos2Coord: function (pos) {
		pos -= this.OFFSET;	// 計算しやすいようにグリッドの表示開始位置によるズレを無くす

		var half = Math.floor(this.GRID_SIZE / 2);
		pos += half;	// グリッドのクロス位置を中心としたスナップにする
		if (pos < 0) return -1;	// グリッド外なので無効

		if ((pos % this.GRID_SIZE - half) % half > 6) return -1;	// グリッドのクロス位置から離れすぎている場合は無効とする

		return Math.floor(pos / this.GRID_SIZE);
	}

	// 座標からpx位置を返す
	// 引数：coord 座標
	// 返値：px位置
	, coord2Pos: function (coord) {
		return coord * this.GRID_SIZE + this.OFFSET;
	}

	// 点が有効がとうか
	// 引数：x X座標
	//       y Y座標
	// 返値：有効ならtrue、それ以外はfalse
	, isValidPoint: function (x, y) {
		return x >= 0 && x <= this.GRID_NUM_X && y >= 0 && y <= this.GRID_NUM_Y;
	}

	// 点の座標hidden生成
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
