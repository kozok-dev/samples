// レイアウト多角形
var LayoutPolygon = Class.create(LayoutObject, {
	POINT_MAX: 20	// 点の最大数

	, initialize: function ($super, point, option) {
		$super(point, option);
		if (!("inner" in this.option)) this.option["inner"] = 0;
	}

	, isValidPoint: function (x, y) {
		var i;

		if (this.point.length >= 4 && this.point[0].x == this.point[this.point.length - 1].x
			&& this.point[0].y == this.point[this.point.length - 1].y) {

			return false;	// 完成しているので点は追加できない
		}
		if (this.point.length >= 1 && this.point.length <= 2 && this.point[0].x == x && this.point[0].y == y) {
			return false;	// 点は3個以上必要なのに結ぼうとしたので点は追加できない
		}

		if (!LayoutGrid.isValidPoint(x, y)) return false;

		// 開始点の除き、点の重複は許可しない
		for (i = 1; i < this.point.length; i++) {
			if (this.point[i].x == x && this.point[i].y == y) return false;
		}

		if (this.point.length >= this.POINT_MAX && (this.point[0].x != x || this.point[0].y != y)) {
			// 点の最大数に達しているのに新たに点を追加しようとした
			alert("これ以上追加できません。");
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

		if (this.point.length >= 4 && this.point[0].x == x && this.point[0].y == y) {	// 結ぶ？
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

		// 点を描画
		if (!comp_flag && this.option["point"]) {
			this.context.lineWidth = 1;
			this.context.strokeStyle = this.context.fillStyle = this.point.length <= 1 ? "#f00" : "#000";
			this.context.beginPath();
			this.context.arc(LayoutGrid.coord2Pos(x), LayoutGrid.coord2Pos(y), 3, 0, Math.PI * 2, false);
			this.context.closePath();
			this.context.fill();
			this.context.stroke();
		}

		// 内側
		if (comp_flag) this.drawInnerLine();
	}

	// 最大幅と最大高さを取得
	// 引数：init_flag trueにすると初期値用の点情報が対象となる
	// 返値：サイズ情報
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

	// 内側に線を描く
	// 備考：点の数>=4かつ終点=始点とし、同じ座標が連続しないものとする
	, drawInnerLine: function () {
		if (this.option["inner"] == 0) return;

		var point = [], i, j, a, b, al, bl;
		var rad, pos, pos0, pos1, pos_cmp0, pos_cmp1, cross_count;

		// 多角形の各線分を多角形の内側に移動
		for (i = 0; i < this.point.length - 1; i++) {
			a = this.point[i];	// 今の点
			b = this.point[i + 1];	// 1つ後の点

			rad = this.getSegmentRadian(a, b);
			rad += Math.PI / 2;	// 直角
			rad %= 2 * Math.PI;

			// rad(Y軸)をレイとし、レイの方向は-1として多角形内外判定を行う(cross_countが0なら外、それ以外は内)
			// (レイと直線上の線分は含まない)
			cross_count = 0;
			pos = {x: a.x + (b.x - a.x) / 2, y: a.y + (b.y - a.y) / 2};	// 内外判定の基準位置
			pos0 = this.point[0];
			pos_cmp0 = this.comparePoint(pos, pos0, rad);
			for (j = 1; j < this.point.length; j++) {
				pos1 = this.point[j];
				pos_cmp1 = this.comparePoint(pos, pos1, rad);

				if (pos_cmp0.x == -1 ^ pos_cmp1.x == -1) {	// レイが交差する可能性有り？(点接触、線分接触を考慮)
					if (pos_cmp0.y == -1 && pos_cmp1.y == 1 || pos_cmp0.y == 1 && pos_cmp1.y == -1) {	// 交差するかどうかは計算する必要有り？
						al = this.getNormalYAxisPoint(pos, pos0, rad);
						bl = this.getNormalYAxisPoint(pos, pos1, rad);
						if (pos.y > al.y + (bl.y - al.y) * (pos.x - al.x) / (bl.x - al.x)) {	// レイの方向に交差する？
							cross_count += pos_cmp0.x < pos_cmp1.x ? 1 : -1;
						}
					} else if (pos_cmp0.y == -1 || pos_cmp1.y == -1) {	// レイの方向に交差する？
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

		// 多角形の内側に移動した直線と1つの前の直線の交点を設定
		for (i = 0; i < point.length; i++) {
			a = point[i];
			b = point[i > 0 ? i - 1 : point.length - 1];

			pos = this.getCrossSegment(a, b, true);
			if (pos == null) {
				// ありえないが念のため
				a.x = b.x2;
				a.y = b.y2;
			} else {
				a.x = pos.x;
				a.y = pos.y;
			}
		}

		// 下記処理がし易いように配列の各要素に線分を設定する
		for (i = 0; i < point.length; i++) {
			a = point[i];
			b = point[i > 0 ? i - 1 : point.length - 1];

			a.x1 = b.x2 = a.x;
			a.y1 = b.y2 = a.y;
			a.e = a.e1 = a.e2 = true;
		}

		// 縮小後の点が妥当かどうか設定
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

		// 必要な点を設定
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
				// 不要な点の一つ前の点にマークをつけ、下記処理で交点を設定できるようにする
				point[i > 0 ? i - 1 : point.length - 1].d = true;
			}
		}
		point = eval(Object.toJSON(point_tmp));

		// 必要な点で再度、直線と1つの前の直線の交点と線分を設定
		for (i = 0; i < point.length; i++) {
			a = point[i];
			b = point[i > 0 ? i - 1 : point.length - 1];
			if (!b.d) continue;

			pos = this.getCrossSegment(a, b, true);
			if (pos == null) {
				// ありえないが念のため
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

		// 多角形の内側に移動した線分の交点を計算し、描画を行う
		var draw_flag = true;
		this.context.lineWidth = 1;
		this.context.strokeStyle = "#f00";
		this.context.beginPath();
		for (i = 0; i < point.length; i++) {
			a = point[i];

			pos = [];
			for (j = 0; j < point.length; j++) {
				// 対象を含む前後の線分は除外
				if (j == i) continue;
				if (i > 0 && j == i - 1 || i <= 0 && j == point.length - 1) continue;
				if (i < point.length - 1 && j == i + 1 || i >= point.length - 1 && j == 0) continue;

				b = point[j];

				pos0 = this.getCrossSegment(a, b);
				if (pos0 == null) continue;

				pos.push(pos0);

				// 交点までの距離の昇順に挿入ソートで並べ替え
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

	// 線分のラジアンを3時の方向を0度として取得
	// 引数：a 基準となる座標
	//       b 座標
	// 返値：0度以上360度未満のラジアン
	, getSegmentRadian: function (a, b) {
		var ab = Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
		var ac = Math.sqrt(Math.pow(a.x - (a.x + 1), 2));
		var rad = Math.acos(((a.x - b.x) * (a.x - (a.x + 1))) / (ab * ac));	// 3時の方向を0度とした場合の線分の角度
		if (a.y < b.y) rad = 2 * Math.PI - rad;
		return rad;
	}

	// 指定されたラジアンがY軸になっている座標を基準となる座標を中心に通常のY軸(0時の方向)の座標に
	// 変換した場合の座標を返す
	// 引数：a 基準となる座標
	//     ：b 対象の座標
	//     ：rad ラジアン
	// 返値：座標
	, getNormalYAxisPoint: function (a, b, rad) {
		rad = Math.PI / 2 - rad;	// 90度(0時の方向)までの差
		return {
			x: a.x + ((b.x - a.x) * Math.cos(rad) - (b.y - a.y) * -Math.sin(rad))
			, y: a.y + ((b.x - a.x) * -Math.sin(rad) + (b.y - a.y) * Math.cos(rad))
		};
	}

	// 基準となる座標と指定した座標との位置関係を角度を考慮して返す
	// 引数：a 基準となる座標
	//     ：b 対象の座標
	//     ：rad 基準となるラジアン(Y軸)
	// 返値：X、Yでそれぞれ、-1:基準より小さい、0:同じ、1:基準より大きい
	, comparePoint: function (a, b, rad) {
		var ret = {x: 0, y: 0}, rad2, rad3;
		var rad_ab = this.round(this.getSegmentRadian(a, b));

		// 計算時、角度がマイナスになったり360度以上になると面倒なのでradによって計算方法を変えている

		// X (radから180度より小さいなら-1、大きいなら1)
		if (rad >= Math.PI) {	// 180度以上？
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

		// Y (radの前後90度より小さいなら-1、大きいなら1)
		if (rad < Math.PI / 2) {	// 90度未満？
			rad2 = this.round(rad + Math.PI / 2);
			rad3 = this.round(rad + Math.PI * 1.5);
			if (rad_ab < rad2 || rad_ab > rad3) ret.y = -1;
			else if (rad_ab > rad2 && rad_ab < rad3) ret.y = 1;
		} else if (rad >= Math.PI * 1.5) {	// 270度以上？
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

	// 2線分の交点を取得する
	// 引数：a 基準となる線分
	//       b 対象の線分
	//       line_flag trueにすると線分ではなく、直線として計算する
	// 返値：交点情報。交差しない場合はnull
	, getCrossSegment: function (a, b, line_flag) {
		var p = (a.x2 - a.x1) * (b.y2 - b.y1) - (a.y2 - a.y1) * (b.x2 - b.x1);
		var r = (b.y2 - b.y1) * (b.x1 - a.x1) - (b.x2 - b.x1) * (b.y1 - a.y1);
		var s = (a.y2 - a.y1) * (b.x1 - a.x1) - (a.x2 - a.x1) * (b.y1 - a.y1);
		var x, y, flag;

		if (p == 0 && r == 0 && s == 0) {	// 直線が重なり合っている？
			if (!line_flag) {
				r = Math.sqrt(Math.pow(a.x1 - a.x2, 2) + Math.pow(a.y1 - a.y2, 2));
				flag = false;

				s = Math.sqrt(Math.pow(a.x1 - b.x1, 2) + Math.pow(a.y1 - b.y1, 2));
				if (r > s && (a.x2 - a.x1) * (b.x1 - a.x1) + (a.y2 - a.y1) * (b.y1 - a.y1) == r * s) {
					flag = true;	// 線分aがbの始点とaの端点以外で重なっている
				}
				s = Math.sqrt(Math.pow(a.x1 - b.x2, 2) + Math.pow(a.y1 - b.y2, 2));
				if (r > s && (a.x2 - a.x1) * (b.x2 - a.x1) + (a.y2 - a.y1) * (b.y2 - a.y1) == r * s) {
					flag = true;	// 線分aがbの終点とaの端点以外で重なっている
				}

				if (!flag) return null;
			}

			x = b.x2;
			y = b.y2;
			r = s = 0;
		} else {
			if (p == 0) return null;	// 平行なので交差しない

			r /= p;
			s /= p;
			if (!line_flag) {
				if (r < 0 || r > 1 || s < 0 || s > 1) return null;	// 交差していない
			}

			x = a.x1 + r * (a.x2 - a.x1);
			y = a.y1 + r * (a.y2 - a.y1);
		}

		return {
			x: x	// 交点のX座標
			, y: y	// 交点のY座標
			, d: Math.sqrt(Math.pow(a.x1 - x, 2) + Math.pow(a.y1 - y, 2))	// 交点までの距離
			, r: r	// 線分aを基準にした交点の位置の比率
			, s: s	// 線分bを基準にした交点の位置の比率
			, p: p	// 0なら平行
		};
	}

	// 小数点を一定の精度に変換して返す
	// 引数：value 数値
	// 返値：数値
	, round: function (value) {
		return Math.round(value * 10000000000) / 10000000000;
	}
});
