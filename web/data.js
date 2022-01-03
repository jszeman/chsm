
export const state_machine = {
	"states": {
        "__top__": {
            "pos": [
                -99999997,
                -100000000
            ],
            "size": [
                200000000,
                200000000
            ],
            "title": "__top__4",
            "text": [],
            "connectors": [],
            "parent": null,
            "children": [
                "state_0",
                "istate_3"
            ],
            "type": "top"
        },
        "state_0": {
            "pos": [
                12,
                6
            ],
            "size": [
                136,
                55
            ],
            "title": "s",
            "text": [
                "entry/ s_entry()",
                "exit/ s_exit()",
                "init/ s_init()"
            ],
            "connectors": [
                "conn_13",
                "conn_26",
                "conn_37",
                "conn_39",
                "conn_44",
                "conn_47"
            ],
            "parent": "__top__",
            "children": [
                "state_1",
                "state_3",
                "istate_2",
                "state_6",
                "state_7"
            ],
            "type": "normal"
        },
        "state_1": {
            "pos": [
                17,
                19
            ],
            "size": [
                27,
                39
            ],
            "title": "s1",
            "text": [
                "entry/ s1_entry()",
                "exit/ s1_exit()",
                "init/ s1_init()",
                "[s1_guard()]/ s1_func()",
                "ID/ s1_func()"
            ],
            "connectors": [
                "conn_0",
                "conn_3",
                "conn_5",
                "conn_10",
                "conn_24",
                "conn_28",
                "conn_29",
                "conn_31",
                "conn_35",
                "conn_32"
            ],
            "parent": "state_0",
            "children": [
                "state_2",
                "istate_4"
            ],
            "type": "normal"
        },
        "state_2": {
            "pos": [
                23,
                33
            ],
            "size": [
                19,
                20
            ],
            "title": "s11",
            "text": [
                "entry/ s11_entry()",
                "exit/ s11_exit()",
                "init/ s11_init()",
                "ID/ s11_id(8)",
                "G [s11_g_guard1(1)]/ s11_g1(2)",
                "[s11_guard(6)]/ s11_func(7)",
                "K/ chsm_defer",
                "L/ chsm_defer"
            ],
            "connectors": [
                "conn_6",
                "conn_9",
                "conn_25",
                "conn_27",
                "conn_30",
                "conn_38",
                "conn_41",
                "conn_42"
            ],
            "parent": "state_1",
            "children": [],
            "type": "normal"
        },
        "state_3": {
            "pos": [
                63,
                18
            ],
            "size": [
                32,
                38
            ],
            "title": "s2",
            "text": [
                "entry/ s2_entry()",
                "exit/ s2_exit()",
                "init/ s2_init()"
            ],
            "connectors": [
                "conn_1",
                "conn_2",
                "conn_8",
                "conn_43",
                "conn_33"
            ],
            "parent": "state_0",
            "children": [
                "state_4",
                "istate_1"
            ],
            "type": "normal"
        },
        "state_4": {
            "pos": [
                65,
                28
            ],
            "size": [
                24,
                25
            ],
            "title": "s21",
            "text": [
                "entry/ s21_entry()",
                "exit/ s21_exit()",
                "init/ s21_init()"
            ],
            "connectors": [
                "conn_4",
                "conn_15",
                "conn_16",
                "conn_22",
                "conn_23",
                "conn_21"
            ],
            "parent": "state_3",
            "children": [
                "state_5",
                "istate_0"
            ],
            "type": "normal"
        },
        "state_5": {
            "pos": [
                69,
                38
            ],
            "size": [
                13,
                13
            ],
            "title": "s211",
            "text": [
                "entry/ s211_entry()",
                "exit/ s211_exit(6)",
                "init/ s211_init()",
                "ID/ s211_id()"
            ],
            "connectors": [
                "conn_7",
                "conn_11",
                "conn_12",
                "conn_14",
                "conn_17",
                "conn_19"
            ],
            "parent": "state_4",
            "children": [],
            "type": "normal"
        },
        "istate_0": {
            "pos": [
                87,
                38
            ],
            "size": [
                2,
                2
            ],
            "title": "istate_0",
            "text": [
                "entry/",
                "exit/"
            ],
            "connectors": [
                "conn_18"
            ],
            "parent": "state_4",
            "children": [],
            "type": "initial"
        },
        "istate_1": {
            "pos": [
                93,
                30
            ],
            "size": [
                2,
                2
            ],
            "title": "istate_1",
            "text": [
                "entry/",
                "exit/"
            ],
            "connectors": [
                "conn_20"
            ],
            "parent": "state_3",
            "children": [],
            "type": "initial"
        },
        "istate_2": {
            "pos": [
                50,
                16
            ],
            "size": [
                2,
                2
            ],
            "title": "istate_2",
            "text": [
                "entry/",
                "exit/"
            ],
            "connectors": [
                "conn_34"
            ],
            "parent": "state_0",
            "children": [],
            "type": "initial"
        },
        "istate_3": {
            "pos": [
                50,
                0
            ],
            "size": [
                2,
                2
            ],
            "title": "istate_3",
            "text": [
                "entry/",
                "exit/"
            ],
            "connectors": [
                "conn_36"
            ],
            "parent": "__top__",
            "children": [],
            "type": "initial"
        },
        "istate_4": {
            "pos": [
                19,
                33
            ],
            "size": [
                2,
                2
            ],
            "title": "istate_4",
            "text": [
                "entry/",
                "exit/"
            ],
            "connectors": [
                "conn_40"
            ],
            "parent": "state_1",
            "children": [],
            "type": "initial"
        },
        "state_6": {
            "pos": [
                98,
                30
            ],
            "size": [
                14,
                16
            ],
            "title": "s3",
            "text": [
                "entry/ chsm_recall",
                "exit/",
                "K/ s3_k_func()",
                "L/ s3_l_func()",
                ""
            ],
            "connectors": [
                "conn_45",
                "conn_46",
                "conn_48",
                "conn_50"
            ],
            "parent": "state_0",
            "children": [],
            "type": "normal"
        },
        "state_7": {
            "pos": [
                134,
                30
            ],
            "size": [
                8,
                16
            ],
            "title": "s4",
            "text": [
                "ID/ s4_id()"
            ],
            "connectors": [
                "conn_49",
                "conn_51"
            ],
            "parent": "state_0",
            "children": [],
            "type": "normal"
        }
    },
    "connectors": {
        "conn_0": {
            "parent": "state_1",
            "offset": 6,
            "side": "right",
            "dir": "out",
            "transition": "trans_0"
        },
        "conn_1": {
            "parent": "state_3",
            "offset": 7,
            "side": "left",
            "dir": "in",
            "transition": "trans_0"
        },
        "conn_2": {
            "parent": "state_3",
            "offset": 4,
            "side": "left",
            "dir": "out",
            "transition": "trans_1"
        },
        "conn_3": {
            "parent": "state_1",
            "offset": 3,
            "side": "right",
            "dir": "in",
            "transition": "trans_1"
        },
        "conn_4": {
            "parent": "state_4",
            "offset": 2,
            "side": "left",
            "dir": "out",
            "transition": "trans_2"
        },
        "conn_5": {
            "parent": "state_1",
            "offset": 11,
            "side": "right",
            "dir": "in",
            "transition": "trans_2"
        },
        "conn_6": {
            "parent": "state_2",
            "offset": 12,
            "side": "right",
            "dir": "out",
            "transition": "trans_3"
        },
        "conn_7": {
            "parent": "state_5",
            "offset": 5,
            "side": "left",
            "dir": "in",
            "transition": "trans_3"
        },
        "conn_8": {
            "parent": "state_3",
            "offset": 28,
            "side": "left",
            "dir": "out",
            "transition": "trans_4"
        },
        "conn_9": {
            "parent": "state_2",
            "offset": 15,
            "side": "right",
            "dir": "in",
            "transition": "trans_4"
        },
        "conn_10": {
            "parent": "state_1",
            "offset": 31,
            "side": "right",
            "dir": "out",
            "transition": "trans_5"
        },
        "conn_11": {
            "parent": "state_5",
            "offset": 12,
            "side": "left",
            "dir": "in",
            "transition": "trans_5"
        },
        "conn_12": {
            "parent": "state_5",
            "offset": 4,
            "side": "bottom",
            "dir": "out",
            "transition": "trans_6"
        },
        "conn_13": {
            "parent": "state_0",
            "offset": 61,
            "side": "bottom",
            "dir": "in",
            "transition": "trans_6"
        },
        "conn_14": {
            "parent": "state_5",
            "offset": 8,
            "side": "right",
            "dir": "out",
            "transition": "trans_7"
        },
        "conn_15": {
            "parent": "state_4",
            "offset": 18,
            "side": "right",
            "dir": "in",
            "transition": "trans_7"
        },
        "conn_16": {
            "parent": "state_4",
            "offset": 15,
            "side": "right",
            "dir": "out",
            "transition": "trans_8"
        },
        "conn_17": {
            "parent": "state_5",
            "offset": 5,
            "side": "right",
            "dir": "in",
            "transition": "trans_8"
        },
        "conn_18": {
            "parent": "istate_0",
            "offset": 0,
            "side": "all",
            "dir": "out",
            "transition": "trans_9"
        },
        "conn_19": {
            "parent": "state_5",
            "offset": 2,
            "side": "right",
            "dir": "in",
            "transition": "trans_9"
        },
        "conn_20": {
            "parent": "istate_1",
            "offset": 0,
            "side": "all",
            "dir": "out",
            "transition": "trans_10"
        },
        "conn_21": {
            "parent": "state_4",
            "offset": 2,
            "side": "right",
            "dir": "in",
            "transition": "trans_10"
        },
        "conn_22": {
            "parent": "state_4",
            "offset": 6,
            "side": "right",
            "dir": "out",
            "transition": "trans_11"
        },
        "conn_23": {
            "parent": "state_4",
            "offset": 14,
            "side": "right",
            "dir": "in",
            "transition": "trans_11"
        },
        "conn_24": {
            "parent": "state_1",
            "offset": 21,
            "side": "left",
            "dir": "out",
            "transition": "trans_12"
        },
        "conn_25": {
            "parent": "state_2",
            "offset": 9,
            "side": "left",
            "dir": "in",
            "transition": "trans_12"
        },
        "conn_26": {
            "parent": "state_0",
            "offset": 39,
            "side": "left",
            "dir": "out",
            "transition": "trans_13"
        },
        "conn_27": {
            "parent": "state_2",
            "offset": 14,
            "side": "left",
            "dir": "in",
            "transition": "trans_13"
        },
        "conn_28": {
            "parent": "state_1",
            "offset": 28,
            "side": "left",
            "dir": "out",
            "transition": "trans_14"
        },
        "conn_29": {
            "parent": "state_1",
            "offset": 33,
            "side": "left",
            "dir": "in",
            "transition": "trans_14"
        },
        "conn_30": {
            "parent": "state_2",
            "offset": 7,
            "side": "bottom",
            "dir": "out",
            "transition": "trans_15"
        },
        "conn_31": {
            "parent": "state_1",
            "offset": 13,
            "side": "bottom",
            "dir": "in",
            "transition": "trans_15"
        },
        "conn_34": {
            "parent": "istate_2",
            "offset": 0,
            "side": "all",
            "dir": "out",
            "transition": "trans_17"
        },
        "conn_35": {
            "parent": "state_1",
            "offset": 18,
            "side": "top",
            "dir": "in",
            "transition": "trans_17"
        },
        "conn_36": {
            "parent": "istate_3",
            "offset": 0,
            "side": "all",
            "dir": "out",
            "transition": "trans_18"
        },
        "conn_37": {
            "parent": "state_0",
            "offset": 38,
            "side": "top",
            "dir": "in",
            "transition": "trans_18"
        },
        "conn_38": {
            "parent": "state_2",
            "offset": 4,
            "side": "bottom",
            "dir": "out",
            "transition": "trans_19"
        },
        "conn_39": {
            "parent": "state_0",
            "offset": 15,
            "side": "bottom",
            "dir": "in",
            "transition": "trans_19"
        },
        "conn_40": {
            "parent": "istate_4",
            "offset": 0,
            "side": "all",
            "dir": "out",
            "transition": "trans_20"
        },
        "conn_41": {
            "parent": "state_2",
            "offset": 3,
            "side": "left",
            "dir": "in",
            "transition": "trans_20"
        },
        "conn_42": {
            "parent": "state_2",
            "offset": 6,
            "side": "right",
            "dir": "out",
            "transition": "trans_21"
        },
        "conn_43": {
            "parent": "state_3",
            "offset": 19,
            "side": "left",
            "dir": "in",
            "transition": "trans_21"
        },
        "conn_32": {
            "parent": "state_1",
            "offset": 35,
            "side": "right",
            "dir": "out",
            "transition": "trans_16"
        },
        "conn_33": {
            "parent": "state_3",
            "offset": 36,
            "side": "left",
            "dir": "in",
            "transition": "trans_16"
        },
        "conn_44": {
            "parent": "state_0",
            "offset": 15,
            "side": "right",
            "dir": "out",
            "transition": "trans_22"
        },
        "conn_45": {
            "parent": "state_6",
            "offset": 6,
            "side": "top",
            "dir": "in",
            "transition": "trans_22"
        },
        "conn_46": {
            "parent": "state_6",
            "offset": 7,
            "side": "bottom",
            "dir": "out",
            "transition": "trans_23"
        },
        "conn_47": {
            "parent": "state_0",
            "offset": 93,
            "side": "bottom",
            "dir": "in",
            "transition": "trans_23"
        },
        "conn_48": {
            "parent": "state_6",
            "offset": 4,
            "side": "right",
            "dir": "out",
            "transition": "trans_24"
        },
        "conn_49": {
            "parent": "state_7",
            "offset": 4,
            "side": "left",
            "dir": "in",
            "transition": "trans_24"
        },
        "conn_50": {
            "parent": "state_6",
            "offset": 9,
            "side": "right",
            "dir": "out",
            "transition": "trans_25"
        },
        "conn_51": {
            "parent": "state_7",
            "offset": 9,
            "side": "left",
            "dir": "in",
            "transition": "trans_25"
        }
    },
    "transitions": {
        "trans_0": {
            "start": "conn_0",
            "end": "conn_1",
            "vertices": [
                [
                    44,
                    25
                ],
                [
                    46,
                    25
                ],
                [
                    46,
                    25
                ],
                [
                    63,
                    25
                ]
            ],
            "label": "C",
            "label_offset": [
                2.5,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                46.5,
                24.6
            ]
        },
        "trans_1": {
            "start": "conn_2",
            "end": "conn_3",
            "vertices": [
                [
                    63,
                    22
                ],
                [
                    46,
                    22
                ],
                [
                    46,
                    22
                ],
                [
                    44,
                    22
                ]
            ],
            "label": "C",
            "label_offset": [
                -16.5,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                46.5,
                21.6
            ]
        },
        "trans_2": {
            "start": "conn_4",
            "end": "conn_5",
            "vertices": [
                [
                    65,
                    30
                ],
                [
                    46,
                    30
                ],
                [
                    46,
                    30
                ],
                [
                    44,
                    30
                ]
            ],
            "label": "G",
            "label_offset": [
                -18.5,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                46.5,
                29.6
            ]
        },
        "trans_3": {
            "start": "conn_6",
            "end": "conn_7",
            "vertices": [
                [
                    42,
                    45
                ],
                [
                    45,
                    45
                ],
                [
                    45,
                    43
                ],
                [
                    69,
                    43
                ]
            ],
            "label": "G [s11_g_guard2(4)]/ s11_g2(3)",
            "label_offset": [
                0.5,
                -2.3999999999999986
            ],
            "label_anchor": 1,
            "label_pos": [
                45.5,
                42.6
            ]
        },
        "trans_4": {
            "start": "conn_8",
            "end": "conn_9",
            "vertices": [
                [
                    63,
                    46
                ],
                [
                    45,
                    46
                ],
                [
                    45,
                    48
                ],
                [
                    42,
                    48
                ]
            ],
            "label": "F",
            "label_offset": [
                -17.5,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                45.5,
                45.6
            ]
        },
        "trans_5": {
            "start": "conn_10",
            "end": "conn_11",
            "vertices": [
                [
                    44,
                    50
                ],
                [
                    45,
                    50
                ],
                [
                    45,
                    50
                ],
                [
                    69,
                    50
                ]
            ],
            "label": "F",
            "label_offset": [
                1.5,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                45.5,
                49.6
            ]
        },
        "trans_6": {
            "start": "conn_12",
            "end": "conn_13",
            "vertices": [
                [
                    73,
                    51
                ],
                [
                    73,
                    56
                ],
                [
                    73,
                    56
                ],
                [
                    73,
                    61
                ]
            ],
            "label": "H",
            "label_offset": [
                0.5,
                4.600000000000001
            ],
            "label_anchor": 0,
            "label_pos": [
                73.5,
                55.6
            ]
        },
        "trans_7": {
            "start": "conn_14",
            "end": "conn_15",
            "vertices": [
                [
                    82,
                    46
                ],
                [
                    86,
                    46
                ],
                [
                    86,
                    46
                ],
                [
                    89,
                    46
                ]
            ],
            "label": "D",
            "label_offset": [
                4.5,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                86.5,
                45.6
            ]
        },
        "trans_8": {
            "start": "conn_16",
            "end": "conn_17",
            "vertices": [
                [
                    89,
                    43
                ],
                [
                    86,
                    43
                ],
                [
                    86,
                    43
                ],
                [
                    82,
                    43
                ]
            ],
            "label": "B",
            "label_offset": [
                -2.5,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                86.5,
                42.6
            ]
        },
        "trans_9": {
            "start": "conn_18",
            "end": "conn_19",
            "vertices": [
                [
                    87,
                    38
                ],
                [
                    87,
                    40
                ],
                [
                    82,
                    40
                ]
            ],
            "label": "",
            "label_offset": [
                -3.5,
                -0.3999999999999986
            ],
            "label_anchor": 1,
            "label_pos": [
                83.5,
                39.6
            ]
        },
        "trans_10": {
            "start": "conn_20",
            "end": "conn_21",
            "vertices": [
                [
                    93,
                    30
                ],
                [
                    93,
                    30
                ],
                [
                    89,
                    30
                ]
            ],
            "label": "",
            "label_offset": [
                0.5,
                -0.4
            ],
            "label_anchor": 1,
            "label_pos": [
                93.5,
                29.6
            ]
        },
        "trans_11": {
            "start": "conn_22",
            "end": "conn_23",
            "vertices": [
                [
                    89,
                    34
                ],
                [
                    93,
                    34
                ],
                [
                    93,
                    42
                ],
                [
                    89,
                    42
                ]
            ],
            "label": "A",
            "label_offset": [
                0.5,
                -0.4
            ],
            "label_anchor": 3,
            "label_pos": [
                89.5,
                41.6
            ]
        },
        "trans_12": {
            "start": "conn_24",
            "end": "conn_25",
            "vertices": [
                [
                    17,
                    40
                ],
                [
                    19,
                    40
                ],
                [
                    19,
                    42
                ],
                [
                    23,
                    42
                ]
            ],
            "label": "B",
            "label_offset": [
                0.5,
                1.6000000000000014
            ],
            "label_anchor": 1,
            "label_pos": [
                19.5,
                41.6
            ]
        },
        "trans_13": {
            "start": "conn_26",
            "end": "conn_27",
            "vertices": [
                [
                    12,
                    45
                ],
                [
                    18,
                    45
                ],
                [
                    18,
                    47
                ],
                [
                    23,
                    47
                ]
            ],
            "label": "E",
            "label_offset": [
                6.5,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                18.5,
                44.6
            ]
        },
        "trans_14": {
            "start": "conn_28",
            "end": "conn_29",
            "vertices": [
                [
                    17,
                    47
                ],
                [
                    14,
                    47
                ],
                [
                    14,
                    52
                ],
                [
                    17,
                    52
                ]
            ],
            "label": "A",
            "label_offset": [
                0.5,
                4.600000000000001
            ],
            "label_anchor": 1,
            "label_pos": [
                14.5,
                51.6
            ]
        },
        "trans_15": {
            "start": "conn_30",
            "end": "conn_31",
            "vertices": [
                [
                    30,
                    53
                ],
                [
                    30,
                    56
                ],
                [
                    30,
                    56
                ],
                [
                    30,
                    58
                ]
            ],
            "label": "D [cond()] / d_func() ",
            "label_offset": [
                0.5,
                2.6000000000000014
            ],
            "label_anchor": 0,
            "label_pos": [
                30.5,
                55.6
            ]
        },
        "trans_17": {
            "start": "conn_34",
            "end": "conn_35",
            "vertices": [
                [
                    50,
                    16
                ],
                [
                    35,
                    16
                ],
                [
                    35,
                    19
                ]
            ],
            "label": "",
            "label_offset": [
                -14.5,
                -0.40000000000000036
            ],
            "label_anchor": 0,
            "label_pos": [
                35.5,
                15.6
            ]
        },
        "trans_18": {
            "start": "conn_36",
            "end": "conn_37",
            "vertices": [
                [
                    50,
                    0
                ],
                [
                    50,
                    0
                ],
                [
                    50,
                    6
                ]
            ],
            "label": "",
            "label_offset": [
                0.5,
                -0.4
            ],
            "label_anchor": 1,
            "label_pos": [
                50.5,
                -0.4
            ]
        },
        "trans_19": {
            "start": "conn_38",
            "end": "conn_39",
            "vertices": [
                [
                    27,
                    53
                ],
                [
                    27,
                    53
                ],
                [
                    27,
                    53
                ],
                [
                    27,
                    61
                ]
            ],
            "label": "H",
            "label_offset": [
                0.5,
                2.8000000000000043
            ],
            "label_anchor": 2,
            "label_pos": [
                27.5,
                55.800000000000004
            ]
        },
        "trans_20": {
            "start": "conn_40",
            "end": "conn_41",
            "vertices": [
                [
                    19,
                    33
                ],
                [
                    19,
                    36
                ],
                [
                    23,
                    36
                ]
            ],
            "label": "",
            "label_offset": [
                0.5,
                2.6000000000000014
            ],
            "label_anchor": 0,
            "label_pos": [
                19.5,
                35.6
            ]
        },
        "trans_21": {
            "start": "conn_42",
            "end": "conn_43",
            "vertices": [
                [
                    42,
                    39
                ],
                [
                    45,
                    39
                ],
                [
                    45,
                    37
                ],
                [
                    63,
                    37
                ]
            ],
            "label": "[k_guard(5)]",
            "label_offset": [
                0.5,
                -2.3999999999999986
            ],
            "label_anchor": 1,
            "label_pos": [
                45.5,
                36.6
            ]
        },
        "trans_16": {
            "start": "conn_32",
            "end": "conn_33",
            "vertices": [
                [
                    44,
                    54
                ],
                [
                    45,
                    54
                ],
                [
                    45,
                    54
                ],
                [
                    63,
                    54
                ]
            ],
            "label": "[j_guard()]",
            "label_offset": [
                1.5,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                45.5,
                53.6
            ]
        },
        "trans_22": {
            "start": "conn_44",
            "end": "conn_45",
            "vertices": [
                [
                    148,
                    21
                ],
                [
                    104,
                    21
                ],
                [
                    104,
                    30
                ]
            ],
            "label": "J",
            "label_offset": [
                0.5,
                4.600000000000001
            ],
            "label_anchor": 1,
            "label_pos": [
                104.5,
                25.6
            ]
        },
        "trans_23": {
            "start": "conn_46",
            "end": "conn_47",
            "vertices": [
                [
                    105,
                    46
                ],
                [
                    105,
                    52
                ],
                [
                    105,
                    52
                ],
                [
                    105,
                    61
                ]
            ],
            "label": "J",
            "label_offset": [
                0.5,
                0.6000000000000014
            ],
            "label_anchor": 2,
            "label_pos": [
                105.5,
                52.6
            ]
        },
        "trans_24": {
            "start": "conn_48",
            "end": "conn_49",
            "vertices": [
                [
                    112,
                    34
                ],
                [
                    116,
                    34
                ],
                [
                    116,
                    34
                ],
                [
                    134,
                    34
                ]
            ],
            "label": "G [s11_g_guard2(4)]/ s11_g2(3)",
            "label_offset": [
                2,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                114,
                33.6
            ]
        },
        "trans_25": {
            "start": "conn_50",
            "end": "conn_51",
            "vertices": [
                [
                    112,
                    39
                ],
                [
                    123,
                    39
                ],
                [
                    123,
                    39
                ],
                [
                    134,
                    39
                ]
            ],
            "label": "G [s11_g_guard1(1)]/ s11_g1(2)",
            "label_offset": [
                2,
                -0.3999999999999986
            ],
            "label_anchor": 0,
            "label_pos": [
                114,
                38.6
            ]
        }
    },
    "notes": {
        "s1_entry()": "S1 entry function comment.",
        "s11_entry()": "S11 entry function comment",
        "s11_g_guard1()": "S11 G Guard",
        "s1_func()": "",
        "k_guard()": "",
        "s11_g_guard2()": "",
        "s11_g1": "",
        "s11_g_guard2": "",
        "s11_exit()": "",
        "s11_init()": "",
        "s1_exit()": "",
        "s11_g_guard1": "Comment for the s11_g_guard1 func.\n\nMultiple lines.",
        "F": "Comment for F",
        "G": "Comment for G",
        "k_guard": "Parametric guard comment.",
        "H": "",
        "trans_24": "",
        "s211_id()": ""
    },
    "view": {
        "translate": [
            -108,
            46.5
        ],
        "scale": 9.5
    }
};

