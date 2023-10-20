#pragma once

// TODO: Refactor the rest of the code to use this keycodes

#define LG_KEY_SPACE			1
#define LG_KEY_APOSTROPHE		2 /* ' */
#define LG_KEY_COMMA			3 /*  */
#define LG_KEY_MINUS			4 /* - */
#define LG_KEY_PERIOD			5 /* . */
#define LG_KEY_SLASH			6 /* / */
#define LG_KEY_0				7
#define LG_KEY_1				8
#define LG_KEY_2				9
#define LG_KEY_3				10
#define LG_KEY_4				11
#define LG_KEY_5				12
#define LG_KEY_6				13
#define LG_KEY_7				14
#define LG_KEY_8				15
#define LG_KEY_9				16
#define LG_KEY_SEMICOLON		17 /* ; */
#define LG_KEY_EQUAL			18 /*  */
#define LG_KEY_A				19
#define LG_KEY_B				20
#define LG_KEY_C				21
#define LG_KEY_D				22
#define LG_KEY_E				23
#define LG_KEY_F				24
#define LG_KEY_G				25
#define LG_KEY_H				26
#define LG_KEY_I				27
#define LG_KEY_J				28
#define LG_KEY_K				29
#define LG_KEY_L				30
#define LG_KEY_M				31
#define LG_KEY_N				32
#define LG_KEY_O				33
#define LG_KEY_P				34
#define LG_KEY_Q				35
#define LG_KEY_R				36
#define LG_KEY_S				37
#define LG_KEY_T				38
#define LG_KEY_U				39
#define LG_KEY_V				40
#define LG_KEY_W				41
#define LG_KEY_X				42
#define LG_KEY_Y				43
#define LG_KEY_Z				44
#define LG_KEY_LEFT_BRACKET		45 /* [ */
#define LG_KEY_BACKSLASH		46 /* \ */
#define LG_KEY_RIGHT_BRACKET	47 /* ] */
#define LG_KEY_GRAVE_ACCENT		48 /* ` */
#define LG_KEY_WORLD_1			49 /* non-US #1 */
#define LG_KEY_WORLD_2			50 /* non-US #2 */
#define LG_KEY_ESCAPE			51
#define LG_KEY_ENTER			52
#define LG_KEY_TAB				53
#define LG_KEY_BACKSPACE		54
#define LG_KEY_INSERT			55
#define LG_KEY_DELETE			56
#define LG_KEY_RIGHT			57
#define LG_KEY_LEFT				58
#define LG_KEY_DOWN				59
#define LG_KEY_UP				60
#define LG_KEY_PAGE_UP			61
#define LG_KEY_PAGE_DOWN		62
#define LG_KEY_HOME				63
#define LG_KEY_END				64
#define LG_KEY_CAPS_LOCK		65
#define LG_KEY_SCROLL_LOCK		66
#define LG_KEY_NUM_LOCK			67
#define LG_KEY_PRINT_SCREEN		68
#define LG_KEY_PAUSE			69
#define LG_KEY_F1				70
#define LG_KEY_F2				71
#define LG_KEY_F3				72
#define LG_KEY_F4				73
#define LG_KEY_F5				74
#define LG_KEY_F6				75
#define LG_KEY_F7				76
#define LG_KEY_F8				77
#define LG_KEY_F9				78
#define LG_KEY_F10				79
#define LG_KEY_F11				80
#define LG_KEY_F12				81
#define LG_KEY_F13				82
#define LG_KEY_F14				83
#define LG_KEY_F15				84
#define LG_KEY_F16				85
#define LG_KEY_F17				86
#define LG_KEY_F18				87
#define LG_KEY_F19				88
#define LG_KEY_F20				89
#define LG_KEY_F21				90
#define LG_KEY_F22				91
#define LG_KEY_F23				92
#define LG_KEY_F24				93
#define LG_KEY_F25				94
#define LG_KEY_KP_0				95
#define LG_KEY_KP_1				96
#define LG_KEY_KP_2				97
#define LG_KEY_KP_3				98
#define LG_KEY_KP_4				99
#define LG_KEY_KP_5				100
#define LG_KEY_KP_6				101
#define LG_KEY_KP_7				102
#define LG_KEY_KP_8				103
#define LG_KEY_KP_9				104
#define LG_KEY_KP_DECIMAL		105
#define LG_KEY_KP_DIVIDE		106
#define LG_KEY_KP_MULTIPLY		107
#define LG_KEY_KP_SUBTRACT		108
#define LG_KEY_KP_ADD			109
#define LG_KEY_KP_ENTER			110
#define LG_KEY_KP_EQUAL			111
#define LG_KEY_LEFT_SHIFT		112
#define LG_KEY_LEFT_CONTROL		113
#define LG_KEY_LEFT_ALT			114
#define LG_KEY_LEFT_SUPER		115
#define LG_KEY_RIGHT_SHIFT		116
#define LG_KEY_RIGHT_CONTROL	117
#define LG_KEY_RIGHT_ALT		118
#define LG_KEY_RIGHT_SUPER		119
#define LG_KEY_MENU				120
#define LG_KEY_LAST				LG_KEY_MENU

