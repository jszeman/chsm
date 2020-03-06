import { Gui } from './modules/gui.mjs';
import { Model } from './modules/model.js';
import { state_machine } from './data.js';


class App {
	constructor(model)
	{
		this.model = new Model(model);
		this.gui = new Gui();
		this.drawing = null;
		this.drag_data = {};
		this.resize_data = {};
		this.tr_draw_data = {};
		this.mouse_pos = [0, 0];
		this.enable_keys = true;
		this.text_state_id = '';
		this.text_tr_id = '';
		this.prop_editor = {
			obj_id: null,
			obj_type: null
		};
		
		this.model.states().map(s => this.render_state(s), this);
		this.model.transitions().map(t => this.render_transiton(t), this);

		this.body = document.querySelector('body');
		
		this.body.addEventListener("keydown", event => {
			if (!this.enable_keys) return;
			if (event.isComposing || event.keyCode === 229) {
				return;
			}
			this.dispatch('KEYDOWN', event);
		});

		this.title_input = document.querySelector('#obj-label');
		this.title_input.addEventListener('focus', e => this.dispatch('LABEL_FOCUS', e));
		this.title_input.addEventListener('blur', e => this.dispatch('LABEL_BLUR', e));

		this.text_area = document.querySelector('#obj-text');
		this.text_area.addEventListener('focus', e => this.dispatch('TEXT_FOCUS', e));
		this.text_area.addEventListener('blur', e => this.dispatch('TEXT_BLUR', e));

		this.sidebar = document.querySelector('#sidebar');

		this.sidebar_handle = document.querySelector('#sidebar-handle');
		this.sidebar_handle.addEventListener('mousedown', e => this.dispatch('SB_HANDLE_MDOWN', e));

		this.sidebar_handle_text = document.querySelector('#sidebar-handle-text');
		this.sidebar_handle_text.addEventListener('click', e => this.dispatch('SB_HANDLE_TEXT_CLICK', e));
		this.sidebar_handle_text.addEventListener('mousedown', e => this.dispatch('SB_HANDLE_TEXT_MDOWN', e));

		this.title_reset_btn = document.querySelector('#btn-label-reset');
		this.title_reset_btn.addEventListener('click', e => this.dispatch('RESET_TITLE', e));

		this.text_reset_btn = document.querySelector('#btn-text-reset');
		this.text_reset_btn.addEventListener('click', e => this.dispatch('RESET_TEXT', e));

		this.title_apply_btn = document.querySelector('#btn-label-apply');
		this.title_apply_btn.addEventListener('click', e => this.dispatch('APPLY_TITLE', e));

		this.text_apply_btn = document.querySelector('#btn-text-apply');
		this.text_apply_btn.addEventListener('click', e => this.dispatch('APPLY_TEXT', e));

		this.body.addEventListener('mousemove', event => {
			this.mouse_pos = this.gui.get_absolute_pos(event);
			this.dispatch('MOUSEMOVE', event);
		});

		this.body.addEventListener('mouseup', event => {
			this.dispatch('MOUSEUP', event);
		});

		this.body.addEventListener('click', event => {
			this.dispatch('CLICK', event);
		});

		this.state = this.idle_state;
	}

	push_transition_changes_to_gui()
	{
		this.model.changes.trans_new.map(d => this.render_transiton(...d), this);

		this.model.changes.trans_redraw.map(d =>
			{
				const [tr_id, tr] = d;
				this.gui.redraw_path_with_arrow(tr_id, tr.vertices, tr.label, tr.label_pos);
			}, this);

		this.model.changes.trans_set_label.map(d => this.gui.paths[d[0]].set_label(d[1].label), this);

		this.model.changes.trans_del.map(d => this.gui.delete_transition(d[0]), this);
	}

	push_state_changes_to_gui()
	{
		this.model.changes.state_new.map(d => this.render_state(...d), this);
		this.model.changes.state_del.map(d => this.gui.delete_state(d[0]), this);
		this.model.changes.state_move.map(d => this.gui.states[d[0]].move(d[1].pos), this);
		this.model.changes.state_resize.map(d => this.gui.states[d[0]].resize(d[1].size), this);
		this.model.changes.state_set_text.map(d => this.gui.states[d[0]].set_text(d[1].text), this);
		this.model.changes.state_set_title.map(d => this.gui.states[d[0]].set_title(d[1].title), this);
	}

