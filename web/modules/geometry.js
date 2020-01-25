export class Rect {
	constructor(x1, y1, x2, y2)
	{
		this.data = [x1, y1, x2, y2];
	}

	overlaps(other)
	{
		/*
		 * https://stackoverflow.com/questions/306316 
	 	 */

		const [x1, y1, x2, y2] = this.data;
		const [ox1, oy1, ox2, oy2] = other.data;

		return ((x1 < ox2) && (x2 > ox1) && (y1 > oy2) && (y2 < oy1));
	}

	is_inside(other)
	{
		const [x1, y1, x2, y2] = this.data;
		const [ox1, oy1, ox2, oy2] = other.data;

		return ((x1 > ox1) && (x2 < ox2) && (y1 > oy1) && (y2 < oy2));
	}
}

export class Point {
	constructor(x, y)
	{
		this.data = [x, y];
	}

	is_inside_rect(rect)
	{
		const [x, y] = this.data;
		const [rx1, ry1, rx2, ry2] = rect.data;

		return ((x > rx1) && (x < rx2) && (y > ry1) && (y < ry2));
	}
}
