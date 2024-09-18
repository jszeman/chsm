import { Gui } from './modules/gui.mjs';
import { Model } from './modules/model.js';
import { state_machine } from './data.js';


class App {
	constructor(model)
	{
		this.model = new Model(state_machine);
		this.gui = new Gui();
		this.drawing = null;
		this.drag_data = {};
		this.resize_data = {};
		this.tr_draw_data = {};
		this.mouse_pos = [0, 0];
		this.enable_keys = true;
		this.text_state_id = '';
		this.text_tr_id = '';
		this.busy = false;
		this.prop_editor = {
			obj_id: null,
			obj_type: null
		};
		this.file_name = '';
		this.filepath = null;
		
		this.model.states().map(s => this.render_state(s), this);
		this.model.transitions().map(t => this.render_transiton(t), this);

		this.body = document.querySelector('body');
		this.main = document.querySelector('main');

		this.title = document.querySelector('#title');
		
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

		this.save_btn = document.querySelector('#btn-save');
		this.save_btn.addEventListener('click', e => this.dispatch('SAVE', e));

		this.open_btn = document.querySelector('#btn-open');
		this.open_btn.addEventListener('click', e => this.dispatch('OPEN', e));

		this.codegen_btn = document.querySelector('#btn-codegen');
		this.codegen_btn.addEventListener('click', e => this.dispatch('CODE_GEN', e));

		this.body.addEventListener('mousemove', event => {
			this.mouse_pos = this.gui.get_absolute_pos(event);
			this.dispatch('MOUSEMOVE', event);
		});

		this.body.addEventListener('mouseup', event => {
			this.dispatch('MOUSEUP', event);
		});

		this.gui.svg.addEventListener('click', event => {
			this.dispatch('CLICK', event);
		});

		this.gui.svg.addEventListener('wheel', event => {
			if (event.ctrlKey)
			{
				this.dispatch('DRAWING_CTRL_WHEEL', event);
			}
			else if (event.shiftKey)
			{
				this.dispatch('DRAWING_SHIFT_WHEEL', event);
			}
			else
			{
				this.dispatch('DRAWING_WHEEL', event);
			}
		});

		this.gui.svg.addEventListener('mousedown', event => {
			if (event.ctrlKey)
			{
				this.dispatch('DRAWING_CTRL_MDOWN', event);
			}
			else
			{
				this.dispatch('DRAWING_MDOWN', event);
			}
		});

		this.state = this.idle_state;

		eel.startup()
	}

	load_model(data, fname, fpath)
	{
		this.gui.clear();

		this.model = new Model(JSON.parse(data));
		this.model.states().map(s => this.render_state(s), this);
		this.model.transitions().map(t => this.render_transiton(t), this);

		this.gui.set_view(this.model.get_view());

		this.file_name = fname;
		this.filepath = fpath;
		this.title.textContent = this.file_name;
		document.title = this.file_name;
		
		this.prop_editor = {
			obj_id: null,
			obj_type: null
		};
	}

	changed()
	{
		return this.model.changed;
	}

	push_transition_changes_to_gui()
	{
		this.model.changes.trans_new.map(d => this.render_transiton(...d), this);

		this.model.changes.trans_redraw.map(d =>
			{
				const [tr_id, tr] = d;
				this.gui.redraw_path_with_arrow(tr_id, tr.vertices, tr.label, tr.label_pos);
			}, this);

		this.model.changes.trans_set_label.map(d => this.gui.paths[d[0]].set_label(this.model.chop_text(d[1].label)), this);

		this.model.changes.trans_del.map(d => this.gui.delete_transition(d[0]), this);

		this.model.changes.trans_set_label_pos.map(d => this.gui.paths[d[0]].set_label_pos(d[1]), this);
	}

