
export class Gui {
	constructor()
	{
		this.drawing = document.querySelector('#drawing');
		this.svg = document.querySelector('svg');
		this.paths = {};
		this.states = {};
		this.options = {
			text_height: 2,
			state_min_size: [5,5]
		};

	}

	get_absolute_pos(evt)
	{
		const CTM = this.drawing.getScreenCTM();
		const x = Math.round((evt.clientX - CTM.e) / CTM.a);
		const y = Math.round((evt.clientY - CTM.f) / CTM.d);

		return [x, y];
	}

	set_cursor(style)
	{
		this.svg.style.cursor = style;
	}

	get_state_rel_pos(evt, state_id)
	{
		const CTM = this.states[state_id].obj.getScreenCTM();
		const x = Math.round((evt.clientX - CTM.e) / CTM.a);
		const y = Math.round((evt.clientY - CTM.f) / CTM.d);

		return [x, y];
	}

	add_event_handler(event, handler)
	{
		this.svg.addEventListener(event, handler);
	}

	remove_event_handler(event, handler)
	{
		this.svg.removeEventListener(event, handler);
	}

	make_svg_elem(name, attrs)
	{
		const svgns = "http://www.w3.org/2000/svg";
		const el = document.createElementNS(svgns, name )
		for(var key in attrs) {
			el.setAttributeNS(null, key, attrs[key]);
		}

		return el;
	}

	modify_svg_elem(el, attrs)
	{
		for(var key in attrs) {
			el.setAttributeNS(null, key, attrs[key]);
		}
	}

	get_path_from_vertices(vertices)
	{
		const [sx, sy] = vertices[0];

		const path = [`M ${sx} ${sy}`];

		vertices.slice(1).map(([x, y]) => path.push(`L ${x} ${y}`));

		return path.join(' ');
	}

	get_arrow_transform_from_vertices(vertices)
	{
		const [[lx, ly], [ex, ey]] = vertices.slice(-2);

		let arrow_rot = '';

		if (lx === ex) // vertical
		{
			if (ly < ey) // pointing down
			{
				arrow_rot = 'rotate(-90) ';
			}
			else // pointing up
			{
				arrow_rot = 'rotate(90) ';
			}
		}
		else // horizontal
		{
			if (lx < ex) // pointing right
			{
				arrow_rot = 'rotate(180) ';
			}
			else // pointing left
			{
			}
		}

		return `translate(${ex} ${ey}) ${arrow_rot}`;
	}

	delete_transition(id)
	{
		this.drawing.removeChild(this.paths[id].obj);
		delete this.paths[id];
	}

	render_transition(id, vertices, label, label_pos, on_mousedown, on_dblclick, on_click)
	{
		const path = this.get_path_from_vertices(vertices);
		const arrow_transform = this.get_arrow_transform_from_vertices(vertices);

		const g = this.make_svg_elem('g', {});
		const p = this.make_svg_elem('path', {d: path, class: 'transition_line'});
		const p2 = this.make_svg_elem('path', {d: path, class: 'transition_handle'});
		const a = this.make_svg_elem('use', {href: '#arrow',
			transform: arrow_transform,
			class: 'transition_arrow',
			display: 'block'});
		const l = this.make_svg_elem('text', {x: label_pos[0], y: label_pos[1], class: 'transition_label'});
		l.textContent = label;

		g.appendChild(p);
		g.appendChild(a);
		g.appendChild(l);
		g.appendChild(p2);

		this.paths[id] = {
			obj: 	g,
			mod_svg: this.modify_svg_elem,
			gpfv: this.get_path_from_vertices,
			gatfv: this.get_arrow_transform_from_vertices,
			redraw: function(vertices, label, label_pos)
			{
				const npath = this.gpfv(vertices);
				const narrow_transform = this.gatfv(vertices);

				this.mod_svg(p, {d: npath});
				this.mod_svg(p2, {d: npath});
				this.mod_svg(a, {transform: narrow_transform});
				this.mod_svg(l, {x: label_pos[0], y: label_pos[1]});
				l.textContent = label;
			},
			get_label_bbox: function()
			{
				const bbox = l.getBBox();
				return [bbox.width, bbox.height];
			},
			add_handle_class: function(cl)
			{
					p2.classList.add(cl);
			},
			remove_handle_class: function(cl)
			{
					p2.classList.remove(cl);
			},
            change_trans_line_color: function(is_deleting)
            {
                if (is_deleting)
                {
                    this.mod_svg(p2, { class: 'transition_handle_deleting' })
                }
                else
                {
                    this.mod_svg(p2, { class: 'transition_handle' })
                }
            },
		}

		p2.addEventListener('mousedown', on_mousedown);
		p2.addEventListener('dblclick', on_dblclick);
		p2.addEventListener('click', on_click);

		this.drawing.prepend(g);
	}