	push_model_changes_to_gui()
	{
		this.push_transition_changes_to_gui();
		this.push_state_changes_to_gui();

		this.model.ack_changes();
	}

	dispatch(event, data)
	{
		this.state(event, data);

		this.push_model_changes_to_gui();
	}

	idle_state(event, data)
	{
		//console.log('idle', event);
		switch(event)
		{
			case 'KEYDOWN':
				switch(data.code)
				{
					case 'KeyS':
						this.create_state(this.mouse_pos);
						break;

					case 'KeyT':
						this.start_transition();
						this.state = this.select_tr_start_state;
						break;
						
					case 'KeyD':
						this.start_delete_state_or_transition();
                        this.state = this.delete_st_or_tr_state;
						this.model.transitions().map(t => this.gui.redraw_path_change_line_color(t, true));
						break;
				}
				break;

			case 'MOUSEMOVE':
				//console.log('idle', 'MOUSE_MOVE', data);
				break;

			case 'RESET_TITLE':
				this.reset_title();
				break

			case 'RESET_TEXT':
				this.reset_text();
				break;

			case 'APPLY_TITLE':
				this.apply_title();
				break;

			case 'APPLY_TEXT':
				this.apply_text();
				break;

			case 'LABEL_FOCUS':
			case 'TEXT_FOCUS':
				this.highlight_object();
				this.enable_keys = false;
				break;

			case 'LABEL_BLUR':
			case 'TEXT_BLUR':
				this.enable_keys = true;
				break;

			case 'STATE_HEADER_M_DOWN':
				this.state_drag_start(data.event, data.id);
				this.state = this.state_dragging_state;
				break;

			case 'STATE_RESIZE':
				this.state_resize_start(data.event, data.id);
				this.state = this.state_resizing_state;
				break;

			case 'TR_M_DOWN':
				this.trans_drag_start(data.event, data.id);
				this.state = this.transition_dragging_state;
				break;

			case 'TR_DBLCLICK':
				this.trans_split(data.event, data.id);
				break;

			case 'TR_CLICK':
				data.event.stopPropagation();
				this.dim_object();
				this.cache_text_changes();
				this.prop_editor.obj_id = data.id;
				this.prop_editor.obj_type = 'transition';
				this.highlight_object();
				this.show_obj_text();
				break;

			case 'STATE_HEADER_CLICK':
				data.event.stopPropagation();
				this.dim_object();
				this.cache_text_changes();
				this.prop_editor.obj_id = data.id;
				this.prop_editor.obj_type = 'state';
				this.highlight_object();
				this.show_obj_text();
				break;

			case 'TR_CTRL_CLICK':
				const p = this.gui.get_absolute_pos(data.event);
				this.model.transition_restart_from_pos(data.id, p);
				this.start_transition();
				this.gui.paths[data.id].add_handle_class('transition_handle_highlight_draw');
				this.tr_draw_data.trans_id = data.id;
				this.state = this.transition_drawing_state;
				break;

			case 'CLICK':
				this.dim_object();
				this.cache_text_changes();
				break;

			case 'SB_HANDLE_MDOWN':
				data.stopPropagation();
				this.state = this.sidebar_resizing_state;
				break;

			case 'SB_HANDLE_TEXT_MDOWN':
				data.stopPropagation();
				break;


			case 'SB_HANDLE_TEXT_CLICK':
				data.stopPropagation();
				data.preventDefault();
				this.sidebar.hidden = !this.sidebar.hidden;
				this.sidebar_handle_text.textContent = this.sidebar.hidden ? '>' : '<';
				break;
		}
	}

