import {Rect, Point} from './geometry.js';

export class Model {
	constructor(data)
	{
		this.data = 			data;

		this.options = {
			state_min_width: 	5,
			state_min_height: 	3,
			text_height: 		2,
			label_offset:		[0.5, -0.4],
		};

		this.elbow = 			'v';

		this.state_text_cache = {};
		this.tr_text_cache = {};

		this.history = [];
		this.history_idx = 1;

		this.ack_changes();
		this.save_state(null);

		this.selection = new Set();

		if (!('notes' in this.data))
		{
			this.data['notes'] = {};
		}

		if (!('view' in this.data))
		{
			this.data['view'] = {
				translate: [10, 10],
				scale: 10
			};
		}
	}

	select_state(state_id)
	{
		if (!this.selection.delete(state_id))
		{
			this.selection.add(state_id);
		}
	}

	drop_selection()
	{
		this.selection.clear();
	}

	save_state(view)
	{
		const state = [JSON.stringify(this.data), view];

		if (this.history_idx > 1)
		{
			this.history.splice(-(this.history_idx-1));
			this.history_idx = 1;
		}

		if (this.history.length === 0)
		{
			this.history.push(state);
			return true;
		}
		else if (this.history[this.history.length - 1][0] !== state[0])
		{
			this.history.push(state);
			return true;
		}

		return false;
	}

	get_view()
	{
		return this.data.view;
	}

	set_view(view)
	{
		this.data.view = view;
	}

	undo()
	{
		if (this.history_idx < this.history.length)
		{
			this.history_idx++;
			const [data_str, view] = this.history[this.history.length - this.history_idx];
			this.data = JSON.parse(data_str);

			return view;
		}

		return null;
	}

	redo()
	{
		if (this.history_idx > 1)
		{
			this.history_idx--;
			const [data_str, view] = this.history[this.history.length - this.history_idx];
			this.data = JSON.parse(data_str);

			return view;
		}

		return null;
	}

	get_data_string()
	{
		return JSON.stringify(this.data, null, 4);
	}

	ack_changes()
	{
		this.changes = {
			trans_new:			[],
			trans_del:			[],
			trans_redraw:		[],
			trans_set_label:	[],
			trans_set_label_pos:[],
			state_new:			[],
			state_del:			[],
			state_resize:		[],
			state_move:			[],
			state_set_text:		[],
			state_set_title:	[],
		};
	}

	get_note_text(obj_id)
	{
		if (!(obj_id in this.data.notes))
		{
			this.data.notes[obj_id] = '';
		}

		return this.data.notes[obj_id];
	}

	get_transition_text(tr_id)
	{
		if (!(tr_id in this.tr_text_cache))
		{
			const t = this.data.transitions[tr_id];
			this.tr_text_cache[tr_id] = t.label;
		}

		return this.tr_text_cache[tr_id];
	}

	cache_transition_label(tr_id, label)
	{
		this.tr_text_cache[tr_id] = label;
	}

	reset_transition_label(tr_id)
	{
		const t = this.data.transitions[tr_id];
		this.tr_text_cache[tr_id] = t.label;
		return this.tr_text_cache[tr_id];
	}

	apply_transition_label(tr_id, label)
	{
		this.tr_text_cache[tr_id] = label;
		const t = this.data.transitions[tr_id];
		t.label = label;
		this.changes.trans_set_label.push([tr_id, t]);
	}

	compare_str_array(a, b)
	{
		var i = a.length;

		while (i--)
		{
			if (a[i] !== b[i]) return false;
		}

		return true
	}

	chop_text(text)
	{
		const parts = text.split(/(\w+\(\))|(\w+)/)

		const result = []

		for (let p of parts)
		{
			if ((p != undefined) && (p !== ''))
			{
				const note = this.data.notes[p] ? this.data.notes[p] : '';
				result.push([p, p.match(/\w+/) != null, note]);
			}
		}

		return result;
	}

