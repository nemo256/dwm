/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  	= 5;        /* border pixel size of windows */
static const unsigned int gappx     	= 5;        /* gaps size between windows */
static const unsigned int snap      	= 32;       /* snap pixel */
static const int showbar            	= 1;        /* 0 means no bar */
static const int topbar             	= 1;        /* 0 means bottom bar */
static const char *fonts[]          	= { "Fira Code:size=20:style=Bold",  "SymbolsNerdFont:size=22:style=Bold", "Arab:size=20:antialias=true:autohint=true:style=Bold" };
static const char dmenufont[]       	= "Fira Code:size=20:style=Bold";
//background color
static const char col_black[]       	= "#000000";
//inactive window border color
static const char col_gray2[]       	= "#444444";
//font color
static const char col_gray3[]       	= "#bbbbbb";
//current tag and current window font color
static const char col_gray4[]       	= "#000000";
//Top bar second color (orange) and active window border color
//#f59542
static const char col_indigo[]      	= "#4b0088";
static const unsigned int baralpha  	= 0xcc;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      	= {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_black, col_gray2 },
	[SchemeSel]  = { col_gray4, col_indigo,  col_indigo  },
};

static const unsigned int alphas[][3] = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
//tag names (upper left)
static const char *tags[] = { "", "", "{}", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance      title    		    tags mask  		switchtotag  isfloating  monitor */
	{ "firefox",  "Navigator",  NULL,  	 	      1,	    	 		1, 				   0,   			 -1 },
	{ "St",  			"st",       	"nmtui",   	    1 << 1,	   		1, 				   0,   			 -1 },
	{ "St",  			"st",       	"htop",   	    1 << 1,	   		1, 				   0,   			 -1 },
	{ "St",  			"st",       	"nvim",   	    1 << 2,	   		1, 				   0,   			 -1 },
	{ "St",  			"st",       	"tty-clock",   	1 << 2,	   		1, 				   0,   			 -1 },
	{ "St",  			"st",       	"newsboat",     1 << 3,	   		1, 				   0,   		 	 -1 },
	{ "St",  			"st",       	"irssi",  	    1 << 3,	   		1, 				   0,   			 -1 },
	{ "St",  			"st",       	"ani-cli",      1 << 3,	   		1, 				   0,   			 -1 },
	{ "St",  			"st",       	"pipe-viewer",  1 << 3,	   		1, 				   0,   			 -1 },
	{ "St",  			"st",       	"tremc",  			1 << 3,	   		1, 				   0,   			 -1 },
	{ "Zathura",  NULL,       	NULL,  					1 << 3,	   		1, 				   0,   			 -1 },
	{ "mpv",  		"gl",       	NULL,  			    1 << 3,	   		1, 				   0,   			 -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[M]",      monocle },   /* first entry is default */
	//{ "[]=",      tile },    /* first entry is default */
	//{ "><>",      NULL },    /* no layout function means floating behavior */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
// controls
static const char *upbrightness[] = { "brightnessctl", "s", "10%+", NULL };
static const char *downbrightness[] = { "brightnessctl", "s", "10%-", NULL };
static const char *upkbbacklight[] = { "/root/bin/kbbacklight", "UP", NULL };
static const char *downkbbacklight[] = { "/root/bin/kbbacklight", "DOWN", NULL };
static const char *batterylevel[] = { "/root/bin/batterylevel", NULL };
static const char *dunstclose[]   = { "dunstctl", "close", NULL };
static const char *dunstcloseall[]   = { "dunstctl", "close-all", NULL };
static const char *dunsthistory[]   = { "dunstctl", "history-pop", NULL };
static const char *playmusic[]   = { "mpc", "toggle", NULL };
static const char *playnext[]   = { "mpc", "next", NULL };
static const char *playprev[]   = { "mpc", "prev", NULL };
static const char *upvol[]   = { "amixer", "-q", "set", "Master", "5%+", "unmute", NULL };
static const char *downvol[] = { "amixer", "-q", "set", "Master", "5%-", "unmute", NULL };
static const char *mutevol[] = { "amixer", "-q", "set", "Master", "toggle", NULL };
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_black, "-nf", col_gray3, "-sb", col_indigo, "-sf", col_gray4, NULL };

// set st as the default terminal
static const char *termcmd[]  = { "st", NULL };

// terminal commands
static const char *shutdown[] = { "st", "-e", "systemctl", "poweroff",  NULL };
static const char *reboot[] = { "st", "-e", "systemctl", "reboot",  NULL };
static const char *ranger[] = { "st", "-e", "ranger", NULL };
static const char *htop[] = { "st", "-e", "htop", NULL };
static const char *vim[] = { "st", "-e", "nvim", NULL };
static const char *irssi[] = { "st", "-e", "irssi", NULL };
static const char *newsboat[] = { "st", "-e", "newsboat", NULL };
static const char *email[] = { "st", "-e", "neomutt", NULL };
static const char *clock[] = { "st", "-e", "tty-clock", "-scbC", "4", NULL };
static const char *pipeviewer[] = { "st", "-f", "'Fira Code:size=16:style=bold:antialias=true:autohint=true'", "-e", "pipe-viewer", NULL };
static const char *torrent[] = { "st", "-e", "tremc", NULL };
static const char *music[] = { "st", "-e", "ncmpcpp", NULL };
static const char *lynx[] = { "st", "-e", "lynx", NULL };
static const char *anime[] = { "st", "-e", "ani-cli", NULL };
static const char *wifi[] = { "st", "-e", "nmtui", NULL };
static const char *watch[] = { "/root/bin/watch", NULL };
static const char *record[] = { "/root/bin/record", NULL };
static const char *vpn[] = { "/root/bin/vpn", NULL };
static const char *mykeys[] = { "/root/bin/keys", NULL };
static const char *download[] = { "/root/bin/download", NULL };
static const char *mypointer[] = { "/root/bin/pointer", NULL };
static const char *screenshot[] = { "/root/bin/screenshot", NULL };
static const char *screenshotSelect[] = { "/root/bin/screenshotSelect", NULL };
static const char *photos[] = { "sxiv", "-ftqrb", "/root/Pictures", NULL };
/* static const char *tuir[] = { "st", "-e", "tuir", NULL }; */

// firefox command
static const char *firefox[] = { "firefox", NULL };

// websites
static const char *google[] = { "firefox", "https://www.google.com/", NULL };
static const char *facebook[] = { "firefox", "https://www.facebook.com/", NULL };
static const char *messenger[] = { "firefox", "https://www.messenger.com/", NULL };
static const char *netflix[] = { "firefox", "https://www.netflix.com/", NULL };
static const char *youtube[] = { "firefox", "https://www.youtube.com/", NULL };
static const char *instagram[] = { "firefox", "https://www.instagram.com/", NULL };
static const char *gmail[] = { "firefox", "https://mail.google.com/", NULL };
static const char *aliexpress[] = { "firefox", "https://www.aliexpress.com/", NULL };
static const char *ouedkniss[] = { "firefox", "https://www.ouedkniss.com/", NULL };
static const char *GPhotos[] = { "firefox", "https://photos.google.com/", NULL };
static const char *eccp[] = { "firefox", "https://eccp.poste.dz/", NULL };
static const char *univ[] = { "firefox", "https://fs.univ-boumerdes.dz/", NULL };
static const char *translate[] = { "firefox", "https://translate.google.com/", NULL };
static const char *magnetDL[] = { "firefox", "https://www.magnetdl.com/", NULL };
static const char *contacts[] = { "firefox", "https://contacts.google.com/", NULL };
static const char *github[] = { "firefox", "https://www.github.com/nemo256/", NULL };
static const char *localhost[] = { "firefox", "localhost:3000", NULL };

#include "shiftview.c"
static char *endx[] = { "/bin/sh", "-c", "endx", "externalpipe", NULL };
static Key keys[] = {
	/* modifier                     key        function        argument */
  /* system commands */
	{ 0,        XF86XK_MonBrightnessUp,       spawn,          {.v = upbrightness } },
	{ 0,        XF86XK_MonBrightnessDown,     spawn,          {.v = downbrightness } },
	{ 0,        XF86XK_KbdBrightnessUp,       spawn,          {.v = upkbbacklight } },
	{ 0,        XF86XK_KbdBrightnessDown,     spawn,          {.v = downkbbacklight } },
	{ 0,        XF86XK_AudioPlay,      				spawn,          {.v = playmusic } },
	{ 0,        XF86XK_AudioNext,      				spawn,          {.v = playnext } },
	{ 0,        XF86XK_AudioPrev,      				spawn,          {.v = playprev } },
	{ 0,        XF86XK_AudioRaiseVolume,      spawn,          {.v = upvol } },
	{ 0,        XF86XK_AudioLowerVolume,      spawn,          {.v = downvol } },
	{ 0,    		XF86XK_AudioMute,      spawn,          {.v = mutevol } },
	{ Mod1Mask,             	XK_space,      spawn,          {.v = dunstclose } },
	{ Mod1Mask|ShiftMask,     XK_space,      spawn,          {.v = dunstcloseall } },
	{ Mod1Mask,           		XK_grave,      spawn,          {.v = dunsthistory } },
	{ MODKEY|ShiftMask,       XK_x,      spawn,          {.v = shutdown } },
	{ MODKEY|ShiftMask,       XK_r,      spawn,          {.v = reboot } },
  /* terminal commands */
	{ MODKEY,	                XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                 XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                 XK_f,      spawn,          {.v = ranger } },
	{ MODKEY,                 XK_h,      spawn,          {.v = htop } },
	{ MODKEY,                 XK_v,      spawn,          {.v = vim } },
	{ MODKEY,                 XK_e,      spawn,          {.v = email } },
	{ MODKEY,                 XK_c,      spawn,          {.v = clock } },
	{ MODKEY,                 XK_y,      spawn,          {.v = pipeviewer } },
	{ MODKEY,                 XK_i,      spawn,          {.v = irssi } },
	{ MODKEY,                 XK_n,      spawn,          {.v = newsboat } },
	{ MODKEY,                 XK_r,      spawn,          {.v = record } },
	{ MODKEY|ShiftMask,       XK_v,      spawn,          {.v = vpn } },
	{ MODKEY,                 XK_k,      spawn,          {.v = mykeys } },
	{ MODKEY,                 XK_s,      spawn,          {.v = download } },
	{ MODKEY,                 XK_t,      spawn,          {.v = torrent } },
	{ MODKEY,                 XK_m,      spawn,          {.v = music } },
	{ MODKEY,                 XK_l,      spawn,          {.v = lynx } },
	{ MODKEY,                 XK_a,      spawn,          {.v = anime } },
	{ MODKEY,                 XK_grave,      spawn,          {.v = screenshot } },
	{ MODKEY|ShiftMask,       XK_grave,      spawn,          {.v = screenshotSelect} },
	{ ControlMask,            XK_grave,      spawn,          {.v = mypointer } },
	{ MODKEY,                 XK_p,      spawn,          {.v = photos } },
	{ MODKEY,             		XK_w,      spawn,          {.v = wifi } },
  /* website commands */
	{ MODKEY,	                XK_b,      spawn,          {.v = firefox } },
	{ MODKEY|ShiftMask,	      XK_b,      spawn,          {.v = batterylevel } },
	{ MODKEY,	                XK_g,      spawn,          {.v = google } },
	{ MODKEY|ShiftMask,	      XK_f,      spawn,          {.v = facebook } },
	{ MODKEY|ShiftMask,	      XK_m,      spawn,          {.v = messenger } },
	{ MODKEY|ShiftMask,	      XK_n,      spawn,          {.v = netflix } },
	{ MODKEY|ShiftMask,	      XK_y,      spawn,          {.v = youtube } },
	{ MODKEY|ShiftMask,	      XK_i,      spawn,          {.v = instagram } },
	{ MODKEY|ShiftMask,	      XK_g,      spawn,          {.v = gmail } },
	{ MODKEY|ShiftMask,	      XK_a,      spawn,          {.v = aliexpress } },
	{ MODKEY|ShiftMask,	      XK_o,      spawn,          {.v = ouedkniss } },
	{ MODKEY|ShiftMask,	      XK_p,      spawn,          {.v = GPhotos } },
	{ MODKEY|ShiftMask,	      XK_e,      spawn,          {.v = eccp } },
	{ MODKEY|ShiftMask,	      XK_u,      spawn,          {.v = univ } },
	{ MODKEY|ShiftMask,	      XK_t,      spawn,          {.v = translate } },
	{ MODKEY|ShiftMask,	      XK_d,      spawn,          {.v = magnetDL } },
	{ MODKEY|ShiftMask,	      XK_c,      spawn,          {.v = contacts } },
	{ MODKEY|ShiftMask,	      XK_l,      spawn,          {.v = localhost } },
	//{ MODKEY|ShiftMask,	      XK_g,      spawn,          {.v = github } },

	//{ MODKEY,         		    XK_f,        togglebar,      {0} },
	{ MODKEY,                 XK_Tab,      focusstack,     {.i = +1 } },
	{ MODKEY|ShiftMask,       XK_Tab,      focusstack,     {.i = -1 } },
	//{ MODKEY,                 XK_i,      incnmaster,     {.i = +1 } },
	//{ MODKEY,                 XK_u,      incnmaster,     {.i = -1 } },
	//{ MODKEY,                 XK_h,      setmfact,       {.f = -0.05} },
	//{ MODKEY,                 XK_l,      setmfact,       {.f = +0.05} },
	//{ MODKEY,                 XK_Tab,	   zoom,           {0} },
	{ MODKEY,                 XK_z,      view,           {0} },
	{ MODKEY,	                XK_q,      killclient,     {0} },
	//{ MODKEY|ShiftMask,       XK_t,      setlayout,      {.v = &layouts[0]} },
	//{ MODKEY|ShiftMask,       XK_f,      setlayout,      {.v = &layouts[1]} },
	//{ MODKEY|ShiftMask,       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,       XK_space,  togglefloating, {0} },
	{ MODKEY,                 XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,       XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                 XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                 XK_period, focusmon,       {.i = +1 } },
	{ MODKEY,                 XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                 XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,       XK_equal,  setgaps,        {.i = 0  } },
	{ MODKEY|ShiftMask,       XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,       XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                  XK_1,                      0)
	TAGKEYS(                  XK_2,                      1)
	TAGKEYS(                  XK_3,                      2)
	TAGKEYS(                  XK_4,                      3)
	//TAGKEYS(                  XK_5,                      4)
	//TAGKEYS(                  XK_6,                      5)
	//TAGKEYS(                  XK_7,                      6)
	//TAGKEYS(                  XK_8,                      7)
	//TAGKEYS(                  XK_9,                      8)
	{ MODKEY|ShiftMask,       XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