	sidebar_resizing_state(event, data)
	{
		console.log('resize', event);
		switch(event)
		{
			case 'KEYDOWN':
				switch(data.code)
				{
					case 'Escape':
						this.gui.set_cursor('auto');
						this.state = this.idle_state;
						break;
				}
				break;

			case 'MOUSEMOVE':
				data.preventDefault();
				data.stopPropagation();
				if (!this.sidebar.hidden)
				{
					this.sidebar.style.flex = `0 0 ${(data.x - this.sidebar.getBoundingClientRect().left - 10)}px`;
				}
				break

			case 'MOUSEUP':
				this.gui.set_cursor('auto');
				this.state = this.idle_state;
				break;
		}
	}


	highlight_object()
	{
		const {obj_id, obj_type} = this.prop_editor;

		if (obj_type === 'state')
		{
			this.gui.states[obj_id].add_border_class('state_border_highlight');
			this.sidebar.style.background = 'rgba(23, 197, 67, 0.1)';
		}
		else if (obj_type === 'transition')
		{
			this.gui.paths[obj_id].add_handle_class('transition_handle_highlight_edit');
			this.sidebar.style.background = 'rgba(23, 197, 67, 0.1)';
		}
	}

	dim_object()
	{
		const {obj_id, obj_type} = this.prop_editor;

		if (obj_type === 'state')
		{
			this.gui.states[obj_id].remove_border_class('state_border_highlight');
		}
		else if (obj_type === 'transition')
		{
			this.gui.paths[obj_id].remove_handle_class('transition_handle_highlight_edit');
		}
		
		this.sidebar.style.background = 'white';
	}

	reset_title()
	{
		const {obj_id, obj_type} = this.prop_editor;

		if (obj_type === 'state')
		{
			const text = this.model.reset_state_title(obj_id);
			this.title_input.value = text.title;
		}
		else if (obj_type === 'transition')
		{
			this.title_input.value = this.model.reset_transition_label(obj_id);
		}
	}

	apply_title()
	{
		const {obj_id, obj_type} = this.prop_editor;

		if (obj_type === 'state')
		{
			this.model.apply_state_title(obj_id, this.title_input.value);
		}
		else if (obj_type === 'transition')
		{
			this.model.apply_transition_label(obj_id, this.title_input.value);
		}
	}

	reset_text()
	{
		const {obj_id, obj_type} = this.prop_editor;

		if (obj_type === 'state')
		{
			const text = this.model.reset_state_text(obj_id);
			this.text_area.value = text.text;
		}
	}

	apply_text()
	{
		const {obj_id, obj_type} = this.prop_editor;

		if (obj_type === 'state')
		{
			this.model.apply_state_text(obj_id, this.text_area.value);
		}
	}

	cache_text_changes()
	{
		const {obj_id, obj_type} = this.prop_editor;

		if (obj_type === 'state')
		{
			const text = {title: this.title_input.value, text: this.text_area.value};
			this.model.cache_state_text(obj_id, text);
		}
		else if (obj_type === 'transition')
		{
			this.model.cache_transition_label(obj_id, this.title_input.value);
		}
	}

	show_state_text(state_id)
	{
		const text = this.model.get_state_text(state_id);

		this.title_input.value = text.title;
		this.text_area.disabled = false;
		this.text_area.value = text.text;
	}

	show_transition_text(tr_id)
	{
		this.title_input.value = this.model.get_transition_text(tr_id);
		this.text_area.value = '';
		this.text_area.disabled = true;
	}

	show_obj_text()
	{
		const {obj_id, obj_type} = this.prop_editor;

		if (obj_type === 'state')
		{
			this.show_state_text(obj_id);
		}
		else if (obj_type === 'transition')
		{
			this.show_transition_text(obj_id);
		}
	}

	start_transition()
	{
		this.gui.set_cursor('crosshair');
	}
	
	start_delete_state_or_transition()
	{
		this.gui.set_cursor('crosshair');
	}

	select_tr_start_state(event, data)
	{
		switch(event)
		{
			case 'KEYDOWN':
				switch(data.code)
				{
					case 'Escape':
						this.gui.set_cursor('auto');
						this.state = this.idle_state;
						break;
				}
				break;

			case 'STATE_BORDER_CLICK':
				data.event.stopPropagation();
				const pos = this.gui.get_state_rel_pos(data.event, data.id);
				const t = this.model.make_new_transition();
				if (this.model.set_transition_start(t, data.id, pos))
				{
					this.tr_draw_data.trans_id = t;
					this.render_transiton(t);
					this.state = this.transition_drawing_state;
					this.gui.paths[t].add_handle_class('transition_handle_highlight_draw');
				}
				else
				{
					this.model.delete_transition(t);
				}
				break;
		}
	}
	