	get_path_label_size(id)
	{
		return this.paths[id].get_label_bbox();
	}

	redraw_path_with_arrow(id, vertices, label, label_pos)
	{
		this.paths[id].redraw(vertices, label, label_pos);
	}

	redraw_path_change_line_color(id, is_deleting)
	{
		this.paths[id].change_trans_line_color(is_deleting);
    }

    render_state(id, title, pos, size, strings, text_height, on_drag_start, on_resize_start, on_border_click, on_mouse_over, on_mouse_leave)
	{
		const [x, y] = pos;
		const [w, h] = size;
		const th = text_height;

		const g = this.make_svg_elem('g', {transform: `translate(${x}, ${y})`})
		const r = this.make_svg_elem('rect',
			{x: 0, y: 0, width: w, height: h, class: 'state_body'});
		const r2 = this.make_svg_elem('rect',
			{x: 0, y: 0, width: w, height: h, class: 'state_border'});
		r2.addEventListener('click', on_border_click);

		const s1 = this.make_svg_elem('line',
			{x1: 0, y1: th, x2: w, y2: th, class: 'state_separator'});
		const t = this.make_svg_elem('text', {x: th*0.3, y: th*0.8, class: 'state_title'});
		t.textContent = title;

		const text = this.make_svg_elem('text', {x: th*0.3, y: th*0.8, class: 'state_text'});
		for (const s of strings)
		{
			const tspan = this.make_svg_elem('tspan', {x: th*0.3, dy: th});
			tspan.textContent = s;
			text.appendChild(tspan);
		}

		const m = this.make_svg_elem('line',
			{x1: w-1, y1: h, x2: w, y2: h-1, class: 'state_resize_mark'});

		const offset = th + th*strings.length;
		const s2 = this.make_svg_elem('line',
			{x1: 0, y1: offset, x2: w, y2: offset, class: 'state_separator'});

		const drag_handle = this.make_svg_elem('rect',
			{x: 0, y: 0, width: w, height: offset, class: 'state_drag_handle'});
		drag_handle.addEventListener('mousedown', on_drag_start);

		const resize_handle = this.make_svg_elem('circle',
			{cx: w-0.25, cy: h-0.25, class: 'state_resize_handle'});
		resize_handle.addEventListener('mousedown', on_resize_start)

		g.appendChild(r);
		g.appendChild(s1);
		g.appendChild(t);
		g.appendChild(text);
		g.appendChild(s2);
		g.appendChild(m);
		g.appendChild(drag_handle);
		g.appendChild(r2);
		g.appendChild(resize_handle);

		this.states[id] = {
			obj: g,
			mod_svg: this.modify_svg_elem,
			make_svg: this.make_svg_elem,
			resize: function(size)
			{
				const [w, h] = size;
				this.mod_svg(r, {width: w, height: h});
				this.mod_svg(r2, {width: w, height: h});
				this.mod_svg(s1, {x2: w});
				this.mod_svg(s2, {x2: w});
				this.mod_svg(m, {x1: w-1, y1: h, x2: w, y2: h-1});
				this.mod_svg(drag_handle, {width: w});
				this.mod_svg(resize_handle, {cx: w-0.25, cy: h-0.25});
			},
			move: function(pos)
			{
				this.mod_svg(g, {transform: `translate(${pos[0]}, ${pos[1]})`});
			},
			set_text(txt)
			{
				if (txt.length > text.childElementCount)
				{
					while (text.childElementCount < txt.length)
					{
						const tspan = this.make_svg('tspan', {x: th*0.3, dy: th});
						text.appendChild(tspan);
					}
				}
				else if (txt.length < text.childElementCount)
				{
					while (text.childElementCount > txt.length)
					{
						text.removeChild(text.lastChild);
					  }
				}

				const offset = th + th*txt.length;
				this.mod_svg(s2, {y1: offset, y2: offset});
				this.mod_svg(drag_handle, {height: offset});

				for (const [i, s] of txt.entries())
				{
					text.children[i].textContent = s;
				}
			}
		};

		this.drawing.appendChild(g);
	}
};

