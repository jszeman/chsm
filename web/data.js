
export const state_machine = {
	states: {
		state_0: {
			pos: [2, 2],
			size: [15, 15],
			title: 'State 1',
			text: ['entry/ entry_func()', 'exit/ exit_func()'],
			connectors: ['conn_0', 'conn_2']
			},
		state_1: {
			pos: [2, 25],
			size: [15, 15],
			title: 'State 2',
			text: ['entry/ entry_func()', 'exit/ exit_func()'],
			connectors: ['conn_1', 'conn_3']
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
			vertices: [[17, 7], [22, 7], [22, 35], [17, 35]]
		},
		trans_1: {
			start: 'conn_2',
			end: 'conn_3',
			vertices: [[7, 17], [7, 20], [7, 20], [7, 25]]
		}
	},

};