	delete_st_or_tr_state(event, data)
	{
		switch(event)
		{
			case 'KEYDOWN':
				switch(data.code)
				{
					case 'Escape':
						this.leave_delete_state();
						break;
				}
				break;
			
			case 'TR_M_DOWN':								//deleting a transition
				this.model.delete_transition(data.id);
				this.leave_delete_state();
				break;

			case 'STATE_HEADER_M_DOWN':							//deleting a state
				this.model.delete_state(data.id);
				this.leave_delete_state();
				break;

			case 'STATE_HEADER_M_OVER':
				this.gui.states[data.id].add_border_class('state_border_deleting');
				break;

			case 'STATE_HEADER_M_LEAVE':
				this.gui.states[data.id].remove_border_class('state_border_deleting');
				break;
		}
	}

	leave_delete_state()
	{
		this.gui.set_cursor('auto');
		this.state = this.idle_state;
		this.model.transitions().map(t => this.gui.redraw_path_change_line_color(t, false));
	}

	transition_drawing_state(event, data)
	{
		switch(event)
		{
			case 'KEYDOWN':
				switch(data.code)
				{
					case 'Escape':
						this.model.delete_transition(this.tr_draw_data.trans_id);
						this.gui.set_cursor('auto');
						this.state = this.idle_state;
						break;

					case 'Space':
						this.model.switch_transition_elbow(this.tr_draw_data.trans_id, this.mouse_pos);
						break;

					case 'Backspace':
						data.preventDefault();
						this.model.remove_transition_vertex(this.tr_draw_data.trans_id, this.mouse_pos);
						break;
				}
				break;

			case 'MOUSEMOVE':
				this.model.set_transition_endpoint(this.tr_draw_data.trans_id, this.mouse_pos);
				break

			case 'CLICK':
				this.model.add_transition_vertex(this.tr_draw_data.trans_id);
				break;
			
			case 'STATE_BORDER_CLICK':
				data.event.stopPropagation();
				const pos = this.gui.get_state_rel_pos(data.event, data.id);
				if (this.model.set_transition_end(this.tr_draw_data.trans_id, data.id, pos))
				{
					this.gui.set_cursor('auto');
					this.state = this.idle_state;
					this.gui.paths[this.tr_draw_data.trans_id].remove_handle_class('transition_handle_highlight_draw');
				}
				break;
		}
	}

	transition_dragging_state(event, data)
	{
		switch(event)
		{
			case 'MOUSEMOVE':
				this.trans_drag(data);
				break

			case 'MOUSEUP':
				this.trans_drag_end(data);
				this.state = this.idle_state;
				break;
		}
	}

	state_resizing_state(event, data)
	{
		switch(event)
		{
			case 'MOUSEMOVE':
				this.state_resize(data);
				break

			case 'MOUSEUP':
				this.state_resize_end(data);
				this.state = this.idle_state;
				break;
		}
	}

	state_dragging_state(event, data)
	{
		switch(event)
		{
			case 'MOUSEMOVE':
				this.state_drag(data);
				break

			case 'MOUSEUP':
				this.state_drag_end(data);
				this.state = this.idle_state;
				break;
		}
	}

	set_state_text(state_id, text)
	{
		this.model.set_state_text(state_id, text);
		this.push_model_changes_to_gui();
	}

	create_state(pos)
	{
		const state_id = this.model.make_new_state(pos);
	}

	state_drag_start(evt, state_id)
	{
		evt.preventDefault();
		const [ex, ey] = this.gui.get_absolute_pos(evt);
		const [sx, sy] = this.model.get_state(state_id).pos;
		this.drag_data.offset = [ex-sx, ey-sy];
		this.drag_data.state_id = state_id;
	}
	
