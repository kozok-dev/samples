// レイアウト長方形
var LayoutRectangle = Class.create(LayoutObject, {
	initialize: function ($super, point, option) {
		$super(point, option);
		if (!("inner" in this.option)) this.option["inner"] = 0;
	}

	, isValidPoint: function (x, y) {
		var i;

		if (this.point.length >= 2) return false;
		if (!LayoutGrid.isValidPoint(x, y)) return false;

		// 点の重複は許可しない
		for (i = 0; i < this.point.length; i++) {
			if (this.point[i].x == x && this.point[i].y == y) return false;
		}

		return true;
	}

	, setPoint: function (x, y) {
		this.point.push({x: x, y: y});

		this.context.strokeStyle = "#000";

		// 点を描画
		if (this.option["point"]) {
			this.context.fillStyle = "#000";
			this.context.beginPath();
			this.context.arc(LayoutGrid.coord2Pos(x), LayoutGrid.coord2Pos(y), 3, 0, Math.PI * 2, false);
			this.context.closePath();
			this.context.fill();
			this.context.stroke();
		}

		if (this.point.length >= 2) {
			// 点を結ぶ
			this.context.lineWidth = this.option["line"];
			this.context.fillStyle = this.option["fill"];
			this.context.beginPath();
			this.context.moveTo(LayoutGrid.coord2Pos(this.point[0].x), LayoutGrid.coord2Pos(this.point[0].y));
			this.context.lineTo(LayoutGrid.coord2Pos(this.point[1].x), LayoutGrid.coord2Pos(this.point[0].y));
			this.context.lineTo(LayoutGrid.coord2Pos(this.point[1].x), LayoutGrid.coord2Pos(this.point[1].y));
			this.context.lineTo(LayoutGrid.coord2Pos(this.point[0].x), LayoutGrid.coord2Pos(this.point[1].y));
			this.context.closePath();
			this.context.fill();
			this.context.stroke();

			// 内側
			var pos = this.getInnerPoint();
			if (pos != null) {
				this.context.lineWidth = 1;
				this.context.strokeStyle = "#f00";
				this.context.beginPath();
				this.context.moveTo(Math.round(LayoutGrid.coord2Pos(pos[0].x)), Math.round(LayoutGrid.coord2Pos(pos[0].y)));
				this.context.lineTo(Math.round(LayoutGrid.coord2Pos(pos[1].x)), Math.round(LayoutGrid.coord2Pos(pos[0].y)));
				this.context.lineTo(Math.round(LayoutGrid.coord2Pos(pos[1].x)), Math.round(LayoutGrid.coord2Pos(pos[1].y)));
				this.context.lineTo(Math.round(LayoutGrid.coord2Pos(pos[0].x)), Math.round(LayoutGrid.coord2Pos(pos[1].y)));
				this.context.closePath();
				this.context.stroke();
			}
		}
	}

	// 座標を始点<=終点となるようにして返す
	// 返値：座標。点が結べていない場合は無変換の座標
	, getAdjustPoint: function () {
		if (this.point.length < 2) return this.point;

		var x1 = this.point[0].x;
		var y1 = this.point[0].y;
		var x2 = this.point[1].x;
		var y2 = this.point[1].y;
		var tmp;

		if (x1 > x2) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		if (y1 > y2) {
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}

		return [
			{x: x1, y: y1}
			, {x: x2, y: y2}
		];
	}

	// 内側の座標情報を返す
	// 返値：座標情報。内側の長方形を描画できない場合はnull
	, getInnerPoint: function () {
		if (this.point.length < 2 || this.option["inner"] == 0) return null;

		var point = this.getAdjustPoint();
		var length = this.option["inner"];

		if (length > (point[1].x - point[0].x) / 2 || length > (point[1].y - point[0].y) / 2) return null;

		return [
			{x: point[0].x + length, y: point[0].y + length}
			, {x: point[1].x - length, y: point[1].y - length}
		];
	}
});