namespace L3gion
{
	// GLFW keyCodes (glfw3.h)
	enum LgGlfwKeys : int
	{
		GLFW_ENUM_KEY_SPACE = 32,
		GLFW_ENUM_KEY_APOSTROPHE = 39,  /* ' */
		GLFW_ENUM_KEY_COMMA = 44,  /* , */
		GLFW_ENUM_KEY_MINUS = 45,  /* - */
		GLFW_ENUM_KEY_PERIOD = 46,  /* . */
		GLFW_ENUM_KEY_SLASH = 47,  /* / */
		GLFW_ENUM_KEY_0 = 48,
		GLFW_ENUM_KEY_1 = 49,
		GLFW_ENUM_KEY_2 = 50,
		GLFW_ENUM_KEY_3 = 51,
		GLFW_ENUM_KEY_4 = 52,
		GLFW_ENUM_KEY_5 = 53,
		GLFW_ENUM_KEY_6 = 54,
		GLFW_ENUM_KEY_7 = 55,
		GLFW_ENUM_KEY_8 = 56,
		GLFW_ENUM_KEY_9 = 57,
		GLFW_ENUM_KEY_SEMICOLON = 59,  /* ; */
		GLFW_ENUM_KEY_EQUAL = 61,  /* = */
		GLFW_ENUM_KEY_A = 65,
		GLFW_ENUM_KEY_B = 66,
		GLFW_ENUM_KEY_C = 67,
		GLFW_ENUM_KEY_D = 68,
		GLFW_ENUM_KEY_E = 69,
		GLFW_ENUM_KEY_F = 70,
		GLFW_ENUM_KEY_G = 71,
		GLFW_ENUM_KEY_H = 72,
		GLFW_ENUM_KEY_I = 73,
		GLFW_ENUM_KEY_J = 74,
		GLFW_ENUM_KEY_K = 75,
		GLFW_ENUM_KEY_L = 76,
		GLFW_ENUM_KEY_M = 77,
		GLFW_ENUM_KEY_N = 78,
		GLFW_ENUM_KEY_O = 79,
		GLFW_ENUM_KEY_P = 80,
		GLFW_ENUM_KEY_Q = 81,
		GLFW_ENUM_KEY_R = 82,
		GLFW_ENUM_KEY_S = 83,
		GLFW_ENUM_KEY_T = 84,
		GLFW_ENUM_KEY_U = 85,
		GLFW_ENUM_KEY_V = 86,
		GLFW_ENUM_KEY_W = 87,
		GLFW_ENUM_KEY_X = 88,
		GLFW_ENUM_KEY_Y = 89,
		GLFW_ENUM_KEY_Z = 90,
		GLFW_ENUM_KEY_LEFT_BRACKET = 91,  /* [ */
		GLFW_ENUM_KEY_BACKSLASH = 92,  /* \ */
		GLFW_ENUM_KEY_RIGHT_BRACKET = 93,  /* ] */
		GLFW_ENUM_KEY_GRAVE_ACCENT = 96,  /* ` */
		GLFW_ENUM_KEY_WORLD_1 = 161, /* non-US #1 */
		GLFW_ENUM_KEY_WORLD_2 = 162, /* non-US #2
		* Function keys */
		GLFW_ENUM_KEY_ESCAPE = 256,
		GLFW_ENUM_KEY_ENTER = 257,
		GLFW_ENUM_KEY_TAB = 258,
		GLFW_ENUM_KEY_BACKSPACE = 259,
		GLFW_ENUM_KEY_INSERT = 260,
		GLFW_ENUM_KEY_DELETE = 261,
		GLFW_ENUM_KEY_RIGHT = 262,
		GLFW_ENUM_KEY_LEFT = 263,
		GLFW_ENUM_KEY_DOWN = 264,
		GLFW_ENUM_KEY_UP = 265,
		GLFW_ENUM_KEY_PAGE_UP = 266,
		GLFW_ENUM_KEY_PAGE_DOWN = 267,
		GLFW_ENUM_KEY_HOME = 268,
		GLFW_ENUM_KEY_END = 269,
		GLFW_ENUM_KEY_CAPS_LOCK = 280,
		GLFW_ENUM_KEY_SCROLL_LOCK = 281,
		GLFW_ENUM_KEY_NUM_LOCK = 282,
		GLFW_ENUM_KEY_PRINT_SCREEN = 283,
		GLFW_ENUM_KEY_PAUSE = 284,
		GLFW_ENUM_KEY_F1 = 290,
		GLFW_ENUM_KEY_F2 = 291,
		GLFW_ENUM_KEY_F3 = 292,
		GLFW_ENUM_KEY_F4 = 293,
		GLFW_ENUM_KEY_F5 = 294,
		GLFW_ENUM_KEY_F6 = 295,
		GLFW_ENUM_KEY_F7 = 296,
		GLFW_ENUM_KEY_F8 = 297,
		GLFW_ENUM_KEY_F9 = 298,
		GLFW_ENUM_KEY_F10 = 299,
		GLFW_ENUM_KEY_F11 = 300,
		GLFW_ENUM_KEY_F12 = 301,
		GLFW_ENUM_KEY_F13 = 302,
		GLFW_ENUM_KEY_F14 = 303,
		GLFW_ENUM_KEY_F15 = 304,
		GLFW_ENUM_KEY_F16 = 305,
		GLFW_ENUM_KEY_F17 = 306,
		GLFW_ENUM_KEY_F18 = 307,
		GLFW_ENUM_KEY_F19 = 308,
		GLFW_ENUM_KEY_F20 = 309,
		GLFW_ENUM_KEY_F21 = 310,
		GLFW_ENUM_KEY_F22 = 311,
		GLFW_ENUM_KEY_F23 = 312,
		GLFW_ENUM_KEY_F24 = 313,
		GLFW_ENUM_KEY_F25 = 314,
		GLFW_ENUM_KEY_KP_0 = 320,
		GLFW_ENUM_KEY_KP_1 = 321,
		GLFW_ENUM_KEY_KP_2 = 322,
		GLFW_ENUM_KEY_KP_3 = 323,
		GLFW_ENUM_KEY_KP_4 = 324,
		GLFW_ENUM_KEY_KP_5 = 325,
		GLFW_ENUM_KEY_KP_6 = 326,
		GLFW_ENUM_KEY_KP_7 = 327,
		GLFW_ENUM_KEY_KP_8 = 328,
		GLFW_ENUM_KEY_KP_9 = 329,
		GLFW_ENUM_KEY_KP_DECIMAL = 330,
		GLFW_ENUM_KEY_KP_DIVIDE = 331,
		GLFW_ENUM_KEY_KP_MULTIPLY = 332,
		GLFW_ENUM_KEY_KP_SUBTRACT = 333,
		GLFW_ENUM_KEY_KP_ADD = 334,
		GLFW_ENUM_KEY_KP_ENTER = 335,
		GLFW_ENUM_KEY_KP_EQUAL = 336,
		GLFW_ENUM_KEY_LEFT_SHIFT = 340,
		GLFW_ENUM_KEY_LEFT_CONTROL = 341,
		GLFW_ENUM_KEY_LEFT_ALT = 342,
		GLFW_ENUM_KEY_LEFT_SUPER = 343,
		GLFW_ENUM_KEY_RIGHT_SHIFT = 344,
		GLFW_ENUM_KEY_RIGHT_CONTROL = 345,
		GLFW_ENUM_KEY_RIGHT_ALT = 346,
		GLFW_ENUM_KEY_RIGHT_SUPER = 347,
		GLFW_ENUM_KEY_MENU = 348,
		GLFW_ENUM_KEY_LAST = LG_KEY_MENU
	};

