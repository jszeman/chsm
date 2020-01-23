
export class Model {
	constructor(data)
	{
		this.data = data;
		this.options = {
			state_min_width: 5,
			state_min_height: 3,
			text_height: 2,
		}
	}

	move_state(state_id, pos)
	{
		this.data.states[state_id].pos = pos;
	}

	resize_state(state_id, size)
	{
		const [w, h] = size;
		const state =	this.data.states[state_id];

		const vert_offsets = state.connectors.map((c) => {
					const conn = this.data.connectors[c];
					if (['left', 'right'].includes(conn.side))
					{
						return conn.offset;
					}
					else
					{
						return 0;
					}
				}, this);
		
		const horiz_offsets = state.connectors.map((c) => {
					const conn = this.data.connectors[c];
					if (['top', 'bottom'].includes(conn.side))
					{
						return conn.offset;
					}
					else
					{
						return 0;
					}
				}, this);
		


		state.size[0] = Math.max(w,
			this.options.state_min_width,
			Math.max(...horiz_offsets)+1);
		state.size[1] = Math.max(h,
			(state.text.length + 2)*this.options.text_height,
			Math.max(...vert_offsets)+1);
	}

	get_state(state_id)
	{
		return this.data.states[state_id];
	}

	get_transition(trans_id)
	{
		return this.data.transitions[trans_id];
	}

	states()
	{
		return Object.keys(this.data.states);
	}

	transitions()
	{
		return Object.keys(this.data.transitions);
	}

	state_transitions(state_id)
	{
		return this.data.states[state_id].connectors
			.map((c) => this.data.connectors[c].transition, this);
	}

	point_is_in_line(p, a, b)
	{
		const [px, py] = p;
		const [ax, ay] = a;
		const [bx, by] = b;

		if ((px === ax) && (px === bx))
		{
			const [y1, y2] = ay < by ? [ay, by] : [by, ay];

			if ((py > y1) && (py < y2))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		if ((py === ay) && (py === by))
		{
			const [x1, x2] = ax < bx ? [ax, bx] : [bx, ax];

			if ((px > x1) && (px < x2))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		return false;
	}

	transition_get_line_index(tr_id, point)
	{
		const tr = this.get_transition(tr_id);

		for (let i=0; i<(tr.vertices.length-1); i++)
		{
			if (this.point_is_in_line(point, tr.vertices[i], tr.vertices[i+1]))
			{
				return i;
			}
		}

		return null;
	}

	move_connector(conn_id, pos)
	{
		const conn = this.data.connectors[conn_id];
		const state = this.data.states[conn.parent];
		const [x, y] = pos;
		const [sx, sy] = state.pos;

		switch (conn.side)
		{
			case 'top':
			case 'bottom':
				conn.offset = x - sx;
				break;

			case 'left':
			case 'right': conn.offset = y - sy;
				break;
		}
	}

	transition_drag(tr_id, line_idx, p)
	{
		const [x, y] = p;
		const tr = this.get_transition(tr_id);

		const l1 = tr.vertices[line_idx];
		const l2 = tr.vertices[line_idx+1];

		if (line_idx == 0)
		{
			const [min, max] = this.get_connector_movement_limits(tr.start);
			if (l1[0] === l2[0])
			{
				l1[0] = Math.min(max, Math.max(x, min));
				l2[0] = l1[0];
			}
			else
			{
				l1[1] = Math.min(max, Math.max(y, min));
				l2[1] = l1[1];
			}

			this.move_connector(tr.start, l1);
		}
		else if (line_idx == (tr.vertices.length - 2))
		{
			const [min, max] = this.get_connector_movement_limits(tr.end);
			if (l1[0] === l2[0])
			{
				l1[0] = Math.min(max, Math.max(x, min));
				l2[0] = l1[0];
			}
			else
			{
				l1[1] = Math.min(max, Math.max(y, min));
				l2[1] = l1[1];
			}

			this.move_connector(tr.end, l2);
		}
		else
		{
			if (l1[0] === l2[0])
			{
				l1[0] = x;
				l2[0] = x;
			}
			else
			{
				l1[1] = y;
				l2[1] = y;
			}
		}
	}

	connectors()
	{
		return Object.keys(this.data.connectors);
	}

	get_connector_pos(conn_id)
	{
		const conn = this.data.connectors[conn_id];
		const parent = this.data.states[conn.parent];

		switch (conn.side) {
			case 'top':
				return [parent.pos[0] + conn.offset, parent.pos[1]];

			case 'bottom':
				return [parent.pos[0] + conn.offset, parent.pos[1] + parent.size[1]];

			case 'left':
				return [parent.pos[0], parent.pos[1] + conn.offset];

			case 'right':
				return [parent.pos[0] + parent.size[0], parent.pos[1] + conn.offset];
		}
	}

	get_connector_movement_limits(conn_id)
	{
		const conn = this.data.connectors[conn_id];
		const parent = this.data.states[conn.parent];
		const [x, y] = parent.pos;
		const [w, h] = parent.size;

		switch (conn.side) {
			case 'top':
			case 'bottom':
				return [x + 1, x + w - 1];

			case 'left':
			case 'right':
				return [y + 1, y + h - 1];
		}
	}

	update_transition_path(tr_id)
	{
		const tr = this.data.transitions[tr_id];
		const [sx, sy] = this.get_connector_pos(tr.start);
		const [ex, ey] = this.get_connector_pos(tr.end);

		const [xs, ys] = tr.vertices[0];
		const [xe, ye] = tr.vertices.slice(-1);

		// Check if the start connector has moved
		if ((sx !== xs) || (sy !== ys))
		{
			tr.vertices[0] = [sx, sy];

			if (['left', 'right'].includes(this.data.connectors[tr.start].side))
			{
				tr.vertices[1][1] = sy;
			}
			else
			{
				tr.vertices[1][0] = sx;
			}
		}

		// Check if the end connector has moved
		if ((ex !== xe) || (ey !== ye))
		{
			tr.vertices[tr.vertices.length-1] = [ex, ey];

			const v_1 = tr.vertices[tr.vertices.length-2];

			if (['left', 'right'].includes(this.data.connectors[tr.start].side))
			{
				v_1[1] = ey;
			}
			else
			{
				v_1[0] = ex;
			}
		}
	}
}
