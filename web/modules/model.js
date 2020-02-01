import {Rect, Point} from './geometry.js';
export class Model {
	constructor(data)
	{
		this.data = 			data;
		this.options = {
			state_min_width: 	5,
			state_min_height: 	3,
			text_height: 		2,
		};
		this.changes = {
			states:				[],
			transitions:		[],

		};
	}

	make_new_id(array, prefix)
	{
		for (let i=0; i<65536; i++)
		{
			const id = `${prefix}${i}`;
			if (!(id in array))
			{
				return id;
			}
		}
	}

	make_new_transition()
	{
		const trans_id = this.make_new_id(this.data.transitions, 'trans_');
		const start_id = this.make_new_id(this.data.connectors, 'conn_');
		this.data.connectors[start_id] = {};
		const end_id = this.make_new_id(this.data.connectors, 'conn_');
		this.data.connectors[end_id] = {};

		const t = {
			start: start_id,
			end: end_id,
			vertices: [],
			label: trans_id,
			label_offset: [0.5, -0.4],
			label_anchor: 0,
			label_pos: [],
		};
		const start = {
			parent: null,
			offset: 0,
			side: null,
			dir: 'out',
			transition: trans_id,
		};
		const end = {
			parent: null,
			offset: 0,
			side: null,
			dir: 'in',
			transition: trans_id,
		};
		this.data.transitions[trans_id] = t;
		this.data.connectors[start_id] = start;
		this.data.connectors[end_id] = end;

		return trans_id;
	}

	delete_connector(conn_id)
	{
		if (!(conn_id in this.data.connectors))
			return;

		const conn = this.data.connectors[conn_id];
		
		// Remove reference from owner state
		if (conn.parent in this.data.states)
		{
			const s = this.data.states[conn.parent];
			s.connectors = s.connectors.filter(id => id != conn_id);
		}
	
		// Remove reference from attached transition
		if (conn.transition in this.data.transitions)
		{
			const tr = this.data.transitions[conn.transition];

			if (tr.start == conn_id)
			{
				tr.start = null;
			}
			
			if (tr.end == conn_id)
			{
				tr.end = null;
			}
		}

		delete this.data.connectors[conn_id];
	}

	delete_transition(trans_id)
	{

		const t = this.data.transitions[trans_id];
		
		this.delete_connector(t.start);
		this.delete_connector(t.end);

		delete this.data.transitions[trans_id];
	}

	set_transition_start(trans_id, state_id, rel_pos)
	{
		const t = this.data.transitions[trans_id];
		const conn = this.data.connectors[t.start];
		const s = this.data.states[state_id];

		const [rx, ry] = rel_pos;
		const [x, y] = s.pos;
		const [w, h] = s.size;

		let v = [0, 0];


		if ((rx === 0) && (ry > 0) && (ry < h)) //left
		{
			conn.side = 'left';
			conn.offset = ry;
			v = [x, y + ry];
		}
		else if ((rx === w) && (ry > 0) && (ry < h)) //right
		{
			conn.side = 'right';
			conn.offset = ry;
			v = [x + w, y + ry];
		}
		else if ((ry === 0) && (rx > 0) && (rx < w)) //top
		{
			conn.side = 'top';
			conn.offset = rx;
			v = [x + rx, y];
		}
		else if ((ry === h) && (rx > 0) && (rx < w)) //top
		{
			conn.side = 'bottom';
			conn.offset = rx;
			v = [x + rx, y + h];
		}
		else
		{
			return false;
		}

		t.vertices = [v, v, v];
		t.label_pos = v;
		
		conn.parent = state_id;
		s.connectors.push[conn];

		return true;
	}

	set_transition_endpoint(trans_id, pos)
	{
		const [x, y] = pos;
		const t = this.data.transitions[trans_id];

		t.vertices.splice(-2, 2); // remove the last two vertices
		const [lx, ly] = t.vertices[t.vertices.length - 1];

		t.vertices.push([x, ly]);
		t.vertices.push([x, y]);
	}

	make_new_state(init_pos)
	{
		const state_id = this.make_new_id(this.data.states, 'state_');
		this.data.states[state_id] = {
			pos: init_pos, 
			size: [15, 15],
			title: state_id,
			text: ['entry/', 'exit/'],
			connectors: [],
			parent: '__top__',
			children: [],
			};
		this.data.states['__top__'].children.push(state_id);

		return state_id;
	}

	ack_changes()
	{
		this.changes.states = [];
		this.changes.transitions = [];
	}

	make_point(x, y)
	{
		return new Point(x, y);
	}

	get_state_body_rect(state_id)
	{
		const s = this.data.states[state_id];
		const [x1, y1] = s.pos;
		const [w, h] = s.size;

		return new Rect(x1, y1, x1 + w, y1 + h);
	}