	// ImGui
	enum LgImGui : int
	{
		LG_IMGUI_KEY_Space = 524,
		LG_IMGUI_KEY_Apostrophe = 596,
		LG_IMGUI_KEY_Comma = 597,
		LG_IMGUI_KEY_Minus = 598,
		LG_IMGUI_KEY_Period = 599,
		LG_IMGUI_KEY_Slash = 600,
		LG_IMGUI_KEY_0 = 536,
		LG_IMGUI_KEY_1,
		LG_IMGUI_KEY_2,
		LG_IMGUI_KEY_3,
		LG_IMGUI_KEY_4,
		LG_IMGUI_KEY_5,
		LG_IMGUI_KEY_6,
		LG_IMGUI_KEY_7,
		LG_IMGUI_KEY_8,
		LG_IMGUI_KEY_9,
		LG_IMGUI_KEY_Semicolon = 601,
		LG_IMGUI_KEY_Equal,
		LG_IMGUI_KEY_A = 546,
		LG_IMGUI_KEY_B,
		LG_IMGUI_KEY_C,
		LG_IMGUI_KEY_D,
		LG_IMGUI_KEY_E,
		LG_IMGUI_KEY_F,
		LG_IMGUI_KEY_G,
		LG_IMGUI_KEY_H,
		LG_IMGUI_KEY_I,
		LG_IMGUI_KEY_J,
		LG_IMGUI_KEY_K,
		LG_IMGUI_KEY_L,
		LG_IMGUI_KEY_M,
		LG_IMGUI_KEY_N,
		LG_IMGUI_KEY_O,
		LG_IMGUI_KEY_P,
		LG_IMGUI_KEY_Q,
		LG_IMGUI_KEY_R,
		LG_IMGUI_KEY_S,
		LG_IMGUI_KEY_T,
		LG_IMGUI_KEY_U,
		LG_IMGUI_KEY_V,
		LG_IMGUI_KEY_W,
		LG_IMGUI_KEY_X,
		LG_IMGUI_KEY_Y,
		LG_IMGUI_KEY_Z,
		LG_IMGUI_KEY_LeftBracket = 603,
		LG_IMGUI_KEY_Backslash,
		LG_IMGUI_KEY_RightBracket,
		LG_IMGUI_KEY_GraveAccent,
		LG_IMGUI_KEY_World_1 = 0, // Currently NONE, i have no idea what World1 means in GLFW
		LG_IMGUI_KEY_World_2 = 0, // Currently NONE, i have no idea what World2 means in GLFW
		LG_IMGUI_KEY_Escape = 526,
		LG_IMGUI_KEY_Enter = 525,
		LG_IMGUI_KEY_Tab = 512,
		LG_IMGUI_KEY_Backspace = 523,
		LG_IMGUI_KEY_Insert = 521,
		LG_IMGUI_KEY_Delete,
		LG_IMGUI_KEY_RightArrow = 514,
		LG_IMGUI_KEY_LeftArrow = 513,
		LG_IMGUI_KEY_DownArrow = 516,
		LG_IMGUI_KEY_UpArrow = 515,
		LG_IMGUI_KEY_PageUp = 517,
		LG_IMGUI_KEY_PageDown,
		LG_IMGUI_KEY_Home,
		LG_IMGUI_KEY_End,
		LG_IMGUI_KEY_CapsLock = 607,
		LG_IMGUI_KEY_ScrollLock,
		LG_IMGUI_KEY_NumLock,
		LG_IMGUI_KEY_PrintScreen,
		LG_IMGUI_KEY_Pause,
		LG_IMGUI_KEY_F1 = 572,
		LG_IMGUI_KEY_F2,
		LG_IMGUI_KEY_F3,
		LG_IMGUI_KEY_F4,
		LG_IMGUI_KEY_F5,
		LG_IMGUI_KEY_F6,
		LG_IMGUI_KEY_F7,
		LG_IMGUI_KEY_F8,
		LG_IMGUI_KEY_F9,
		LG_IMGUI_KEY_F10,
		LG_IMGUI_KEY_F11,
		LG_IMGUI_KEY_F12,
		LG_IMGUI_KEY_F13,
		LG_IMGUI_KEY_F14,
		LG_IMGUI_KEY_F15,
		LG_IMGUI_KEY_F16,
		LG_IMGUI_KEY_F17,
		LG_IMGUI_KEY_F18,
		LG_IMGUI_KEY_F19,
		LG_IMGUI_KEY_F20,
		LG_IMGUI_KEY_F21,
		LG_IMGUI_KEY_F22,
		LG_IMGUI_KEY_F23,
		LG_IMGUI_KEY_F24,
		LG_IMGUI_KEY_F25 = 0, // None
		LG_IMGUI_KEY_Keypad0 = 612,
		LG_IMGUI_KEY_Keypad1,
		LG_IMGUI_KEY_Keypad2,
		LG_IMGUI_KEY_Keypad3,
		LG_IMGUI_KEY_Keypad4,
		LG_IMGUI_KEY_Keypad5,
		LG_IMGUI_KEY_Keypad6,
		LG_IMGUI_KEY_Keypad7,
		LG_IMGUI_KEY_Keypad8,
		LG_IMGUI_KEY_Keypad9,
		LG_IMGUI_KEY_KeypadDecimal,
		LG_IMGUI_KEY_KeypadDivide,
		LG_IMGUI_KEY_KeypadMultiply,
		LG_IMGUI_KEY_KeypadSubtract,
		LG_IMGUI_KEY_KeypadAdd,
		LG_IMGUI_KEY_KeypadEnter,
		LG_IMGUI_KEY_KeypadEqual,
		LG_IMGUI_KEY_LeftShift = 528,
		LG_IMGUI_KEY_LeftCtrl = 527,
		LG_IMGUI_KEY_LeftAlt = 529,
		LG_IMGUI_KEY_LeftSuper,
		LG_IMGUI_KEY_RightShift = 532,
		LG_IMGUI_KEY_RightCtrl = 531,
		LG_IMGUI_KEY_RightAlt = 533,
		LG_IMGUI_KEY_RightSuper,
		LG_IMGUI_KEY_Menu,
		LG_IMGUI_KEY_Last = LG_IMGUI_KEY_Menu
	};