	state_resize_start(evt, state_id)
	{
		evt.preventDefault();

		this.resize_data.transitions = this.model.state_transitions(state_id);
		this.resize_data.state_id = state_id;
	}
	
	state_resize(evt)
	{
		evt.preventDefault();
		const state_id = this.resize_data.state_id;
		const size = this.gui.get_state_rel_pos(evt, state_id);
		this.model.resize_state(state_id, size);
	}

	state_resize_end(evt)
	{
		evt.preventDefault();
	}

	state_drag(evt)
	{
		evt.preventDefault();
		const [ex, ey] = this.gui.get_absolute_pos(evt);
		const [ox, oy] = this.drag_data.offset;
		const pos = [ex-ox, ey-oy];
		const state_id = this.drag_data.state_id;
		this.model.move_state(state_id, pos);
	}

	state_drag_end(evt)
	{
		evt.preventDefault();
		this.model.update_parent(this.drag_data.state_id);
	}

	render_state(state_id, state_data=null)
	{
		const state = state_data !== null ? state_data : this.model.get_state(state_id);

		const params = {
			id: 					state_id,
			title: 					state.title,
			pos: 					state.pos,
			size:					state.size,
			strings:				state.text,
			text_height:			this.model.options.text_height,
			on_header_mouse_down:	evt => this.dispatch('STATE_HEADER_M_DOWN', {event: evt, id: state_id}),
			on_header_click:		evt => this.dispatch('STATE_HEADER_CLICK', {event: evt, id: state_id}),
			on_corner_mouse_down:	evt => this.dispatch('STATE_RESIZE', {event: evt, id: state_id}),
			on_border_click:		evt => this.dispatch('STATE_BORDER_CLICK', {event: evt, id: state_id}),
			on_header_mouse_over:	evt => this.dispatch('STATE_HEADER_M_OVER', {event: evt, id: state_id}),
			on_header_mouse_leave:	evt => this.dispatch('STATE_HEADER_M_LEAVE', {event: evt, id: state_id})
		};

		this.gui.render_state(params);
	}

	trans_drag_start(evt, trans_id)
	{
		evt.preventDefault();

		const p = this.gui.get_absolute_pos(evt);
		const line = this.model.transition_get_line_index(trans_id, p);

		this.drag_data.trans_id = trans_id;
		this.drag_data.line = line;
		this.drag_data.label_width = this.gui.get_path_label_size(trans_id)[0];
		this.gui.paths[trans_id].add_handle_class('transition_handle_highlight_drag');
		this.gui.set_cursor('grab');
	}

	trans_split(evt, trans_id)
	{
		evt.preventDefault();

		const p = this.gui.get_absolute_pos(evt);
		this.model.transition_split(trans_id, p);
	}

	trans_drag(evt)
	{
		evt.preventDefault();

		const p = this.gui.get_absolute_pos(evt);
		const trans_id = this.drag_data.trans_id;
		const line = this.drag_data.line;
		this.model.transition_drag(trans_id, line, p, this.drag_data.label_width);
	}

	trans_drag_end(evt)
	{
		evt.preventDefault();
		this.model.simplify_tr_path(this.drag_data.trans_id);
		this.gui.paths[this.drag_data.trans_id].remove_handle_class('transition_handle_highlight_drag');
		this.gui.set_cursor('auto');
	}

	render_transiton(trans_id, tr_data=null)
	{
		const tr = tr_data !== null ? tr_data : this.model.get_transition(trans_id);
		this.gui.render_transition(
			trans_id,
			tr.vertices,
			tr.label,
			tr.label_pos,
			evt => this.dispatch('TR_M_DOWN', {event: evt, id: trans_id}),
			evt => this.dispatch('TR_DBLCLICK', {event: evt, id: trans_id}),
			evt => {
				if (evt.ctrlKey)
				{
					this.dispatch('TR_CTRL_CLICK', {event: evt, id: trans_id});
				}
				else
				{
					this.dispatch('TR_CLICK', {event: evt, id: trans_id});
				}
			});
	}
}

window.addEventListener('DOMContentLoaded', (event) => {window.app = new App(state_machine)});