	is_in_state_body(state_id, point)
	{
		const r = this.get_state_body_rect(state_id);
		return point.is_inside_rect(r);
	}

	find_state_at(point, state_id='__top__')
	{
		const children = this.data.states[state_id].children;

		for (const c of children)
		{
			const s = this.find_state_at(point, c);
			if (s !== null)
			{
				return s;
			}
		}
		
		if (this.is_in_state_body(state_id, point))
		{
			return state_id;
		}

		return null;
	}

	get_substates(state_id)
	{
		const s = this.data.states[state_id];

		const substates = [...s.children];

		for (const id of s.children)
		{
			substates.push(...this.get_substates(id));
		}

		return substates;
	}

	/* Return all connectors in the state and in its substates */
	get_state_connectors(state_id)
	{
		const s = this.data.states[state_id];
		const conns = [...s.connectors];

		for (const id of s.children)
		{
			conns.push(...this.get_state_connectors(id));
		}

		return conns;
	}

	get_state_transitions(state_id)
	{
		const conns = this.get_state_connectors(state_id);
		const trs = new Set(conns.map(c => this.data.connectors[c].transition));
		const internal = [...trs].filter(
			(t) => {
				const tr = this.data.transitions[t];
				return conns.includes(tr.start) && conns.includes(tr.end);
			}) 
		const external = [...trs].filter(t => !internal.includes(t))

		return [internal, external];
	}

	move_transition(tr_id, delta)
	{
		const tr = this.data.transitions[tr_id];
		const [dx, dy] = delta;

		tr.vertices.map(v => {v[0] += dx; v[1] += dy});
		tr.label_pos[0] += dx;
		tr.label_pos[1] += dy;
	}


	move_state(state_id, pos)
	{
		const s = this.data.states[state_id];
		const [dx, dy] = [pos[0] - s.pos[0], pos[1] - s.pos[1]];
		this.data.states[state_id].pos = pos;

		const subs = this.get_substates(state_id);
		const conns = this.get_state_connectors(state_id);
		const [internal, external] = this.get_state_transitions(state_id);

		for (const id of subs)
		{
			const sub = this.data.states[id]; 
			const old_pos = sub.pos;
			sub.pos = [old_pos[0] + dx, old_pos[1] + dy];
		}

		internal.map(t => this.move_transition(t, [dx, dy]));
		external.map(this.update_transition_path, this);
		this.changes.states.push(state_id);
		this.changes.states.push(...subs);
		this.changes.transitions.push(...internal);
		this.changes.transitions.push(...external);
	}

	remove_child(parent_id, child_id)
	{
		const p = this.data.states[parent_id];
		p.children = p.children.filter(c => c !== child_id);
	}

	add_child(parent_id, child_id)
	{
		const p = this.data.states[parent_id];
		p.children.push(child_id);
	}

	set_parent(state_id, parent_id)
	{
		const s = this.data.states[state_id];

		if (parent_id != s.parent)
		{
			this.remove_child(s.parent, state_id);
			this.add_child(parent_id, state_id);
			s.parent = parent_id;
		}
	}

	update_parent(state_id)
	{
		const s = this.data.states[state_id];
		const parent = this.find_state_at(new Point(...s.pos));

		if (parent != s.parent)
		{
			this.set_parent(state_id, parent);
		}
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
		return this.data.states['__top__'].children;
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

	/* This function should select a suitable anchor point for the label.
	 * A good point is the leftmost vertex of the longest horizontal line.
	 */
	update_transition_label_anchor(tr_id, label_width)
	{
		const tr = this.get_transition(tr_id);
		const v = tr.vertices;

		let anchor = null;
		let len = -1;
		const anchors = [];

		for (let i = 0; i < v.length - 1; i++)
		{
			if (v[i][1] == v[i + 1][1]) // horizontal line
			{
				const l = Math.abs(v[i][0] - v[i + 1][0]);
				if (v[i][0] < v[i + 1][0])
				{
					anchors.push([i, l])
				}
				else
				{
					anchors.push([i + 1, l])
				}
			}
		}

		const middle = Math.round((anchors.length - 1) / 2);

		tr.label_anchor = anchors[middle][0];
	}

	update_transition_label_pos(tr_id)
	{
		const tr = this.get_transition(tr_id);
		this.update_transition_label_anchor(tr_id);
		const [ax, ay] = tr.vertices[tr.label_anchor];
		const [ox, oy] = tr.label_offset;
		tr.label_pos = [ax + ox, ay + oy];
	}

	transition_drag(tr_id, line_idx, p, label_width)
	{
		if (line_idx == null)
		{
			return;
		}

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

		this.update_transition_label_pos(tr_id, label_width);
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

		this.update_transition_label_pos(tr_id);
	}
}
