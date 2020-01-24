
export const state_machine = {
	states: {
		state_0: {
			pos: [2, 2],
			size: [15, 20],
			title: 'State 0',
			text: ['entry/ entry_func()', 'exit/ exit_func()'],
			connectors: ['conn_0', 'conn_2'],
			parent: '__top__',
			children: ['state_2'],
			},
		state_1: {
			pos: [2, 35],
			size: [15, 15],
			title: 'State 1',
			text: ['entry/ entry_func()', 'exit/ exit_func()'],
			connectors: ['conn_1', 'conn_3'],
			parent: '__top__',
			children: [],
			},
		state_2: {
			pos: [3, 9],
			size: [12, 8],
			title: 'State 2',
			text: ['entry/ entry_func()', 'exit/ exit_func()'],
			connectors: [],
			parent: 'state_0',
			children: [],
			}
	},

	connectors: {
		conn_0: {
			parent: 'state_0',
			offset: 5,
			side: 'right',
			dir: 'out',
			transition: 'trans_0',
		},
		conn_1: {
			parent: 'state_1',
			offset: 10,
			side: 'right',
			dir: 'in',
			transition: 'trans_0',
		},
		conn_2: {
			parent: 'state_0',
			offset: 5,
			side: 'bottom',
			dir: 'out',
			transition: 'trans_1',
		},
		conn_3: {
			parent: 'state_1',
			offset: 5,
			side: 'top',
			dir: 'in',
			transition: 'trans_1',
		}
	},

	transitions: {
		trans_0: {
			start: 'conn_0',
			end: 'conn_1',
			vertices: [[17, 7], [22, 7], [22, 45], [17, 45]],
			label: 'trans_0',
			label_offset: [0.5, -0.4],
			label_anchor: 0,
			label_pos: [17.5, 6.6],
		},
		trans_1: {
			start: 'conn_2',
			end: 'conn_3',
			vertices: [[7, 22], [7, 30], [7, 30], [7, 35]],
			label: 'trans_1',
			label_offset: [0.5, -0.4],
			label_anchor: 1,
			label_pos: [7.5, 29.6],
		}
	},

};