	push_state_changes_to_gui()
	{
		this.model.changes.state_new.map(d => this.render_state(...d), this);
		this.model.changes.state_del.map(d => this.gui.delete_state(d[0]), this);
		this.model.changes.state_move.map(d => this.gui.states[d[0]].move(d[1].pos), this);
		this.model.changes.state_resize.map(d => this.gui.states[d[0]].resize(d[1].size), this);
		this.model.changes.state_set_text.map(d => this.gui.states[d[0]].set_text(d[1].text.map(this.model.chop_text, this.model),), this);
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
		const start_state = this.state;

		this.state(event, data);

		this.push_model_changes_to_gui();

		if ((this.state === this.idle_state) && (this.state !== start_state))
		{
			this.save_state();
		}
	}

	undo()
	{
		const view = this.model.undo();
		this.gui.clear();
		
		if (view !== null)
		{
			this.gui.set_view(view);
		}

		this.model.states().map(s => this.render_state(s), this);
		this.model.transitions().map(t => this.render_transiton(t), this);
	}

	redo()
	{
		const view = this.model.redo();
		this.gui.clear();

		if (view !== null)
		{
			this.gui.set_view(view);
		}

		this.model.states().map(s => this.render_state(s), this);
		this.model.transitions().map(t => this.render_transiton(t), this);
	}