	get_state_text(state_id)
	{
		if (!(state_id in this.state_text_cache))
		{
			const s = this.data.states[state_id];
			this.state_text_cache[state_id] = {title: s.title, text: s.text.join('\n')};
		}

		return this.state_text_cache[state_id];
	}

	cache_state_text(state_id, text)
	{
		this.state_text_cache[state_id] = text;
	}

	cache_note_text(obj_id, text)
	{
		this.data.notes[obj_id] = text;

		for (const [tr_id, tr] of Object.entries(this.data.transitions))
		{
			if (tr.label.includes(obj_id))
			{
				this.changes.trans_set_label.push([tr_id, tr]);
			}
		}

		for (const [s_id, s] of Object.entries(this.data.states))
		{
			for (const t of s.text)
			{
				if (t.includes(obj_id))
				{
					this.changes.state_set_text.push([s_id, s]);
					break;
				}
			}
		}
	}

	apply_state_text(state_id, text)
	{
		this.state_text_cache[state_id].text = text;
		const s = this.data.states[state_id];
		this.set_state_text(state_id, text.split('\n'))
	}

	apply_state_title(state_id, title)
	{
		this.state_text_cache[state_id].title = title;
		const s = this.data.states[state_id];
		s.title = title;
		this.changes.state_set_title.push([state_id, s]);
	}

	reset_state_title(state_id)
	{
		const s = this.data.states[state_id];
		this.state_text_cache[state_id].title = s.title;
		return this.state_text_cache[state_id];
	}

	reset_state_text(state_id)
	{
		const s = this.data.states[state_id];
		this.state_text_cache[state_id].text = s.text.join('\n');
		return this.state_text_cache[state_id];
	}

