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
		
		this.model.states().map(this.render_state, this);
		this.model.transitions().map(this.render_transiton, this);
	}

	state_drag_start(evt, state_id)
	{
		evt.preventDefault();
		this.drag_data.on_mousemove = (evt) => {this.state_drag(evt, state_id)};
		this.drag_data.on_mouseup = (evt) => {this.state_drag_end(evt, state_id)};
		this.gui.add_event_handler('mousemove', this.drag_data.on_mousemove);
		this.gui.add_event_handler( 'mouseup', this.drag_data.on_mouseup);

		const [ex, ey] = this.gui.get_absolute_pos(evt, state_id);
		const [sx, sy] = this.model.get_state(state_id).pos;
		this.drag_data.offset = [ex-sx, ey-sy];
		this.drag_data.transitions = this.model.state_transitions(state_id);
	}
	
	state_resize_start(evt, state_id)
	{
		evt.preventDefault();
		this.resize_data.on_mousemove = (evt) => {this.state_resize(evt, state_id)};
		this.resize_data.on_mouseup = (evt) => {this.state_resize_end(evt, state_id)};
		this.gui.add_event_handler('mousemove', this.resize_data.on_mousemove);
		this.gui.add_event_handler( 'mouseup', this.resize_data.on_mouseup);

		this.resize_data.transitions = this.model.state_transitions(state_id);
	}
	
	state_resize(evt, state_id)
	{
		evt.preventDefault();
		const size = this.gui.get_state_rel_pos(evt, state_id);
		this.model.resize_state(state_id, size);
		this.gui.states[state_id].resize(this.model.get_state(state_id).size);
		this.resize_data.transitions.map(
			(t) => {
				this.model.update_transition_path(t);
				this.redraw_transition(t);
			});
	}

	state_resize_end(evt, state_id)
	{
		evt.preventDefault();
		this.gui.remove_event_handler('mousemove', this.resize_data.on_mousemove);
		this.gui.remove_event_handler( 'mouseup', this.resize_data.on_mouseup);
	}

	state_drag(evt, state_id)
	{
		evt.preventDefault();
		const [ex, ey] = this.gui.get_absolute_pos(evt);
		const [ox, oy] = this.drag_data.offset;
		const pos = [ex-ox, ey-oy];
		const affected_states = this.model.move_state(state_id, pos);
		affected_states.push(state_id);
	
		affected_states.map((id) => {this.gui.states[id].move(this.model.get_state(id).pos);}, this);
		this.drag_data.transitions.map(
			(t) => {
				this.model.update_transition_path(t);
				this.redraw_transition(t);
			});
	}

	state_drag_end(evt, state_id)
	{
		evt.preventDefault();
		this.gui.remove_event_handler('mousemove', this.drag_data.on_mousemove);
		this.gui.remove_event_handler( 'mouseup', this.drag_data.on_mouseup);
	}

	render_state(state_id)
	{
		const state = this.model.get_state(state_id);

		this.gui.render_state(state_id,
			state.title,
			state.pos,
			state.size,
			state.text,
			this.model.options.text_height,
			(evt) => {this.state_drag_start(evt, state_id)},
			(evt) => {this.state_resize_start(evt, state_id)});
	}

	trans_drag_start(evt, trans_id)
	{
		evt.preventDefault();

		const p = this.gui.get_absolute_pos(evt);
		const line = this.model.transition_get_line_index(trans_id, p);

		this.drag_data.on_mousemove = (evt) => {this.trans_drag(evt, trans_id, line)};
		this.drag_data.on_mouseup = (evt) => {this.trans_drag_end(evt, trans_id, line)};
		this.drag_data.label_width = this.gui.get_path_label_size(trans_id)[0];
		this.gui.add_event_handler('mousemove', this.drag_data.on_mousemove);
		this.gui.add_event_handler( 'mouseup', this.drag_data.on_mouseup);
	}

	trans_drag(evt, trans_id, line)
	{
		evt.preventDefault();

		const p = this.gui.get_absolute_pos(evt);
		this.model.transition_drag(trans_id, line, p, this.drag_data.label_width);
		this.redraw_transition(trans_id);
	}

	trans_drag_end(evt, trans_id, line)
	{
		evt.preventDefault();
		this.gui.remove_event_handler('mousemove', this.drag_data.on_mousemove);
		this.gui.remove_event_handler( 'mouseup', this.drag_data.on_mouseup);
	}

	render_transiton(trans_id)
	{
		const tr = this.model.get_transition(trans_id);
		this.gui.render_transition(
			trans_id,
			tr.vertices,
			tr.label,
			tr.label_pos,
			(evt) => {this.trans_drag_start(evt, trans_id);});
	}

	redraw_transition(trans_id)
	{
		const tr = this.model.get_transition(trans_id);
		this.gui.redraw_path_with_arrow(trans_id, tr.vertices, tr.label, tr.label_pos);
	}
}

window.addEventListener('DOMContentLoaded', (event) => {window.app = new App(state_machine)});
