// レイアウトオブジェクト
var LayoutObject = Class.create({
	context: null	// canvasコンテキスト
	, point: null	// 点の座標を格納する配列
	, point_init: null	// 点の座標を格納する初期値用の配列
	, option: null	// 設定

	// コンストラクター
	// 引数：point 点の座標を格納する配列
	//       option 設定
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

	// 点を初期化し、場合によって初期描画を行う
	// 引数：init_flag 初期描画するならtrue
	, reset: function (init_flag) {
		var i;

		this.point = [];
		if (!init_flag) return;

		for (i = 0; i < this.point_init.length; i++) this.setPoint(this.point_init[i].x, this.point_init[i].y);
	}

	// canvasコンテキストを設定する
	// 返値：canvasコンテキスト
	, getContext: function () {
		return this.context;
	}

	// canvasコンテキストを設定する
	// 引数：context canvasコンテキスト
	, setContext: function (context) {
		this.context = context;
	}

	// 点が有効がとうか
	// 引数：x X座標
	//       y Y座標
	// 返値：有効ならtrue、それ以外はfalse
	, isValidPoint: function (x, y) {
		return LayoutGrid.isValidPoint(x, y);
	}

	// 点情報を取得する
	// 返値：点の座標の配列
	, getPoint: function () {
		return this.point;
	}

	// 点を設定と描画を行う
	// 引数：x X座標
	//       y Y座標
	, setPoint: function (x, y) {
	}
});
