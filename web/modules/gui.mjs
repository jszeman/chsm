
export class Gui {
	constructor(view)
	{
		this.drawing = document.querySelector('#drawing');
		this.svg = document.querySelector('svg');
		this.paths = {};
		this.states = {};
		this.options = {
			text_height: 2,
			state_min_size: [5,5]
		};

		this.view = {
			translate: [10, 10],
			scale: 10
		};

		this.zoom_speed = 0.5;
		this.pan_speed = 20;
	}

	clear()
	{
		Object.keys(this.states).map(this.delete_state, this);
		Object.keys(this.paths).map(this.delete_transition, this);

		this.reset_view();
	}

	reset_view()
	{
		this.view.translate = [10, 10];
		this.view.scale = 10;
		this.zoom_speed = 0.5;

		this.set_transform();
	}

	set_transform()
	{
		const [tx, ty] = this.view.translate;
		const s = this.view.scale; 
		this.drawing.attributes.transform.value = `translate(${tx}, ${ty}) scale(${s}, ${s})`;
	}

	get_view()
	{
		return {
			translate: [...this.view.translate],
			scale: this.view.scale
		};
	}

	set_view(view)
	{
		this.view.translate = view.translate;
		this.view.scale = view.scale;
		this.set_transform();
	}

	pan_up()
	{
		this.view.translate[1] += this.pan_speed;
		this.set_transform();
	}

	pan_down()
	{
		this.view.translate[1] -= this.pan_speed;
		this.set_transform();
	}

	pan_right()
	{
		this.view.translate[0] += this.pan_speed;
		this.set_transform();
	}

	pan_left()
	{
		this.view.translate[0] -= this.pan_speed;
		this.set_transform();
	}

	zoom_in(pos)
	{
		const zs = this.zoom_speed;
		const [x, y] = pos;
		const [dtx, dty] = [-zs * x, -zs * y];
		const [tx, ty] = this.view.translate;
		this.view.translate = [tx + dtx, ty + dty];
		this.view.scale += zs;
		this.set_transform();
	}

	zoom_out(pos)
	{
		if (this.view.scale < 2)
		{
			return;
		}
		const zs = -this.zoom_speed;
		const [x, y] = pos;
		const [dtx, dty] = [-zs * x, -zs * y];
		const [tx, ty] = this.view.translate;
		this.view.translate = [tx + dtx, ty + dty];
		this.view.scale += zs;
		this.set_transform();
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
		return this.states[state_id].get_rel_pos(evt);
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
		if (id in this.paths)
		{
			this.drawing.removeChild(this.paths[id].obj);
			delete this.paths[id];
		}
	}

	delete_state(id)
	{
		this.drawing.removeChild(this.states[id].obj);
		delete this.states[id];
	}

	build_text(parent, label, on_txt_click)
	{
		for (const [value, span, note] of label)
		{
			if (span)
			{
				const tspan = this.make_svg_elem('tspan', {class: 'function'});
				tspan.appendChild(document.createTextNode(value));
				if (note)
				{
					const title = this.make_svg_elem('title');
					title.textContent = note;
					tspan.appendChild(title);
				}
				parent.appendChild(tspan);
				tspan.addEventListener('click', on_txt_click);
			}
			else
			{
				const txt = document.createTextNode(value);
				parent.appendChild(txt);
			}
		}
	}

	render_transition(params)
	{
		const {id, vertices, label, label_pos, on_mousedown, on_dblclick, on_click, on_label_mousedown, on_txt_click} = params;

		if (id in this.paths) return;
		
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

		this.build_text(l, label, on_txt_click);

		g.appendChild(p);
		g.appendChild(a);
		g.appendChild(l);
		g.appendChild(p2);

		this.paths[id] = {
			obj: 	g,
			mod_svg: this.modify_svg_elem,
			make_svg_elem: this.make_svg_elem,
			build_txt: this.build_text,
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

				//l.textContent = label;
			},
			set_label(label)
			{
				l.innerHTML = "";

				this.build_txt(l, label, on_txt_click);
			},
			set_label_pos(label_pos)
			{
				this.mod_svg(l, {x: label_pos[0], y: label_pos[1]});
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
			}
		}