	const std::vector<int> glfwKeysBuffer =
	{
		GLFW_ENUM_KEY_SPACE,
		GLFW_ENUM_KEY_APOSTROPHE,
		GLFW_ENUM_KEY_COMMA,
		GLFW_ENUM_KEY_MINUS,
		GLFW_ENUM_KEY_PERIOD,
		GLFW_ENUM_KEY_SLASH,
		GLFW_ENUM_KEY_0,
		GLFW_ENUM_KEY_1,
		GLFW_ENUM_KEY_2,
		GLFW_ENUM_KEY_3,
		GLFW_ENUM_KEY_4,
		GLFW_ENUM_KEY_5,
		GLFW_ENUM_KEY_6,
		GLFW_ENUM_KEY_7,
		GLFW_ENUM_KEY_8,
		GLFW_ENUM_KEY_9,
		GLFW_ENUM_KEY_SEMICOLON,
		GLFW_ENUM_KEY_EQUAL,
		GLFW_ENUM_KEY_A,
		GLFW_ENUM_KEY_B,
		GLFW_ENUM_KEY_C,
		GLFW_ENUM_KEY_D,
		GLFW_ENUM_KEY_E,
		GLFW_ENUM_KEY_F,
		GLFW_ENUM_KEY_G,
		GLFW_ENUM_KEY_H,
		GLFW_ENUM_KEY_I,
		GLFW_ENUM_KEY_J,
		GLFW_ENUM_KEY_K,
		GLFW_ENUM_KEY_L,
		GLFW_ENUM_KEY_M,
		GLFW_ENUM_KEY_N,
		GLFW_ENUM_KEY_O,
		GLFW_ENUM_KEY_P,
		GLFW_ENUM_KEY_Q,
		GLFW_ENUM_KEY_R,
		GLFW_ENUM_KEY_S,
		GLFW_ENUM_KEY_T,
		GLFW_ENUM_KEY_U,
		GLFW_ENUM_KEY_V,
		GLFW_ENUM_KEY_W,
		GLFW_ENUM_KEY_X,
		GLFW_ENUM_KEY_Y,
		GLFW_ENUM_KEY_Z,
		GLFW_ENUM_KEY_LEFT_BRACKET,
		GLFW_ENUM_KEY_BACKSLASH,
		GLFW_ENUM_KEY_RIGHT_BRACKET,
		GLFW_ENUM_KEY_GRAVE_ACCENT,
		GLFW_ENUM_KEY_WORLD_1,
		GLFW_ENUM_KEY_WORLD_2,
		GLFW_ENUM_KEY_ESCAPE,
		GLFW_ENUM_KEY_ENTER,
		GLFW_ENUM_KEY_TAB,
		GLFW_ENUM_KEY_BACKSPACE,
		GLFW_ENUM_KEY_INSERT,
		GLFW_ENUM_KEY_DELETE,
		GLFW_ENUM_KEY_RIGHT,
		GLFW_ENUM_KEY_LEFT,
		GLFW_ENUM_KEY_DOWN,
		GLFW_ENUM_KEY_UP,
		GLFW_ENUM_KEY_PAGE_UP,
		GLFW_ENUM_KEY_PAGE_DOWN,
		GLFW_ENUM_KEY_HOME,
		GLFW_ENUM_KEY_END,
		GLFW_ENUM_KEY_CAPS_LOCK,
		GLFW_ENUM_KEY_SCROLL_LOCK,
		GLFW_ENUM_KEY_NUM_LOCK,
		GLFW_ENUM_KEY_PRINT_SCREEN,
		GLFW_ENUM_KEY_PAUSE,
		GLFW_ENUM_KEY_F1,
		GLFW_ENUM_KEY_F2,
		GLFW_ENUM_KEY_F3,
		GLFW_ENUM_KEY_F4,
		GLFW_ENUM_KEY_F5,
		GLFW_ENUM_KEY_F6,
		GLFW_ENUM_KEY_F7,
		GLFW_ENUM_KEY_F8,
		GLFW_ENUM_KEY_F9,
		GLFW_ENUM_KEY_F10,
		GLFW_ENUM_KEY_F11,
		GLFW_ENUM_KEY_F12,
		GLFW_ENUM_KEY_F13,
		GLFW_ENUM_KEY_F14,
		GLFW_ENUM_KEY_F15,
		GLFW_ENUM_KEY_F16,
		GLFW_ENUM_KEY_F17,
		GLFW_ENUM_KEY_F18,
		GLFW_ENUM_KEY_F19,
		GLFW_ENUM_KEY_F20,
		GLFW_ENUM_KEY_F21,
		GLFW_ENUM_KEY_F22,
		GLFW_ENUM_KEY_F23,
		GLFW_ENUM_KEY_F24,
		GLFW_ENUM_KEY_F25,
		GLFW_ENUM_KEY_KP_0,
		GLFW_ENUM_KEY_KP_1,
		GLFW_ENUM_KEY_KP_2,
		GLFW_ENUM_KEY_KP_3,
		GLFW_ENUM_KEY_KP_4,
		GLFW_ENUM_KEY_KP_5,
		GLFW_ENUM_KEY_KP_6,
		GLFW_ENUM_KEY_KP_7,
		GLFW_ENUM_KEY_KP_8,
		GLFW_ENUM_KEY_KP_9,
		GLFW_ENUM_KEY_KP_DECIMAL,
		GLFW_ENUM_KEY_KP_DIVIDE,
		GLFW_ENUM_KEY_KP_MULTIPLY,
		GLFW_ENUM_KEY_KP_SUBTRACT,
		GLFW_ENUM_KEY_KP_ADD,
		GLFW_ENUM_KEY_KP_ENTER,
		GLFW_ENUM_KEY_KP_EQUAL,
		GLFW_ENUM_KEY_LEFT_SHIFT,
		GLFW_ENUM_KEY_LEFT_CONTROL,
		GLFW_ENUM_KEY_LEFT_ALT,
		GLFW_ENUM_KEY_LEFT_SUPER,
		GLFW_ENUM_KEY_RIGHT_SHIFT,
		GLFW_ENUM_KEY_RIGHT_CONTROL,
		GLFW_ENUM_KEY_RIGHT_ALT,
		GLFW_ENUM_KEY_RIGHT_SUPER,
		GLFW_ENUM_KEY_MENU,
		GLFW_ENUM_KEY_LAST
	};