	delete_state(state_id)
	{
		const s = this.data.states[state_id];
		const parent = this.data.states[s.parent];

		const del_states = [state_id].concat(this.get_substates(state_id));
		const [alltransint, alltransext] = this.get_state_transitions(state_id);
	
		if (parent)
		{
			parent.children = parent.children.filter(ch => ch != state_id);
		}

		alltransint.map(ti => this.delete_transition(ti));
		alltransext.map(ti => this.delete_transition(ti));

		del_states.map(s_id => delete this.data.states[s_id], this);

		del_states.map(s_id => this.changes.state_del.push([s_id, null]));
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
			label_offset: [...this.options.label_offset],
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

	detach_connector_from_state(conn_id)
	{
		const conn = this.data.connectors[conn_id];

		// Remove reference from owner state
		if (conn.parent in this.data.states)
		{
			const s = this.data.states[conn.parent];
			s.connectors = s.connectors.filter(id => id != conn_id);
		}
	}

	delete_connector(conn_id)
	{
		if (!(conn_id in this.data.connectors))
			return;

		const conn = this.data.connectors[conn_id];
		
		this.detach_connector_from_state(conn_id);
	
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

		this.changes.trans_del.push([trans_id, t]);
	}

	attach_connector_to_state(conn_id, state_id, rel_pos)
	{
		const conn = this.data.connectors[conn_id];
		const s = this.data.states[state_id];
		const t = this.data.transitions[conn.transition];
		
		const [rx, ry] = rel_pos;
		const [x, y] = s.pos;
		const [w, h] = s.size;
		
		let v = [0, 0];

		if (s.type == 'initial')
		{
			if (s.connectors.length !== 0)
			{
				return [];
			}
			conn.side = 'all';
			conn.offset = 0;
			v = [x, y];
			this.elbow = 'v';
		}
		else if ((rx === 0) && (ry > 0) && (ry < h)) //left
		{
			conn.side = 'left';
			conn.offset = ry;
			v = [x, y + ry];
			this.elbow = 'v';
		}
		else if ((rx === w) && (ry > 0) && (ry < h)) //right
		{
			conn.side = 'right';
			conn.offset = ry;
			v = [x + w, y + ry];
			this.elbow = 'v';
		}
		else if ((ry === 0) && (rx > 0) && (rx < w)) //top
		{
			conn.side = 'top';
			conn.offset = rx;
			v = [x + rx, y];
			this.elbow = 'h';
		}
		else if ((ry === h) && (rx > 0) && (rx < w)) //top
		{
			conn.side = 'bottom';
			conn.offset = rx;
			v = [x + rx, y + h];
			this.elbow = 'h';
		}
		else
		{
			return [];
		}

		if (t.vertices.length > 1)
		{
			const [[ax, ay], [bx, by]] = t.vertices.slice(-2);
			if ((ax === bx) && (ay === by))
			{
				return [];
			}
			else if ((this.elbow === 'v') && (ay !== by))
			{
				return [];
			}
			else if ((this.elbow === 'h') && (ax !== bx))
			{
				return [];
			}
		}
		
		conn.parent = state_id;
		s.connectors.push(conn_id);

		return v;
	}

	set_transition_start(state_id, rel_pos)
	{
		const trans_id = this.make_new_transition();
		const t = this.data.transitions[trans_id];
		const v = this.attach_connector_to_state(t.start, state_id, rel_pos);

		if (v.length === 0)
		{
			this.delete_transition(trans_id);
			return '';
		}

		t.vertices = [[...v], [...v], [...v]];
		t.label_pos = [v[0] + t.label_offset[0], v[1] + t.label_offset[1]];
		this.changes.trans_new.push([trans_id, t]);
		return trans_id;
	}

	set_transition_end(trans_id, state_id, rel_pos)
	{
		const t = this.data.transitions[trans_id];

		const start_state = this.data.states[this.data.connectors[t.start].parent];
		
		// Make sure that an initial transition connects siblings
		if (start_state.type === 'initial')
		{
			const end_state = this.data.states[state_id];
			if (start_state.parent !== end_state.parent)
			{
				return false; 
			}
		}

		const s = this.data.states[state_id];
		if (s.type === 'initial')
		{
			return false;
		}

		// remove the last two vertex from the array
		const [[ax, ay], [bx, by]] = t.vertices.splice(-2);

		if (t.vertices.length > 2)
		{
			const [[cx, cy], [dx, dy]] = t.vertices.slice(-2);
			// if the last two of the remaining array are in line with a:
			if (((ax === cx) && (cx === dx)) || ((ay === cy) && (cy === dy)))
			{
				t.vertices.splice(-1);
			}
		}
			
		t.vertices.push([ax, ay]);

		if ((ax !== bx) || (ay !== by))
		{
			t.vertices.push([bx, by])
		}
		

		// Handle the case where there are only 2 vertices left.
		if (t.vertices.length == 2)
		{
			const [[x1, y1], [x2, y2]] = t.vertices.slice(-2);
			const m = [Math.round((x1 + x2) / 2), Math.round((y1 + y2) / 2)];
			t.vertices.splice(1, 0, [...m]);
			t.vertices.splice(1, 0, [...m]);
		}

		const v = this.attach_connector_to_state(t.end, state_id, rel_pos);
		if (v.length == 2)
		{
			this.changes.trans_redraw.push([trans_id, t]);
			return true;
		}

		this.changes.trans_redraw.push([trans_id, t]);
		return false;
	}

	set_transition_endpoint(trans_id, pos)
	{
		const [x, y] = pos;
		const t = this.data.transitions[trans_id];

		t.vertices.splice(-2, 2); // remove the last two vertices
		const [lx, ly] = t.vertices[t.vertices.length - 1];

		const v = (this.elbow == 'v') ? [x, ly] : [lx, y];
		t.vertices.push(v);
		t.vertices.push([x, y]);

		this.changes.trans_redraw.push([trans_id, t]);

		this.reset_transition_label_pos(trans_id);
	}

	switch_transition_elbow(trans_id, pos)
	{
		const t = this.data.transitions[trans_id];
		if ((t.vertices.length > 3) || (this.data.connectors[t.start].side === 'all'))
		{
			this.elbow = (this.elbow == 'v') ? 'h' : 'v';
		}
		this.set_transition_endpoint(trans_id, pos);

		this.changes.trans_redraw.push([trans_id, t]);
	}

	add_transition_vertex(trans_id)
	{
		const t = this.data.transitions[trans_id];

		// remove the last two vertex from the array
		const [[ax, ay], [bx, by]] = t.vertices.splice(-2);
		
		if (t.vertices.length < 2)
		{
			// Make sure that the first line in the transition path has non-zero length.
			const [dx, dy] = t.vertices[0];
			if ((ax !== dx) || (ay !== dy))
			{
				t.vertices.push([ax, ay]);
				this.elbow = (this.elbow == 'v') ? 'h' : 'v';
			}
		}
		else
		{
			const [[cx, cy], [dx, dy]] = t.vertices.slice(-2);

			// Remove unnecessary vertices that are in line with the adjacent points
			if (((ax === cx) && (cx === dx)) || ((ay === cy) && (cy === dy)))
			{
				t.vertices.splice(-1);
			}
			
			t.vertices.push([ax, ay]);
			this.elbow = (this.elbow == 'v') ? 'h' : 'v';
		}

		t.vertices.push([ax, ay]);
		t.vertices.push([bx, by]);

		this.changes.trans_redraw.push([trans_id, t]);
	}

	remove_transition_vertex(trans_id, pos)
	{
		const t = this.data.transitions[trans_id];
		if (t.vertices.length > 3)
		{
			t.vertices.splice(-1, 1);
			this.elbow = (this.elbow == 'v') ? 'h' : 'v';
			this.set_transition_endpoint(trans_id, pos);

			this.changes.trans_redraw.push([trans_id, t]);
		}
	}

	transition_split(trans_id, pos)
	{
		const line_idx = this.transition_get_line_index(trans_id, pos);

		if (line_idx == null) return;

		const t = this.data.transitions[trans_id];
		const [[x1, y1], [x2, y2]] = t.vertices.slice(line_idx, line_idx+2);
		const m = [Math.round((x1 + x2) / 2), Math.round((y1 + y2) / 2)];
		t.vertices.splice(line_idx+1, 0, [...m], [...m]);

		this.changes.trans_redraw.push([trans_id, t]);
	}

	transition_restart_from_pos(trans_id, pos)
	{
		const line_idx = this.transition_get_line_index(trans_id, pos);

		if (line_idx == null) return false;

		const t = this.data.transitions[trans_id];

		this.detach_connector_from_state(t.end);

		t.vertices.splice(line_idx+1);
		const [l] = t.vertices.slice(-1);
		t.vertices.push(l, l);

		const v = t.vertices[0];

		t.label_pos = [v[0] + t.label_offset[0], v[1] + t.label_offset[1]];

		this.changes.trans_redraw.push([trans_id, t]);
	}

	make_new_state(init_pos, id_prefix='state_', state_type='normal', init_size=[15, 15])
	{
		const state_id = this.make_new_id(this.data.states, id_prefix);
		const s = {
			pos: init_pos, 
			size: init_size,
			title: state_id,
			text: ['entry/', 'exit/'],
			connectors: [],
			parent: null,
			children: [],
			type: state_type,
			};
		this.data.states[state_id] = s;

		this.update_parent(state_id);

		this.changes.state_new.push([state_id, s]);

		return state_id;
	}

	make_new_initial_state(init_pos)
	{
		return this.make_new_state(init_pos, 'istate_', 'initial', [2, 2]);
	}

	update_state_transitions(state_id)
	{
		const s = this.data.states[state_id];
		const trs = s.connectors.map(c => this.data.connectors[c].transition, this);
		trs.map(this.update_transition_path, this);
	}

	set_state_text(state_id, text)
	{
		const s = this.data.states[state_id];
		const h_diff = (text.length - s.text.length) * this.options.text_height;
		s.text = text;

		if (h_diff !== 0)
		{
			const [w, h] = s.size;
			this.resize_state(state_id, [w, h + h_diff]);
			this.move_substates(state_id, 0, h_diff);
			this.update_state_transitions(state_id);
		}

		this.changes.state_set_text.push([state_id, s]);
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
		const txt_offset = this.options.text_height * (1 + s.text.length);

		return new Rect(x1, y1 + txt_offset, x1 + w, y1 + h);
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

	get_selection_substates()
	{
		const substates = new Set();

		for (const state_id of this.selection)
		{
			this.get_substates(state_id).map(e => substates.add(e));
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

	get_single_state_transitions(state_id)
	{
		const s = this.data.states[state_id];
		const conns = [...s.connectors];

		const trs = new Set(conns.map(c => this.data.connectors[c].transition));
		const internal = [...trs].filter(
			(t) => {
				const tr = this.data.transitions[t];
				return conns.includes(tr.start) && conns.includes(tr.end);
			}) 
		const external = [...trs].filter(t => !internal.includes(t))

		return [internal, external];
	}

	get_selection_transitions()
	{
		const conns = [];
		for (const state_id of this.selection)
		{
			const cns = this.get_state_connectors(state_id);
			conns.push(...cns);
		}

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

		this.changes.trans_redraw.push([tr_id, tr]);
	}

	move_substates(state_id, dx, dy)
	{
		const subs = this.get_substates(state_id);
		const [internal, external] = this.get_state_transitions(state_id);

		for (const id of subs)
		{
			const sub = this.data.states[id]; 
			const old_pos = sub.pos;
			sub.pos = [old_pos[0] + dx, old_pos[1] + dy];
			this.changes.state_move.push([id, sub]);
		}

		internal.map(t => this.move_transition(t, [dx, dy]));
		external.map(this.update_transition_path, this);
	}

	move_selection(dx, dy)
	{
		const states = this.get_selection_substates();
		for (const state_id of this.selection)
		{
			states.add(state_id);
		}
		const [internal, external] = this.get_selection_transitions();

		for (const id of states)
		{
			const sub = this.data.states[id]; 
			const old_pos = sub.pos;
			sub.pos = [old_pos[0] + dx, old_pos[1] + dy];
			this.changes.state_move.push([id, sub]);
		}

		internal.map(t => this.move_transition(t, [dx, dy]));
		external.map(this.update_transition_path, this);
	}

	move_state(state_id, pos, move_substates)
	{
		const s = this.data.states[state_id];
		const [dx, dy] = [pos[0] - s.pos[0], pos[1] - s.pos[1]];

		if (move_substates)
		{
			if (this.selection.has(state_id))
			{
				this.move_selection(dx, dy);
			}
			else
			{
				s.pos = pos;
				this.move_substates(state_id, dx, dy);
				this.changes.state_move.push([state_id, s]);
			}
		}
		else
		{
			s.pos = pos;
			this.changes.state_move.push([state_id, s]);

			const [internal, external] = this.get_single_state_transitions(state_id);

			internal.map(t => this.move_transition(t, [dx, dy]));
			external.map(this.update_transition_path, this);
		}
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
			if (s.parent !== null)
			{
				this.remove_child(s.parent, state_id);
			}
			this.add_child(parent_id, state_id);
			s.parent = parent_id;
		}
	}

	get_initial_state(state_id)
	{
		const s = this.data.states[state_id];

		const i_list = s.children.filter(c_id => this.data.states[c_id].type === 'initial');
		if (i_list.length !== 0)
		{
			return i_list[0];
		}
		
		return null;
	}

	update_parents()
	{
		for (const id of Object.keys(this.data.states))
		{
			this.update_parent(id);
		}
	}

	update_parent(state_id)
	{
		const s = this.data.states[state_id];
		const parent_id = this.find_state_at(new Point(...s.pos));
		

		if (parent_id != s.parent)
		{
			if ((s.type === 'initial') && 
				((s.connectors.length !== 0) || (this.get_initial_state(parent_id) !== null)))
			{
				this.delete_state(state_id);
			}
			else
			{
				this.set_parent(state_id, parent_id);
			}
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

		this.changes.state_resize.push([state_id, state]);

		state.connectors.map(c => this.update_transition_path(this.data.connectors[c].transition), this);
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

			case 'all':
				break;
		}
	}

	/* It is very easy to update the transition label: we just simply try to drag it
	 * to its current position. The function will find the best place.
	 */
	update_transition_label_offset(tr_id, label_width)
	{
		const tr = this.get_transition(tr_id);

		const [lx, ly] = [...tr.label_pos];

		this.transition_label_drag(tr_id, [lx-this.options.label_offset[0], ly-this.options.label_offset[1]]);
	}

	update_transition_label_pos(tr_id)
	{
		const tr = this.get_transition(tr_id);
		this.update_transition_label_offset(tr_id);
		const [ax, ay] = tr.vertices[tr.label_anchor];
		const [ox, oy] = tr.label_offset;
		tr.label_pos = [ax + ox, ay + oy];

		this.changes.trans_redraw.push([tr_id, tr]);
	}

	transition_midpoint(tr_id)
	{
		const tr = this.get_transition(tr_id);

		const len = [];
		for (let i=0; i<tr.vertices.length - 1; i++)
		{
			// Assuming horizontal and vertical lines
			len.push(Math.abs(tr.vertices[i][0] - tr.vertices[i+1][0]) + Math.abs(tr.vertices[i][1] - tr.vertices[i+1][1]));
		}

		const half_len = len.reduce((a, b) => a + b) / 2;

		let sum = 0;
		for (let i=0; i<len.length; i++)
		{
			const diff = half_len - sum;
			
			if (diff < len[i])
			{
				const t = diff / len[i];
				const dx = (tr.vertices[i+1][0] - tr.vertices[i][0]) * t;
				const dy = (tr.vertices[i+1][1] - tr.vertices[i][1]) * t;
				return [tr.vertices[i][0] + dx, tr.vertices[i][1] + dy];
			}
			else
			{
				sum += len[i];
			}
		}

		// Should reach this point
	}

	reset_transition_label_pos(tr_id)
	{
		const p = this.transition_midpoint(tr_id);
		this.transition_label_drag(tr_id, p);
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

		if (line_idx == 0) // first segment
		{
			const [xmin, xmax, ymin, ymax] = this.get_connector_movement_limits(tr.start);
			if (l1[0] === l2[0]) // vertical line 
			{
				l1[0] = Math.min(xmax, Math.max(x, xmin));
				l2[0] = l1[0];
			}
			else // horizontal line
			{
				l1[1] = Math.min(ymax, Math.max(y, ymin));
				l2[1] = l1[1];
			}

			this.move_connector(tr.start, l1);
		}
		else if (line_idx == (tr.vertices.length - 2)) // last segment
		{
			const [xmin, xmax, ymin, ymax] = this.get_connector_movement_limits(tr.end);
			if (l1[0] === l2[0]) // vertical line
			{
				l1[0] = Math.min(xmax, Math.max(x, xmin));
				l2[0] = l1[0];
			}
			else
			{
				l1[1] = Math.min(ymax, Math.max(y, ymin));
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

		this.changes.trans_redraw.push([tr_id, tr]);
	}

	clamp(num, min, max)
	{
		return Math.min(Math.max(num, min), max);
	};

	// Calculate the closest point to P that is on the line defined by A and B
	get_closest_point(A, B, P)
	{
		const a_to_p = [P[0] - A[0], P[1] - A[1]]; // vector A --> P
		const a_to_b = [B[0] - A[0], B[1] - A[1]]; // vector A --> B

		const atb2 = a_to_b[0]**2 + a_to_b[1]**2;

		const atp_dot_atb = a_to_p[0]*a_to_b[0] + a_to_p[1]*a_to_b[1];

		const t = atp_dot_atb / atb2;

		const t2 = this.clamp(t, 0.0, 1.0); // keep the point between A and B

		const c = [A[0] + a_to_b[0]*t2, A[1] + a_to_b[1]*t2]; // calculate the closest point

		const c_to_p = [P[0] - c[0], P[1] - c[1]];
		const ctp2 = c_to_p[0]**2 + c_to_p[1]**2; // distance of the point from the line

		return [c, ctp2]; // return the point and the squared distance of that point from P
	}

	transition_label_drag(tr_id, p)
	{
		const tr = this.data.transitions[tr_id];

		const len = tr.vertices.length - 1;

		let closest_p = [...tr.vertices[0]];
		let d_min = 9999999;
		let anchor_idx = 0;

		for (let i=0; i<len; i++)
		{
			if ((tr.vertices[i][0] !== tr.vertices[i+1][0]) || (tr.vertices[i][1] !== tr.vertices[i+1][1]))
			{
				const [a, d] = this.get_closest_point(tr.vertices[i], tr.vertices[i+1], p);

				if (Math.abs(d) < Math.abs(d_min))
				{
					d_min = d;
					closest_p = [...a];
					anchor_idx = i;
				}
			}
		}

		tr.label_anchor = anchor_idx;
		tr.label_pos = [closest_p[0] + this.options.label_offset[0], closest_p[1] + this.options.label_offset[1]];
		tr.label_offset = [tr.label_pos[0] - tr.vertices[anchor_idx][0] , tr.label_pos[1] - tr.vertices[anchor_idx][1]];

		this.changes.trans_set_label_pos.push([tr_id, tr.label_pos]);
	}

	simplify_tr_path(tr_id)
	{
		const t = this.data.transitions[tr_id];

		if (t.vertices.length < 6) return;

		t.vertices = t.vertices.filter((v, i, a) => {
			if (i === 0) return true;
			if (i === (a.length - 1)) return true;
			if ((v[0] === a[i-1][0]) && (v[0] === a[i+1][0])) return false;
			if ((v[1] === a[i-1][1]) && (v[1] === a[i+1][1])) return false; 

			return true;
		});

		this.changes.trans_redraw.push([tr_id, t]);
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

			case 'all':
				return [...parent.pos];
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
				return [x + 1, x + w - 1, y, y];

			case 'left':
			case 'right':
				return [x, x, y + 1, y + h - 1];

			case 'all':
				return [x, x, y, y];
		}
	}

	update_transition_path(tr_id)
	{
		const tr = this.data.transitions[tr_id];
		const [sx, sy] = this.get_connector_pos(tr.start);
		const [ex, ey] = this.get_connector_pos(tr.end);

		const [x0, y0] = tr.vertices[0];
		const [x1, y1] = tr.vertices[1];
		const [xe, ye] = tr.vertices.slice(-1);

		// Check if the start connector has moved
		if ((sx !== x0) || (sy !== y0))
		{
			tr.vertices[0] = [sx, sy];

			/*if (['left', 'right'].includes(this.data.connectors[tr.start].side))
			{
				tr.vertices[1][1] = sy;
			}
			else
			{
				tr.vertices[1][0] = sx;
			}*/

			switch (this.data.connectors[tr.start].side)
			{
				case 'left':
				case 'right':
					tr.vertices[1][1] = sy;
					break;
					
				case 'top':
				case 'bottom':
					tr.vertices[1][0] = sx;
					break;
				
				case 'all':
					tr.vertices[1][0] = sx;
					tr.vertices[1][1] = sy;
					break;
			}
		}

		// Check if the end connector has moved
		if ((ex !== xe) || (ey !== ye))
		{
			tr.vertices[tr.vertices.length-1] = [ex, ey];

			const v_1 = tr.vertices[tr.vertices.length-2];

			if (['left', 'right'].includes(this.data.connectors[tr.end].side))
			{
				v_1[1] = ey;
			}
			else
			{
				v_1[0] = ex;
			}
		}

		this.update_transition_label_pos(tr_id);
		this.changes.trans_redraw.push([tr_id, tr]);
	}
}
