
export const state_machine = {
	states: {
		__top__: {
			pos: [-100000000, -100000000],
			size: [200000000, 200000000],
			title: '__top__',
			text: [],
			connectors: [],
			parent: null,
			children: ['state_0', 'state_1', 'istate_0'],
			type: 'top'
			},
		state_0: {
			pos: [2, 2],
			size: [15, 20],
			title: 'State 0',
			text: ['entry/ entry_func()', 'exit/ exit_func()'],
			connectors: ['conn_0', 'conn_2'],
			parent: '__top__',
			children: ['state_2'],
			type: 'normal'
			},
		state_1: {
			pos: [2, 35],
			size: [15, 15],
			title: 'State 1',
			text: ['entry/ entry_func()', 'exit/ exit_func()'],
			connectors: ['conn_1', 'conn_3'],
			parent: '__top__',
			children: [],
			type: 'normal'
			},
		state_2: {
			pos: [3, 9],
			size: [12, 8],
			title: 'State 2',
			text: ['entry/ entry_func()', 'exit/ exit_func()'],
			connectors: ['conn_4', 'conn_5'],
			parent: 'state_0',
			children: [],
			type: 'normal'
			},
		istate_0: {
			pos: [30, 10],
			size: [2, 2],
			title: '',
			text: [],
			connectors: [],
			parent: '__top__',
			children: [],
			type: 'initial'
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
		},
		conn_4: {
			parent: 'state_2',
			offset: 2,
			side: 'right',
			dir: 'out',
			transition: 'trans_2',
		},
		conn_5: {
			parent: 'state_2',
			offset: 5,
			side: 'right',
			dir: 'in',
			transition: 'trans_2',
		}
	},

	transitions: {
		trans_0: {
			start: 'conn_0',
			end: 'conn_1',
			vertices: [[17, 7], [22, 7], [22, 45], [17, 45]],
			label: 'sig_0 [guard()]/ func()',
			label_offset: [0.5, -0.4],
			label_anchor: 0,
			label_pos: [17.5, 6.6],
		},
		trans_1: {
			start: 'conn_2',
			end: 'conn_3',
			vertices: [[7, 22], [7, 30], [7, 30], [7, 35]],
			label: 'sig_1',
			label_offset: [0.5, -0.4],
			label_anchor: 1,
			label_pos: [7.5, 29.6],
		},
		trans_2: {
			start: 'conn_4',
			end: 'conn_5',
			vertices: [[15, 11], [16, 11], [16, 14], [15, 14]],
			label: 'sig_2',
			label_offset: [0.5, -0.4],
			label_anchor: 0,
			label_pos: [15.5, 10.6],
		},
	},

	notes: {
		'sig_0': 'Signal 0',
		'guard()': 'Guard function',
		'func()': 'Event handler function',
	},

};