	const std::vector<int> imGuiKeysBuffer =
	{
		LG_IMGUI_KEY_Space,
		LG_IMGUI_KEY_Apostrophe,
		LG_IMGUI_KEY_Comma,
		LG_IMGUI_KEY_Minus,
		LG_IMGUI_KEY_Period,
		LG_IMGUI_KEY_Slash,
		LG_IMGUI_KEY_0,
		LG_IMGUI_KEY_1,
		LG_IMGUI_KEY_2,
		LG_IMGUI_KEY_3,
		LG_IMGUI_KEY_4,
		LG_IMGUI_KEY_5,
		LG_IMGUI_KEY_6,
		LG_IMGUI_KEY_7,
		LG_IMGUI_KEY_8,
		LG_IMGUI_KEY_9,
		LG_IMGUI_KEY_Semicolon,
		LG_IMGUI_KEY_Equal,
		LG_IMGUI_KEY_A,
		LG_IMGUI_KEY_B,
		LG_IMGUI_KEY_C,
		LG_IMGUI_KEY_D,
		LG_IMGUI_KEY_E,
		LG_IMGUI_KEY_F,
		LG_IMGUI_KEY_G,
		LG_IMGUI_KEY_H,
		LG_IMGUI_KEY_I,
		LG_IMGUI_KEY_J,
		LG_IMGUI_KEY_K,
		LG_IMGUI_KEY_L,
		LG_IMGUI_KEY_M,
		LG_IMGUI_KEY_N,
		LG_IMGUI_KEY_O,
		LG_IMGUI_KEY_P,
		LG_IMGUI_KEY_Q,
		LG_IMGUI_KEY_R,
		LG_IMGUI_KEY_S,
		LG_IMGUI_KEY_T,
		LG_IMGUI_KEY_U,
		LG_IMGUI_KEY_V,
		LG_IMGUI_KEY_W,
		LG_IMGUI_KEY_X,
		LG_IMGUI_KEY_Y,
		LG_IMGUI_KEY_Z,
		LG_IMGUI_KEY_LeftBracket,
		LG_IMGUI_KEY_Backslash,
		LG_IMGUI_KEY_RightBracket,
		LG_IMGUI_KEY_GraveAccent,
		LG_IMGUI_KEY_World_1,
		LG_IMGUI_KEY_World_2,
		LG_IMGUI_KEY_Escape,
		LG_IMGUI_KEY_Enter,
		LG_IMGUI_KEY_Tab,
		LG_IMGUI_KEY_Backspace,
		LG_IMGUI_KEY_Insert,
		LG_IMGUI_KEY_Delete,
		LG_IMGUI_KEY_RightArrow,
		LG_IMGUI_KEY_LeftArrow,
		LG_IMGUI_KEY_DownArrow,
		LG_IMGUI_KEY_UpArrow,
		LG_IMGUI_KEY_PageUp,
		LG_IMGUI_KEY_PageDown,
		LG_IMGUI_KEY_Home,
		LG_IMGUI_KEY_End,
		LG_IMGUI_KEY_CapsLock,
		LG_IMGUI_KEY_ScrollLock,
		LG_IMGUI_KEY_NumLock,
		LG_IMGUI_KEY_PrintScreen,
		LG_IMGUI_KEY_Pause,
		LG_IMGUI_KEY_F1,
		LG_IMGUI_KEY_F2,
		LG_IMGUI_KEY_F3,
		LG_IMGUI_KEY_F4,
		LG_IMGUI_KEY_F5,
		LG_IMGUI_KEY_F6,
		LG_IMGUI_KEY_F7,
		LG_IMGUI_KEY_F8,
		LG_IMGUI_KEY_F9,
		LG_IMGUI_KEY_F10,
		LG_IMGUI_KEY_F11,
		LG_IMGUI_KEY_F12,
		LG_IMGUI_KEY_F13,
		LG_IMGUI_KEY_F14,
		LG_IMGUI_KEY_F15,
		LG_IMGUI_KEY_F16,
		LG_IMGUI_KEY_F17,
		LG_IMGUI_KEY_F18,
		LG_IMGUI_KEY_F19,
		LG_IMGUI_KEY_F20,
		LG_IMGUI_KEY_F21,
		LG_IMGUI_KEY_F22,
		LG_IMGUI_KEY_F23,
		LG_IMGUI_KEY_F24,
		LG_IMGUI_KEY_F25,
		LG_IMGUI_KEY_Keypad0,
		LG_IMGUI_KEY_Keypad1,
		LG_IMGUI_KEY_Keypad2,
		LG_IMGUI_KEY_Keypad3,
		LG_IMGUI_KEY_Keypad4,
		LG_IMGUI_KEY_Keypad5,
		LG_IMGUI_KEY_Keypad6,
		LG_IMGUI_KEY_Keypad7,
		LG_IMGUI_KEY_Keypad8,
		LG_IMGUI_KEY_Keypad9,
		LG_IMGUI_KEY_KeypadDecimal,
		LG_IMGUI_KEY_KeypadDivide,
		LG_IMGUI_KEY_KeypadMultiply,
		LG_IMGUI_KEY_KeypadSubtract,
		LG_IMGUI_KEY_KeypadAdd,
		LG_IMGUI_KEY_KeypadEnter,
		LG_IMGUI_KEY_KeypadEqual,
		LG_IMGUI_KEY_LeftShift,
		LG_IMGUI_KEY_LeftCtrl,
		LG_IMGUI_KEY_LeftAlt,
		LG_IMGUI_KEY_LeftSuper,
		LG_IMGUI_KEY_RightShift,
		LG_IMGUI_KEY_RightCtrl,
		LG_IMGUI_KEY_RightAlt,
		LG_IMGUI_KEY_RightSuper,
		LG_IMGUI_KEY_Menu,
		LG_IMGUI_KEY_Last
	};

	inline int LgKeyToGlfw(int keycode)
	{	
		int index = keycode - 1;
		return glfwKeysBuffer[index];
	}

	inline int LgKeyToImGui(int keycode)
	{
		int index = keycode - 1;
		return imGuiKeysBuffer[index];
	}

	int GlfwKeyToLG(int keycode);
	int ImGuiKeyToLG(int keycode);
}