		p2.addEventListener('mousedown', on_mousedown);
		p2.addEventListener('dblclick', on_dblclick);
		p2.addEventListener('click', on_click);
		l.addEventListener('mousedown', on_label_mousedown);

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
		this.paths[id].remove_handle_class(is_deleting ? 'transition_handle' : 'transition_handle_deleting');
		this.paths[id].add_handle_class(is_deleting ? 'transition_handle_deleting' : 'transition_handle');
	}
	
	render_initial_state(params)
	{
		const {id,
			pos,
			on_header_mouse_down,
			on_header_click,
			on_border_click,
			on_header_mouse_over,
			on_header_mouse_leave} = params;

		const [x, y] = pos;

		const initial = this.make_svg_elem('circle',
			{cx: x, cy: y, class: 'initial_state'});

		initial.addEventListener('click', on_border_click);
		initial.addEventListener('mousedown', on_header_mouse_down);
		initial.addEventListener('mouseover', on_header_mouse_over);
		initial.addEventListener('mouseleave', on_header_mouse_leave);

		this.states[id] = {
			obj: initial,
			mod_svg: this.modify_svg_elem,
			make_svg: this.make_svg_elem,
			resize: function(size) {},
			move: function(pos)
			{
				this.mod_svg(initial, {cx: pos[0], cy: pos[1]});
			},
			get_rel_pos: function(evt)
			{
				return [0, 0];
			},
			add_border_class(cl) {},
			remove_border_class(cl) {},
			set_text(txt) {},
			set_title(title) {}
		};

		this.drawing.appendChild(initial);
	}

	get_obj_rel_pos(evt, obj)
	{
		const CTM = obj.getScreenCTM();
		const x = Math.round((evt.clientX - CTM.e) / CTM.a);
		const y = Math.round((evt.clientY - CTM.f) / CTM.d);

		return [x, y];
	}

	render_state(params)
	{
		if (params.type == 'initial')
		{
			this.render_initial_state(params);
			return;
		}

		const {id,
			title,
			pos,
			size,
			strings,
			type,
			text_height,
			on_header_mouse_down,
			on_header_click,
			on_corner_mouse_down,
			on_border_click,
			on_header_mouse_over,
			on_header_mouse_leave,
			on_txt_click} = params;



		const [x, y] = pos;
		const [w, h] = size;
		const th = text_height;

		const g = this.make_svg_elem('g', {transform: `translate(${x}, ${y})`})
		const r = this.make_svg_elem('rect',
			{x: 0, y: 0, width: w, height: h, class: 'state_body'});
		const border = this.make_svg_elem('rect',
			{x: 0, y: 0, width: w, height: h, class: 'state_border'});

		const s1 = this.make_svg_elem('line',
			{x1: 0, y1: th, x2: w, y2: th, class: 'state_separator'});
		const t = this.make_svg_elem('text', {x: th*0.3, y: th*0.8, class: 'state_title'});
		t.textContent = title;

		const text = this.make_svg_elem('text', {x: th*0.3, y: th*0.8, class: 'state_text'});
		for (const s of strings)
		{
			const tspan = this.make_svg_elem('tspan', {x: th*0.3, dy: th});
			this.build_text(tspan, s, on_txt_click);
			text.appendChild(tspan);
		}

		const m = this.make_svg_elem('line',
			{x1: w-1, y1: h, x2: w, y2: h-1, class: 'state_resize_mark'});

		const offset = th + th*strings.length;
		const s2 = this.make_svg_elem('line',
			{x1: 0, y1: offset, x2: w, y2: offset, class: 'state_separator'});

		const header = this.make_svg_elem('rect',
			{x: 0, y: 0, width: w, height: offset, class: 'state_drag_handle'});

		const resize_handle = this.make_svg_elem('circle',
			{cx: w-0.25, cy: h-0.25, class: 'state_resize_handle'});


		border.addEventListener('click', on_border_click);
		header.addEventListener('mousedown', on_header_mouse_down);
		header.addEventListener('mouseover', on_header_mouse_over);
		header.addEventListener('mouseleave', on_header_mouse_leave);
		header.addEventListener('click', on_header_click);
		resize_handle.addEventListener('mousedown', on_corner_mouse_down)

		g.appendChild(r);
		g.appendChild(s1);
		g.appendChild(s2);
		g.appendChild(m);
		g.appendChild(t);
		g.appendChild(header);
		g.appendChild(text);
		g.appendChild(border);
		g.appendChild(resize_handle);

		this.states[id] = {
			obj: g,
			mod_svg: this.modify_svg_elem,
			make_svg: this.make_svg_elem,
			get_evt_pos: this.get_obj_rel_pos,
			make_svg_elem: this.make_svg_elem,
			build_txt: this.build_text,
			get_rel_pos: function(evt)
			{
				return this.get_evt_pos(evt, this.obj);
			},
			resize: function(size)
			{
				const [w, h] = size;
				this.mod_svg(r, {width: w, height: h});
				this.mod_svg(border, {width: w, height: h});
				this.mod_svg(s1, {x2: w});
				this.mod_svg(s2, {x2: w});
				this.mod_svg(m, {x1: w-1, y1: h, x2: w, y2: h-1});
				this.mod_svg(header, {width: w});
				this.mod_svg(resize_handle, {cx: w-0.25, cy: h-0.25});
			},
			move: function(pos)
			{
				this.mod_svg(g, {transform: `translate(${pos[0]}, ${pos[1]})`});
			},
			add_border_class(cl)
			{
				border.classList.add(cl);
			},
			remove_border_class(cl)
			{
				border.classList.remove(cl);
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
				this.mod_svg(header, {height: offset});

				for (const [i, s] of txt.entries())
				{
					text.children[i].innerHTML = '';
					this.build_txt(text.children[i], s, on_txt_click);
				}
			},
			set_title(title)
			{
				t.textContent = title;
			}
		};

		this.drawing.appendChild(g);
	}
};