	save_state()
	{
		if (this.model.save_state(this.gui.get_view()))
		{
			this.title.textContent = '*' + this.file_name;
		}
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
						this.save_state();
						break;

					case 'KeyI':
						this.create_initial_state(this.mouse_pos);
						this.save_state();
						break;

					case 'KeyT':
						this.start_transition();
						this.state = this.select_tr_start_state;
						break;

					case 'Tab':
						data.preventDefault();
						this.toggle_sidebar();
						break;
						
					case 'KeyD':
						this.clear_sidebar();
						this.start_delete_state_or_transition();
                        this.state = this.delete_st_or_tr_state;
						this.model.transitions().map(t => this.gui.redraw_path_change_line_color(t, true));
						break;

					case 'KeyZ':
						if (data.ctrlKey)
						{
							this.undo();
						}
						break;

					case 'KeyY':
						if (data.ctrlKey)
						{
							this.redo();
						}
						break;
	

					case 'KeyU':
						this.undo();
						break;

					case 'KeyR':
						this.redo();
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
				this.save_state();
				break;

			case 'APPLY_TEXT':
				this.apply_text();
				this.save_state();
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
				this.state_drag_start(data.event, data.id, true);
				this.state = this.state_dragging_state;
				break;

			case 'STATE_HEADER_SHIFT_M_DOWN':
				this.state_drag_start(data.event, data.id, false);
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

			case 'TR_LABEL_M_DOWN':
				this.trans_label_drag_start(data.event, data.id);
				this.state = this.transition_label_dragging_state;
				break;

			case 'TR_DBLCLICK':
				this.trans_split(data.event, data.id);
				this.save_state();
				break;

			case 'TXT_CLICK':
				this.dim_object();
				this.cache_text_changes();
				this.prop_editor.obj_id = data.event.target.firstChild.textContent;
				this.prop_editor.obj_type = 'text';
				this.show_obj_text();
				break;

			case 'TR_CLICK':
				data.event.stopPropagation();
				this.dim_object();
				this.drop_selection();
				this.cache_text_changes();
				this.prop_editor.obj_id = data.id;
				this.prop_editor.obj_type = 'transition';
				this.highlight_object();
				this.show_obj_text();
				break;

			case 'STATE_HEADER_CLICK':
				data.event.stopPropagation();
				if (!this.model.selection.has(data.id))
				{
					this.dim_object();
				}
				this.drop_selection();
				this.cache_text_changes();
				this.prop_editor.obj_id = data.id;
				this.prop_editor.obj_type = 'state';
				this.highlight_object();
				this.show_obj_text();
				break;

			case 'STATE_HEADER_CTRL_CLICK':
				data.event.stopPropagation();
				this.dim_object();
				this.select_state(data.id);
				break;

			case 'TR_SHIFT_CLICK':
				{
					const p = this.gui.get_absolute_pos(data.event);
					this.model.transition_restart_from_pos(data.id, p);
					this.start_transition();
					this.gui.paths[data.id].add_handle_class('transition_handle_highlight_draw');
					this.tr_draw_data.trans_id = data.id;
					this.state = this.transition_drawing_state;
				}
				break;

			case 'CLICK':
				this.dim_object();
				this.drop_selection();
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
				this.toggle_sidebar();
				break;

			case 'DRAWING_CTRL_WHEEL':
				{
					data.stopPropagation();
					data.preventDefault();
					const p = this.gui.get_absolute_pos(data);
					if (data.deltaY < 0)
					{
						this.gui.zoom_in(p);
					}
					else
					{
						this.gui.zoom_out(p);
					}
				}
				break;

			case 'DRAWING_WHEEL':
				{
					data.stopPropagation();
					data.preventDefault();
					if (data.deltaY < 0)
					{
						this.gui.pan_up();
					}
					else
					{
						this.gui.pan_down();
					}
				}
				break;

			case 'DRAWING_SHIFT_WHEEL':
				{
					data.stopPropagation();
					data.preventDefault();
					if (data.deltaY < 0)
					{
						this.gui.pan_right();
					}
					else
					{
						this.gui.pan_left();
					}
				}
				break;

			case 'DRAWING_CTRL_MDOWN':
				this.state_drag_start(data, '__top__', true);
				this.state = this.state_dragging_state;
				break;

			case 'SAVE':
				this.model.set_view(this.gui.get_view());
				this.busy = true;
				eel.save_state_machine(this.main.innerHTML, this.model.get_data_string(), this.filepath);
				this.busy = false;
				break;

			case 'OPEN':
				this.busy = true;
				eel.open_file();
				this.busy = false;
				break;

			case 'CODE_GEN':
				this.model.set_view(this.gui.get_view());
				this.busy = true;
				eel.save_state_machine(this.main.innerHTML, this.model.get_data_string(), this.filepath);
				eel.genereate_code();
				this.busy = false;
				break;

			case 'SAVE_RESULT':
				console.log(data);
				this.file_name = data.filename;
				this.title.textContent = this.file_name;
				this.filepath = data.filepath;
				this.model.clr_changed();
				break;

			case 'CODEGEN_RESULT':
				console.log(data);
				break;
		}
	}

	clear_sidebar()
	{
		this.dim_object();
		this.cache_text_changes();
		this.clear_text_inputs();
		this.prop_editor.obj_id = null;
		this.prop_editor.obj_type = null;
	}

	toggle_sidebar()
	{
		this.sidebar.hidden = !this.sidebar.hidden;
		this.sidebar_handle_text.textContent = this.sidebar.hidden ? '>' : '<';
	}

	sidebar_resizing_state(event, data)
	{
		//console.log('resize', event);
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

	highlight_state(state_id)
	{
		this.gui.states[state_id].add_border_class('state_border_highlight');
	}

	dim_state(state_id)
	{
		this.gui.states[state_id].remove_border_class('state_border_highlight');
	}

	drop_selection()
	{
		for (let state_id of this.model.selection)
		{
			this.dim_state(state_id);
		}
		this.model.drop_selection();
	}

	select_state(state_id)
	{
		for (let state_id of this.model.selection)
		{
			this.dim_state(state_id);
		}

		this.model.select_state(state_id);

		for (let state_id of this.model.selection)
		{
			this.highlight_state(state_id);
		}
	}

	highlight_object()
	{
		const {obj_id, obj_type} = this.prop_editor;

		if (obj_type === 'state')
		{
			this.highlight_state(obj_id);
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

		if ((obj_type === 'state') && (obj_id in this.gui.states))
		{
			this.dim_state(obj_id);
		}
		else if ((obj_type === 'transition') && (obj_id in this.gui.paths))
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

		switch (obj_type)
		{
			case 'state':
				this.model.apply_state_title(obj_id, this.title_input.value);
				break;

			case 'transition':
				this.model.apply_transition_label(obj_id, this.title_input.value);
				break;
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

		switch (obj_type)
		{
			case 'state':
				this.model.apply_state_text(obj_id, this.text_area.value);
				break;
		}
	}

	cache_text_changes()
	{
		const {obj_id, obj_type} = this.prop_editor;

		switch (obj_type)
		{
			case 'state':
				const state_text = {title: this.title_input.value, text: this.text_area.value};
				this.model.cache_state_text(obj_id, state_text);
				break;

			case 'transition':
				this.model.cache_transition_label(obj_id, this.title_input.value);
				break;

			case 'text':
				this.model.cache_note_text(obj_id, this.text_area.value);
				break;
		}
	}

	show_state_text(state_id)
	{
		const text = this.model.get_state_text(state_id);

		this.title_input.disabled = false;
		this.title_input.value = text.title;
		this.text_area.disabled = false;
		this.text_area.value = text.text;
	}

	show_transition_text(tr_id)
	{
		this.title_input.disabled = false;
		this.title_input.value = this.model.get_transition_text(tr_id);
		this.text_area.value = '';
		this.text_area.disabled = true;
	}

	show_note_text(obj_id)
	{
		this.title_input.disabled = true;
		this.title_input.value = obj_id;
		this.text_area.disabled = false;
		this.text_area.value = this.model.get_note_text(obj_id);
	}

	clear_text_inputs()
	{
		this.title_input.disabled = false;
		this.title_input.value = '';
		this.text_area.disabled = false;
		this.text_area.value = '';
	}

	show_obj_text()
	{
		const {obj_id, obj_type} = this.prop_editor;

		switch (obj_type)
		{
			case 'state':
				this.show_state_text(obj_id);
				break;

			case 'transition':
				this.show_transition_text(obj_id);
				break;

			case 'text':
				this.show_note_text(obj_id);
				break;
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
				const tr_id = this.model.set_transition_start(data.id, pos);
				if (tr_id !== '')
				{
					this.tr_draw_data.trans_id = tr_id;
					this.render_transiton(tr_id);
					this.state = this.transition_drawing_state;
					this.gui.paths[tr_id].add_handle_class('transition_handle_highlight_draw');
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
				this.trans_drag(data);
				break

			case 'MOUSEUP':
				this.trans_drag_end(data);
				this.state = this.idle_state;
				break;
		}
	}

	transition_label_dragging_state(event, data)
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

			case 'MOUSEMOVE':
				this.trans_label_drag(data);
				break

			case 'MOUSEUP':
				this.trans_label_drag_end(data);
				this.state = this.idle_state;
				break;
		}
	}

	state_resizing_state(event, data)
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
		this.model.make_new_state(pos);
	}

	create_initial_state(pos)
	{
		this.model.make_new_initial_state(pos);
	}

	state_drag_start(evt, state_id, drag_substates)
	{
		evt.preventDefault();
		const [ex, ey] = this.gui.get_absolute_pos(evt);
		const [sx, sy] = this.model.get_state(state_id).pos;
		this.drag_data.offset = [ex-sx, ey-sy];
		this.drag_data.state_id = state_id;
		this.drag_data.drag_substates = drag_substates;
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
		this.model.move_state(state_id, pos, this.drag_data.drag_substates);
	}

	state_drag_end(evt)
	{
		evt.preventDefault();
		this.model.update_parents();
	}

	render_state(state_id, state_data=null)
	{
		const state = state_data !== null ? state_data : this.model.get_state(state_id);

		const params = {
			id: 					state_id,
			title: 					state.title,
			pos: 					state.pos,
			size:					state.size,
			strings:				state.text.map(this.model.chop_text, this.model),
			type:					state.type,
			text_height:			this.model.options.text_height,
			on_header_mouse_down:	evt => {
				if (evt.shiftKey)
				{
					this.dispatch('STATE_HEADER_SHIFT_M_DOWN', {event: evt, id: state_id});
				}
				else
				{
					this.dispatch('STATE_HEADER_M_DOWN', {event: evt, id: state_id});
				}
			},
			on_header_click:		evt => {
				if (evt.ctrlKey)
				{
					this.dispatch('STATE_HEADER_CTRL_CLICK', {event: evt, id: state_id})
				}
				else
				{
					this.dispatch('STATE_HEADER_CLICK', {event: evt, id: state_id})
				}
			},
			on_corner_mouse_down:	evt => this.dispatch('STATE_RESIZE', {event: evt, id: state_id}),
			on_border_click:		evt => this.dispatch('STATE_BORDER_CLICK', {event: evt, id: state_id}),
			on_header_mouse_over:	evt => this.dispatch('STATE_HEADER_M_OVER', {event: evt, id: state_id}),
			on_header_mouse_leave:	evt => this.dispatch('STATE_HEADER_M_LEAVE', {event: evt, id: state_id}),
			on_txt_click:			evt => this.dispatch('TXT_CLICK', {event: evt}),
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

	trans_label_drag_start(evt, trans_id)
	{
		evt.preventDefault();

		const [ex, ey] = this.gui.get_absolute_pos(evt);
		const [sx, sy] = this.model.get_transition(trans_id).label_pos;
		this.drag_data.offset = [ex-sx, ey-sy];
		this.drag_data.trans_id = trans_id;
		this.gui.set_cursor('grab');
	}

	trans_label_drag(evt)
	{
		evt.preventDefault();
		const [ex, ey] = this.gui.get_absolute_pos(evt);
		const [ox, oy] = this.drag_data.offset;
		const pos = [ex-ox, ey-oy];

		const trans_id = this.drag_data.trans_id;
		this.model.transition_label_drag(trans_id, pos);
	}

	trans_label_drag_end(evt)
	{
		evt.preventDefault();
		this.gui.set_cursor('auto');
	}

	render_transiton(trans_id, tr_data=null)
	{
		const tr = tr_data !== null ? tr_data : this.model.get_transition(trans_id);

		const params = {
			id:			trans_id,
			vertices:	tr.vertices,
			label:		this.model.chop_text(tr.label),
			label_pos:	tr.label_pos,
			on_mousedown:	evt => this.dispatch('TR_M_DOWN', {event: evt, id: trans_id}),
			on_dblclick:	evt => this.dispatch('TR_DBLCLICK', {event: evt, id: trans_id}),
			on_click:		evt => {
										if (evt.shiftKey)
										{
											this.dispatch('TR_SHIFT_CLICK', {event: evt, id: trans_id});
										}
										else
										{
											this.dispatch('TR_CLICK', {event: evt, id: trans_id});
										}
									},
			on_label_mousedown: evt => this.dispatch('TR_LABEL_M_DOWN', {event: evt, id: trans_id}),
			on_txt_click:		evt => this.dispatch('TXT_CLICK', {event: evt})
		};

		this.gui.render_transition(params);
	}
}

window.addEventListener('DOMContentLoaded', event => {window.app = new App(state_machine)});

addEventListener("visibilitychange", (event) => {
	if (document.hidden)
	{
		eel.pagehide(window.app.model.get_data_string(), window.app.changed());
	}
	else
	{
		eel.pageshow();
	}
});

eel.expose(load_json); // Expose this function to Python
function load_json(data, filename, filepath) {
	//console.log(data)
	window.app.load_model(data, filename, filepath);
}

eel.expose(set_changed); // Expose this function to Python
function set_changed() {
	//console.log(data)
	window.app.model.set_changed();
	//console.log(`Changed: ${window.app.changed()}`)
}

eel.expose(send_event);
function send_event(event, data)
{
	window.app.dispatch(event, data);
}

let link_time = 1000;

window.addEventListener('beforeunload', function (e) {
	if (link_time > 0 && window.app.changed())
	{
		e.preventDefault();
	}
	eel.closing();
});

eel.expose(link_up);
function link_up(time)
{
	link_time = time;
}

function timer_callback()
{
	link_time -= 500;

	if (link_time < 0)
	{
		console.log('close');
		window.close();
	}
}

window.setInterval(timer_callback, 500);