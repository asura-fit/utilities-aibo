//
// Copyright 2002,2003 Sony Corporation
//
// Permission to use, copy, modify, and redistribute this software is
// hereby granted.
//
// This software is provided "as is" without warranty of any kind,
// either expressed or implied, including but not limited to the
// implied warranties of fitness for a particular purpose.
//
//
// Modified at University Bremen
// for RoboCup 2004 - Martin Fritsche, March 2004

#define GTK_ENABLE_BROKEN

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <fstream>
#include <strstream>
#include <iomanip.h>
#include "RoboCupGameController.h"
#include "TeamInfo.h"
#include "Penalty.h"

#include <sys/stat.h>

#if OSTYPE != cygwin
#include <unistd.h>
#endif

#define RECOVER_FILE "game.dat"
#define LOG_FILE     "game.log"
#define SCORE_FONT   "-adobe-helvetica-bold-r-normal--34-*-*-*-*-*-iso8859-*"
#define NUMBER_FONT  "-adobe-helvetica-bold-r-normal--18-*-*-*-*-*-iso8859-*"


// external symbol
extern RoboCupGameController*  pSelf;


// internal symbols
static GtkWidget* window = 0;
static GtkWidget* dialog;
static GtkWidget* all_button;
static GtkWidget* clear_button;
static GtkWidget* ready_button;
static GtkWidget* set_button;
static GtkWidget* final_button;
static GtkWidget* button_box;
static GtkWidget* penalty_button_box;
static GtkWidget* kickoff_button;

static GtkWidget* score_button[NUM_TEAM];
static int old_score[2] = { -1, -1 };

static GtkWidget* frame[NUM_TEAM];
static GtkWidget* color_button[NUM_TEAM];
static GtkWidget* table[NUM_TEAM];
static GtkWidget* score_spinbutton[NUM_TEAM];
static GtkStyle* team_color_style[2];
static GtkStyle* ready_color_style;
static GtkStyle* blink_style[2];

static GtkWidget *robot_checkbutton[NUM_TEAM][NUM_ROBOT];
static GtkWidget *text[NUM_TEAM][NUM_ROBOT];

static RoboCupGameControlData data[NUM_TEAM][NUM_ROBOT];
static gint8 waitingTimeLeft[NUM_TEAM][NUM_ROBOT];
static int kickoff_index = 0;
static bool red_or_blue_flag = true;


typedef struct {
  GtkWidget *widget;
  GtkStyle *orig_style;
  int count;
} BlinkData;


static GtkStyle*
get_style(GdkColor bg[5], GdkColor base[5])
{
  GtkStyle *style = window ? gtk_style_copy(window->style) : gtk_style_new();

  for (int i = 0; i < 5; i ++) {
    if (bg) {
      style->bg[i] = bg[i];
    }
    if (base) {
      style->base[i] = base[i];
    }
  }

  return style;
}

static void
set_status(int team, int robot, const gchar* str, int r, int g, int b)
{
  GdkColormap* cmap = gdk_colormap_get_system();
  GdkColor color;

  color.red = r;
  color.green = g;
  color.blue = b;

  if (!gdk_color_alloc(cmap, &color))
    g_error("couldn't allocate color");

  gtk_editable_delete_text(GTK_EDITABLE(text[team][robot]), 0, -1);
  gtk_text_insert(
                  GTK_TEXT(text[team][robot]),
                  gtk_style_get_font(gtk_widget_get_style(text[team][robot])),
                  &color,
                  NULL,
                  str,
                  -1);
}


static void
display_state()
{
  const gchar* label[]
    = { "Initial", "Ready",     "Set", "Playing", "Penalized", "Finish" };
  int red[]   = {         0,   32767,         0,         0,       65535,        0 };
  int green[] = {     49152,   32767,     32767,         0,           0,    49152 };
  int blue[]  = {         0,       0,     32767,     65535,           0,        0 };

  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      const RoboCupGameControlData& d = data[i][j];
      set_status(i, j, label[d.state],
                 red[d.state], green[d.state], blue[d.state]);
    }
  }
}


static void
save_data()
{
  std::ofstream out(RECOVER_FILE);

  if (! out.is_open())
    return;

  // score
  for (int i = 0; i < NUM_TEAM; i ++) {
      out << gtk_spin_button_get_value_as_int(
                                            GTK_SPIN_BUTTON(score_spinbutton[i]))
        << std::endl;
  }

  // robot
  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      out << static_cast<int>(data[i][j].state) << ' ';
      out << std::endl;
    }
  }
  
  out << (int) red_or_blue_flag << ' ' << kickoff_index << endl;
}


static void
load_data()
{
  std::ifstream in(RECOVER_FILE);

  if (! in.is_open())
    return;

  // score
  for (int i = 0; i < NUM_TEAM; i ++) {
     int score;
    in >> score >> std::ws;
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(score_spinbutton[i]), score);
  }

  // robot
  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      int p;
      in >> p >> std::ws;
      if(p == ROBOCUP_STATE_FINISHED)
        p = ROBOCUP_STATE_INITIAL; // state restored in second half
      data[i][j].state = p;
    }
  }

  int rob;
  in >> rob >> std::ws >> kickoff_index >> std::ws;
  red_or_blue_flag = (bool) rob;

  display_state();
}


const char*
get_date_string()
{
  static const size_t BUFSIZE = 4+1 + 2+1 + 2+1 + 2+1 + 2+1 + 2 + 1;
  static char buf[BUFSIZE];

  time_t t1 = time(NULL);
  struct tm t2 = *localtime(&t1);

  snprintf(buf, BUFSIZE, "%04d/%02d/%02d %02d:%02d:%02d",
           t2.tm_year + 1900,
           t2.tm_mon + 1,
           t2.tm_mday,
           t2.tm_hour,
           t2.tm_min,
           t2.tm_sec);

  return buf;
}


const char*
get_target_string()
{
  bool first = true;
  static std::strstream str;

  str.seekp(0, std::ios::beg);

  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      if (gtk_toggle_button_get_active(
                                       GTK_TOGGLE_BUTTON(robot_checkbutton[i][j]))) {
        if (! first)
          str << ' ';
        else
          first = false;
        str << static_cast<char>('A' + i)
            << static_cast<char>('1' + j);
      }
    }
  }
  str << std::ends;

  return str.str();
}


static void
log(const char *msg)
{
  std::ofstream out(LOG_FILE, std::ios::app);

  if (! out.is_open())
    return;

  out << get_date_string() << ' ' << msg << std::endl;
  cout << get_date_string() << ' ' << msg << std::endl;
}


static void
send_data()
{
  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      if (gtk_toggle_button_get_active(
                                       GTK_TOGGLE_BUTTON(robot_checkbutton[i][j]))) {
        RoboCupGameControlData& d = data[i][j];
        // set information
        d.kickoff = (i == kickoff_index) ? ROBOCUP_KICKOFF_OWN
          : ROBOCUP_KICKOFF_OPPONENT;
        d.ownScore = gtk_spin_button_get_value_as_int(
                                                      GTK_SPIN_BUTTON(score_spinbutton[i]));
        d.opponentScore = gtk_spin_button_get_value_as_int(
                                                           GTK_SPIN_BUTTON(score_spinbutton[1 - i]));
        // restore style
        gtk_widget_set_style(robot_checkbutton[i][j],
                             gtk_widget_get_style(color_button[i]));
        // send data
        pSelf->SendControlData(d, i * NUM_ROBOT + j);
      }
    }
  }
  display_state();

  gtk_button_clicked(GTK_BUTTON(clear_button));
}


static int
on_blink_timeout(gpointer user_data)
{
  BlinkData* data = static_cast<BlinkData*>(user_data);

  if (data->count == 0) {
    gtk_widget_set_style(data->widget, data->orig_style);
    g_free(data);
    return 0;   // 0: never call me   1: call me again
  }

  data->count --;

  gtk_widget_set_style(data->widget, blink_style[data->count % 2]);

  return 1;     // 0: never call me   1: call me again
}


static void
start_blink(GtkWidget* widget)
{
  BlinkData* data = g_new(BlinkData, 1);

  data->widget = widget;
  data->orig_style = gtk_widget_get_style(widget);
  data->count = 6;

  gtk_timeout_add(33, on_blink_timeout, static_cast<gpointer>(data));
}


void
on_state_button_clicked(GtkButton *button, gpointer user_data)
{
  const RState state = reinterpret_cast<int>(user_data);

  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      if (gtk_toggle_button_get_active(
                                       GTK_TOGGLE_BUTTON(robot_checkbutton[i][j]))) {
        data[i][j].state = state;
        data[i][j].penalty = PENALTY_NONE;
      }
    }
  }
  gtk_widget_set_sensitive(score_button[0], state==3);
  gtk_widget_set_sensitive(score_button[1], state==3);

  static const char* statestr[] = {
    "ROBOCUP_STATE_INITIAL",
    "ROBOCUP_STATE_READY",
    "ROBOCUP_STATE_SET",
    "ROBOCUP_STATE_PLAYING",
    "ROBOCUP_STATE_PENALIZED",
    "ROBOCUP_STATE_FINISH"
  };
  std::strstream msg;
  msg << "state changed " << statestr[state]
      << " (" << get_target_string() << ')' << std::ends;
  log(msg.str());

  send_data();
  save_data();

  start_blink(GTK_WIDGET(button));
}


static int
on_penalty_timeout(gpointer user_data)
{
  const int team  = reinterpret_cast<int>(user_data) / NUM_ROBOT;
  const int robot = reinterpret_cast<int>(user_data) % NUM_ROBOT;

  if (data[team][robot].state == ROBOCUP_STATE_PENALIZED) {
    gtk_widget_set_style(robot_checkbutton[team][robot], ready_color_style);
  }

  return 0;     // 0: never call me   1: call me again
}


// new for count-down
static int
on_penalty_timeout_countdown(gpointer user_data)
{
  const int team  = reinterpret_cast<int>(user_data) / NUM_ROBOT;
  const int robot = reinterpret_cast<int>(user_data) % NUM_ROBOT;
  const gchar* pen_label[]
    = { "Left: 00",  "Left: 01", "Left: 02", "Left: 03", "Left: 04",  "Left: 05", "Left: 06", "Left: 07","Left: 08",  "Left: 09", "Left: 10", "Left: 11","Left: 12",  "Left: 13", "Left: 14", "Left: 15","Left: 16",  "Left: 17", "Left: 18", "Left: 19","Left: 20",  "Left: 21", "Left: 22", "Left: 23", "Left: 24",  "Left: 25", "Left: 26", "Left: 27", "Left: 28",  "Left: 29", "Left: 30"
       };
  
  int red[]   = {         0,   32767,         0,         0,       65535,        0 };
  int green[] = {     49152,   32767,     32767,         0,           0,    49152 };
  int blue[]  = {         0,       0,     32767,     65535,           0,        0 };


  if (data[team][robot].state == ROBOCUP_STATE_PENALIZED) {
    if(waitingTimeLeft[team][robot] >= 0) {
      set_status(team, robot, pen_label[waitingTimeLeft[team][robot]],
                 red[data[team][robot].state], green[data[team][robot].state], blue[data[team][robot].state]);
      waitingTimeLeft[team][robot]--;		 
      // callback
      if(waitingTimeLeft[team][robot] >= 0)
        gtk_timeout_add(
          1000,
          on_penalty_timeout_countdown,
          reinterpret_cast<gpointer>(team * NUM_ROBOT + robot));
    }
  }
  return 0;	// 0: never call me   1: call me again
}


void
on_penalty_button_clicked(GtkButton *button, gpointer user_data)
{
  const int penalty_index = reinterpret_cast<int>(user_data);

  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      if (gtk_toggle_button_get_active(
                                       GTK_TOGGLE_BUTTON(robot_checkbutton[i][j]))) {
        data[i][j].state = ROBOCUP_STATE_PENALIZED;
        data[i][j].penalty = penalty_index;
	waitingTimeLeft[i][j] = penaltyEntry[penalty_index].waitingTime - 1; 
        // call me back!
        gtk_timeout_add(
                        penaltyEntry[penalty_index].waitingTime * 1000,
                        on_penalty_timeout,
                        reinterpret_cast<gpointer>(i * NUM_ROBOT + j));

	//new for count-down
        if (waitingTimeLeft[i][j] >= 0) // dont call back for waiting-time < 0
          gtk_timeout_add(
            1000,
            on_penalty_timeout_countdown,
            reinterpret_cast<gpointer>(i * NUM_ROBOT + j));
      }
    }
  }

  start_blink(GTK_WIDGET(button));

  std::strstream msg;
  msg << "penalized " << penaltyEntry[penalty_index].name
      << " (" << get_target_string() << ')' << std::ends;
  log(msg.str());

  send_data();
  save_data();
}


void
on_resend_button_clicked(GtkButton *button, gpointer user_data)
{
  std::strstream msg;
  msg << "resend "
      << " (" << get_target_string() << ')' << std::ends;
  log(msg.str());

  send_data();
  save_data();
}

void
on_all_button_clicked(GtkButton *button, gpointer user_data)
{
  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      gtk_toggle_button_set_active(
                                   GTK_TOGGLE_BUTTON(robot_checkbutton[i][j]),
                                   reinterpret_cast<int>(user_data));
    }
  }
}


void
on_robot_checkbutton_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
  const int team = reinterpret_cast<int>(user_data);

  bool active = false;
  bool passive = false;
  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      if (gtk_toggle_button_get_active(
                                       GTK_TOGGLE_BUTTON(robot_checkbutton[i][j]))) {
        active = true;
      } else {
        passive = true;
      }
    }
  }
  if (active) {
    gtk_widget_set_sensitive(button_box, true);
    gtk_widget_set_sensitive(clear_button, true);
  } else {
    // all non-active
    gtk_widget_set_sensitive(button_box, false);
    gtk_widget_set_sensitive(clear_button, false);
  }
  if (passive) {
    gtk_widget_set_sensitive(ready_button, false);
    gtk_widget_set_sensitive(set_button, false);
    gtk_widget_set_sensitive(final_button, false);
    gtk_widget_set_sensitive(all_button, true);
    gtk_widget_set_sensitive(penalty_button_box, true);
  } else {
    // all active
    gtk_widget_set_sensitive(ready_button, true);
    gtk_widget_set_sensitive(set_button, true);
    gtk_widget_set_sensitive(final_button, true);
    gtk_widget_set_sensitive(all_button, false);
    gtk_widget_set_sensitive(penalty_button_box, false);
  }
}

static void
update_kickoff_button()
{
  gtk_widget_set_style(kickoff_button,
                       gtk_widget_get_style(color_button[kickoff_index]));
}

static void
on_kickoff_button_clicked(GtkToggleButton *button, gpointer)
{
  kickoff_index = 1 - kickoff_index;

  update_kickoff_button();
 
  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(robot_checkbutton[i][j]),true);
      if(data[i][j].state > ROBOCUP_STATE_READY)
        data[i][j].state = ROBOCUP_STATE_READY;
    }
  }

  send_data();
  save_data();
}

static void
update_color_label_button()
{
  if (red_or_blue_flag) {
    for (int i = 0; i < NUM_TEAM; i ++) {
      gtk_widget_set_style(color_button[i], team_color_style[1 - i]);
      for (int j = 0; j < NUM_ROBOT; j ++) {
        gtk_widget_set_style(robot_checkbutton[i][j],
                             team_color_style[1 - i]);
        data[i][j].teamColor = 1-i ? ROBOCUP_TEAMCOLOR_BLUE : ROBOCUP_TEAMCOLOR_RED;
        if(data[i][j].state > ROBOCUP_STATE_READY)
          data[i][j].state = ROBOCUP_STATE_READY;
      }
    }
  } else {
    for (int i = 0; i < NUM_TEAM; i ++) {
      gtk_widget_set_style(color_button[i], team_color_style[i]);
      for (int j = 0; j < NUM_ROBOT; j ++) {
        gtk_widget_set_style(robot_checkbutton[i][j],
                             team_color_style[i]);
        data[i][j].teamColor = i ? ROBOCUP_TEAMCOLOR_BLUE : ROBOCUP_TEAMCOLOR_RED;
        if(data[i][j].state > ROBOCUP_STATE_READY)
          data[i][j].state = ROBOCUP_STATE_READY;
      }
    }
  }
}

static gboolean
on_color_label_button_press(GtkWidget*, GdkEventButton *event, gpointer)
{

  red_or_blue_flag = ! red_or_blue_flag;
  update_color_label_button();
  update_kickoff_button();

  for (int i = 0; i < NUM_TEAM; i ++) {
    for (int j = 0; j < NUM_ROBOT; j ++) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(robot_checkbutton[i][j]),true);
    }
  }

  send_data();
  save_data();

  return false;
}


void
on_score_spinbutton_changed(GtkEditable *editable, gpointer user_data)
{
  int team = reinterpret_cast<int>(user_data);
  int score = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(editable));

  if (old_score[team] == -1 || old_score[team] < score) {
    kickoff_index = 1 - team;   // next kick-off is opponent team side
    gtk_widget_set_style(GTK_WIDGET(kickoff_button),
                         gtk_widget_get_style(color_button[kickoff_index]));
  }

  old_score[team] = score;
}


void
on_score_button_clicked(GtkButton *button, gpointer user_data)
{
  int team = reinterpret_cast<int>(user_data);
  int score = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(score_spinbutton[team]))+1;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(score_spinbutton[team]), score);
  old_score[team] = score;

  kickoff_index = 1 - team;   // next kick-off is opponent team side
  gtk_widget_set_style(GTK_WIDGET(kickoff_button),
                       gtk_widget_get_style(color_button[kickoff_index]));

  on_all_button_clicked(button, reinterpret_cast<void*>(1)); //1=all
  on_state_button_clicked(button, reinterpret_cast<gpointer>(ROBOCUP_STATE_READY)); 
}


static int
on_delete_event(GtkWidget*, GdkEvent*, gpointer)
{
  log("stop program");

  return false;
}


void
on_confirmation_button_clicked(GtkButton *button, gpointer user_data)
{
  bool yes = static_cast<bool>(reinterpret_cast<int>(user_data));

  gtk_widget_hide(dialog);

  if (yes)
    load_data();
  update_color_label_button();
  update_kickoff_button();
}


GtkWidget*
create_confirmation_window()
{
  GtkWidget *confirmation_window;
  GtkWidget *yes_button;
  GtkWidget *no_button;

  confirmation_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data(GTK_OBJECT(confirmation_window),
                      "confirmation_window", confirmation_window);
  gtk_window_set_title(GTK_WINDOW(confirmation_window), "Confirmation");
  gtk_window_set_position(GTK_WINDOW(confirmation_window), GTK_WIN_POS_CENTER);
  gtk_window_set_modal(GTK_WINDOW(confirmation_window), true);
  gtk_window_set_policy(GTK_WINDOW(confirmation_window), false, false, true);

  GtkWidget* vbox = gtk_vbox_new(false, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(confirmation_window), vbox);

  GtkWidget* message_label = gtk_label_new(
                                           "Restore previous session data?\n"
                                           "\n"
                                           "Hint: data is stored in \"./" RECOVER_FILE "\"");
  gtk_widget_show(message_label);
  gtk_box_pack_start(GTK_BOX(vbox), message_label, false, false, 0);
  gtk_misc_set_padding(GTK_MISC(message_label), 32, 32);

  GtkWidget* buttonbox = gtk_hbutton_box_new();
  gtk_widget_show(buttonbox);
  gtk_box_pack_start(GTK_BOX(vbox), buttonbox, true, true, 0);
  gtk_container_set_border_width(GTK_CONTAINER(buttonbox), 10);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(buttonbox), GTK_BUTTONBOX_SPREAD);
  gtk_button_box_set_spacing(GTK_BUTTON_BOX(buttonbox), 10);

  yes_button = gtk_button_new_with_label("Yes");
  gtk_widget_show(yes_button);
  gtk_container_add(GTK_CONTAINER(buttonbox), yes_button);
  GTK_WIDGET_SET_FLAGS(yes_button, GTK_CAN_DEFAULT);

  no_button = gtk_button_new_with_label("No");
  gtk_widget_show(no_button);
  gtk_container_add(GTK_CONTAINER(buttonbox), no_button);
  GTK_WIDGET_SET_FLAGS(no_button, GTK_CAN_DEFAULT);

  gtk_widget_grab_default(yes_button);

  gtk_signal_connect(GTK_OBJECT(yes_button), "clicked",
                     GTK_SIGNAL_FUNC(on_confirmation_button_clicked),
                     reinterpret_cast<void*>(static_cast<int>(true)));
  gtk_signal_connect(GTK_OBJECT(no_button), "clicked",
                     GTK_SIGNAL_FUNC(on_confirmation_button_clicked),
                     reinterpret_cast<void*>(static_cast<int>(false)));

  return confirmation_window;
}


void
GuiInit()
{
  // accelerator
  GtkAccelGroup *accel_group = gtk_accel_group_new();

  // window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "RoboCup Game Controller");
  gtk_window_set_policy(GTK_WINDOW(window), false, false, true);

  // team_color_style
  static GdkColor team_color[][5] = {
    {   // red
      { 0, 0xffff, 0x4000, 0x4000 },    // GTK_STATE_NORMAL
      { 0, 0x6000, 0x0000, 0x0000 },    // GTK_STATE_ACTIVE
      { 0, 0xffff, 0x8000, 0x8000 },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0x9c40 },    // GTK_STATE_SELECTED
      { 0, 0xffff, 0x4000, 0x4000 }     // GTK_STATE_INSENSITIVE
    }, {        // blue
      { 0, 0x4000, 0x4000, 0xffff },    // GTK_STATE_NORMAL
      { 0, 0x0000, 0x0000, 0x6000 },    // GTK_STATE_ACTIVE
      { 0, 0x8000, 0x8000, 0xffff },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0x9c40 },    // GTK_STATE_SELECTED
      { 0, 0x4000, 0x4000, 0xffff }     // GTK_STATE_INSENSITIVE
    }
  };
  for (int i = 0; i < NUM_TEAM; i++) {
    team_color_style[i] = get_style(team_color[i], 0);
  }

  // ready_color_style
  static GdkColor ready_color[] = {
    { 0, 0xd6d6, 0xf6d6, 0xd6d6 },      // GTK_STATE_NORMAL
    { 0, 0xc350, 0xe350, 0xc350 },      // GTK_STATE_ACTIVE
    { 0, 0xea60, 0xffff, 0xea60 },      // GTK_STATE_PRELIGHT
    { 0,      0,      0, 0x9c40 },      // GTK_STATE_SELECTED
    { 0, 0xd6d6, 0xd6d6, 0xd6d6 }       // GTK_STATE_INSENSITIVE
  };
  ready_color_style = get_style(ready_color, 0);

  // blink_style
  static GdkColor blink_color[][5] = {
    {
      { 0, 0xffff, 0xffff, 0xffff },    // GTK_STATE_NORMAL
      { 0, 0xffff, 0xffff, 0xffff },    // GTK_STATE_ACTIVE
      { 0, 0xffff, 0xffff, 0xffff },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0xffff },    // GTK_STATE_SELECTED
      { 0, 0xffff, 0xffff, 0xffff }     // GTK_STATE_INSENSITIVE
    }, {
      { 0,      0,      0,      0 },    // GTK_STATE_NORMAL
      { 0,      0,      0,      0 },    // GTK_STATE_ACTIVE
      { 0,      0,      0,      0 },    // GTK_STATE_PRELIGHT
      { 0,      0,      0,      0 },    // GTK_STATE_SELECTED
      { 0,      0,      0,      0 }     // GTK_STATE_INSENSITIVE
    }
  };
  blink_style[0] = get_style(blink_color[0], 0);
  blink_style[1] = get_style(blink_color[1], 0);

  static GdkColor penalty_button_bg_color[][5] = {
    {   // No penalty (not used)
      { 0, 0xf6d6, 0xd6d6, 0xd6d6 },    // GTK_STATE_NORMAL
      { 0, 0xe350, 0xc350, 0xc350 },    // GTK_STATE_ACTIVE
      { 0, 0xffff, 0xea60, 0xea60 },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0x9c40 },    // GTK_STATE_SELECTED
      { 0, 0xd6d6, 0xd6d6, 0xd6d6 }     // GTK_STATE_INSENSITIVE
    }, {        // Ball holding
      { 0, 0xe6d6, 0xe6d6, 0xd6d6 },    // GTK_STATE_NORMAL
      { 0, 0xd350, 0xd350, 0xc350 },    // GTK_STATE_ACTIVE
      { 0, 0xfa60, 0xfa60, 0xea60 },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0x9c40 },    // GTK_STATE_SELECTED
      { 0, 0xd6d6, 0xd6d6, 0xd6d6 }     // GTK_STATE_INSENSITIVE
    }, {        // Goalie pushing
      { 0, 0xe6d6, 0xe6d6, 0xd6d6 },    // GTK_STATE_NORMAL
      { 0, 0xd350, 0xd350, 0xc350 },    // GTK_STATE_ACTIVE
      { 0, 0xfa60, 0xfa60, 0xea60 },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0x9c40 },    // GTK_STATE_SELECTED
      { 0, 0xd6d6, 0xd6d6, 0xd6d6 }     // GTK_STATE_INSENSITIVE
    }, {        // Field Player Pushing
      { 0, 0xe6d6, 0xd6d6, 0xe6d6 },    // GTK_STATE_NORMAL
      { 0, 0xd350, 0xc350, 0xd350 },    // GTK_STATE_ACTIVE
      { 0, 0xfa60, 0xea60, 0xfa60 },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0x9c40 },    // GTK_STATE_SELECTED
      { 0, 0xd6d6, 0xd6d6, 0xd6d6 }     // GTK_STATE_INSENSITIVE
    }, {        // Illegal Defender
      { 0, 0xe6d6, 0xd6d6, 0xe6d6 },    // GTK_STATE_NORMAL
      { 0, 0xd350, 0xc350, 0xd350 },    // GTK_STATE_ACTIVE
      { 0, 0xfa60, 0xea60, 0xfa60 },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0x9c40 },    // GTK_STATE_SELECTED
      { 0, 0xd6d6, 0xd6d6, 0xd6d6 }     // GTK_STATE_INSENSITIVE
    }, {        // Illegal Defense
      { 0, 0xe6d6, 0xd6d6, 0xd6d6 },    // GTK_STATE_NORMAL
      { 0, 0xd350, 0xc350, 0xc350 },    // GTK_STATE_ACTIVE
      { 0, 0xfa60, 0xea60, 0xea60 },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0x9c40 },    // GTK_STATE_SELECTED
      { 0, 0xd6d6, 0xd6d6, 0xd6d6 }     // GTK_STATE_INSENSITIVE
    }, {        // Obstruction
      { 0, 0xe6d6, 0xd6d6, 0xd6d6 },    // GTK_STATE_NORMAL
      { 0, 0xd350, 0xc350, 0xc350 },    // GTK_STATE_ACTIVE
      { 0, 0xfa60, 0xea60, 0xea60 },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0x9c40 },    // GTK_STATE_SELECTED
      { 0, 0xd6d6, 0xd6d6, 0xd6d6 }     // GTK_STATE_INSENSITIVE
    }, {        // Request for Pick-up
      { 0, 0xe6d6, 0xd6d6, 0xd6d6 },    // GTK_STATE_NORMAL
      { 0, 0xd350, 0xc350, 0xc350 },    // GTK_STATE_ACTIVE
      { 0, 0xfa60, 0xea60, 0xea60 },    // GTK_STATE_PRELIGHT
      { 0,      0,      0, 0x9c40 },    // GTK_STATE_SELECTED
      { 0, 0xd6d6, 0xd6d6, 0xd6d6 }     // GTK_STATE_INSENSITIVE
    }
  };

  GtkWidget* base_hbox = gtk_hbox_new(false, 15);
  gtk_widget_show(base_hbox);
  gtk_container_add(GTK_CONTAINER(window), base_hbox);
  gtk_container_set_border_width(GTK_CONTAINER(base_hbox), 5);

  // left pane
  GtkWidget* left_vbox = gtk_vbox_new(false, 6);
  gtk_widget_show(left_vbox);
  gtk_box_pack_start(GTK_BOX(base_hbox), left_vbox, true, true, 0);

  // title
#if 0
  GtkWidget* title_frame = gtk_frame_new(NULL);
  gtk_widget_show(title_frame);
  gtk_box_pack_start(GTK_BOX(left_vbox), title_frame, true, false, 0);
  gtk_frame_set_shadow_type(GTK_FRAME(title_frame), GTK_SHADOW_IN);

  GtkWidget* title_label = gtk_label_new(
                                         "RoboCup 2004\nSony Legged Robot League\nGame Manager v1.2");
  gtk_widget_show(title_label);
  gtk_container_add(GTK_CONTAINER(title_frame), title_label);
  gtk_misc_set_padding(GTK_MISC(title_label), 5, 5);
#endif

  // Target
  GtkWidget* all_target_box = gtk_hbox_new(true, 5);
  gtk_widget_show(all_target_box);
  gtk_box_pack_start(GTK_BOX(left_vbox), all_target_box, false, false, 0);

  // All player
  all_button = gtk_button_new_with_label("All");
  gtk_widget_show(all_button);
  gtk_box_pack_start(GTK_BOX(all_target_box), all_button, true, true, 0);
  gtk_signal_connect(GTK_OBJECT(all_button), "clicked",
                     GTK_SIGNAL_FUNC(on_all_button_clicked), reinterpret_cast<void*>(1));
  gtk_widget_add_accelerator(all_button, "clicked", accel_group,
                             GDK_A, (GdkModifierType)0, GTK_ACCEL_VISIBLE);

  // Clear
  clear_button = gtk_button_new_with_label("Clear");
  gtk_widget_show(clear_button);
  gtk_box_pack_start(GTK_BOX(all_target_box), clear_button, true, true, 0);
  gtk_signal_connect(GTK_OBJECT(clear_button), "clicked",
                     GTK_SIGNAL_FUNC(on_all_button_clicked), reinterpret_cast<void*>(0));
  gtk_widget_set_sensitive(clear_button, false);
  gtk_widget_add_accelerator(clear_button, "clicked", accel_group,
                             GDK_C, (GdkModifierType)0, GTK_ACCEL_VISIBLE);

  // Kick-off state
  kickoff_button = gtk_button_new_with_label("Kick-off");
  gtk_widget_show(kickoff_button);
  gtk_box_pack_start(GTK_BOX(left_vbox), kickoff_button, false, false, 0);

  // -----
  GtkWidget* button_separator = gtk_hseparator_new();
  gtk_widget_show(button_separator);
  gtk_box_pack_start(GTK_BOX(left_vbox), button_separator,
                     false, false, 2);

  // button
  button_box = gtk_hbox_new(true, 10);
  gtk_widget_show(button_box);
  gtk_box_pack_start(GTK_BOX(left_vbox), button_box, true, true, 0);
  gtk_widget_set_sensitive(button_box, false);

  // left button
  GtkWidget* state_button_box = gtk_vbox_new(false, 5);
  gtk_widget_show(state_button_box);
  gtk_box_pack_start(GTK_BOX(button_box), state_button_box, true, true, 0);

  // Ready button
  ready_button = gtk_button_new_with_label("Ready");
  gtk_widget_show(ready_button);
  gtk_box_pack_start(GTK_BOX(state_button_box), ready_button,
                     false, false, 0);
  gtk_widget_add_accelerator(ready_button, "clicked", accel_group,
                             GDK_R, (GdkModifierType)0, GTK_ACCEL_VISIBLE);

  // Set button
  set_button = gtk_button_new_with_label("Set");
  gtk_widget_show(set_button);
  gtk_box_pack_start(GTK_BOX(state_button_box), set_button,
                     false, false, 0);
  gtk_widget_add_accelerator(set_button, "clicked", accel_group,
                             GDK_R, (GdkModifierType)0, GTK_ACCEL_VISIBLE);

  // Playing button
  GtkWidget* playing_button = gtk_button_new_with_label("Play");
  gtk_widget_show(playing_button);
  gtk_box_pack_start(GTK_BOX(state_button_box), playing_button,
                     true, true, 0);
  gtk_widget_set_style(playing_button, ready_color_style);
  gtk_widget_add_accelerator(playing_button, "clicked", accel_group,
                             GDK_P, (GdkModifierType)0, GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(playing_button, "clicked", accel_group,
                             GDK_Return, (GdkModifierType)0, GTK_ACCEL_VISIBLE);

  // Finish button
  final_button = gtk_button_new_with_label("Finish");
  gtk_widget_show(final_button);
  gtk_box_pack_start(GTK_BOX(state_button_box), final_button,
                     false, false, 0);
  gtk_widget_add_accelerator(final_button, "clicked", accel_group,
                             GDK_F, (GdkModifierType)0, GTK_ACCEL_VISIBLE);

  // right button // penalty buttons
  penalty_button_box = gtk_vbox_new(false, 5);
  gtk_widget_show(penalty_button_box);
  gtk_box_pack_start(GTK_BOX(button_box), penalty_button_box, true, true, 0);

  static guint accel_key[][4] = {
    { GDK_I, GDK_D, GDK_T, 0 },
    { GDK_O, 0 },
    { GDK_K, 0 },
    { GDK_B, GDK_H, 0 }
  };
  for (int i = 1; i < NUM_PENALTY; i ++) { /* i = 1 "no penalty" is not displayed */
    // button
    GtkWidget* penalty_button
      = gtk_button_new_with_label(penaltyEntry[i].name);
    gtk_widget_set_style(penalty_button,
                         get_style(penalty_button_bg_color[i], 0));
    gtk_widget_show(penalty_button);
    gtk_box_pack_start(GTK_BOX(penalty_button_box), penalty_button,
                       true, true, 0);
    // signal
    gtk_signal_connect(GTK_OBJECT(penalty_button), "clicked",
                       GTK_SIGNAL_FUNC(on_penalty_button_clicked),
                       reinterpret_cast<void*>(i));
    // accelerator
    for (int j = 0; accel_key[i][j] != 0; j ++) {
      gtk_widget_add_accelerator(penalty_button, "clicked", accel_group,
                                 accel_key[i][j], (GdkModifierType)0, GTK_ACCEL_VISIBLE);
    }
  }

  // -----
  button_separator = gtk_hseparator_new();
  gtk_widget_show(button_separator);
  gtk_box_pack_start(GTK_BOX(state_button_box), button_separator,
                     false, false, 2);

  // Resend
  GtkWidget* resend_button = gtk_button_new_with_label("Resend");
  gtk_widget_show(resend_button);
  gtk_box_pack_start(GTK_BOX(state_button_box), resend_button,
                     false, false, 0);
  gtk_widget_add_accelerator(resend_button, "clicked", accel_group,
                             GDK_S, (GdkModifierType)0, GTK_ACCEL_VISIBLE);

  // target selector
  GtkWidget* team_hbox = gtk_hbox_new(false, 10);
  gtk_widget_show(team_hbox);
  gtk_box_pack_start(GTK_BOX(base_hbox), team_hbox, true, true, 0);

  // font
  GtkStyle* score_style = gtk_style_copy(gtk_widget_get_style(window));
  gtk_style_set_font(score_style, gdk_font_load(SCORE_FONT));

  for (int i = 0; i < NUM_TEAM; i++) {
    // frame
    frame[i] = gtk_frame_new(0);
    gtk_widget_show(frame[i]);
    gtk_box_pack_start(GTK_BOX(team_hbox), frame[i], true, true, 0);

    GtkWidget* vbox = gtk_vbox_new(false, 10);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(frame[i]), vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);

    // color bar
    color_button[i] = gtk_button_new();
    gtk_widget_show(color_button[i]);
    gtk_box_pack_start(GTK_BOX(vbox), color_button[i], false, true, 0);
    gtk_widget_set_usize(color_button[i], -2, 22);
    GTK_WIDGET_UNSET_FLAGS(color_button[i], GTK_CAN_FOCUS);
    gtk_button_set_relief(GTK_BUTTON(color_button[i]), GTK_RELIEF_HALF);
    // signal
    gtk_signal_connect(GTK_OBJECT(color_button[i]), "clicked",
                       GTK_SIGNAL_FUNC(on_color_label_button_press), 0);
 
    // table
    table[i] = gtk_table_new(5, 2 + NUM_PENALTY, false);
    gtk_widget_show(table[i]);
    gtk_container_add(GTK_CONTAINER(vbox), table[i]);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_table_set_row_spacings(GTK_TABLE(table[i]), 5);
    gtk_table_set_col_spacings(GTK_TABLE(table[i]), 3);

        
    // penalty label
    for (int k = 1; k < NUM_PENALTY; k ++) {
      if (penaltyEntry[k].maxCount == 0)
        continue;
      GtkWidget* label = gtk_label_new(penaltyEntry[k].shortName);
      gtk_widget_show(label);
      gtk_table_attach(GTK_TABLE(table[i]), label,
                       2 + k, 3 + k, 0, 1,
                       (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
      gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    }

    GtkStyle* num_style = gtk_style_copy(gtk_widget_get_style(window));
        gtk_style_set_font(num_style, gdk_font_load(NUMBER_FONT));
    static GdkColor white = { 0, 0xffff, 0xffff, 0xffff };
    for (int j = 0; j < NUM_ROBOT; j++) {
      num_style->fg[j] = white;
    }

    for (int j = 0; j < NUM_ROBOT; j++) {
      // robot checkbutton
      robot_checkbutton[i][j] = gtk_check_button_new();
      //with gtk-2.2.1 we get strange behavior or crashes if this has more than 3 chars:
      //gchar* str = g_strdup_printf("   %d   ", i * NUM_ROBOT + j + 1);
      gchar* str = g_strdup_printf(" %d ", j + 1);
      GtkWidget *label = gtk_label_new(str);
      g_free(str);
      gtk_widget_set_style(label, num_style);
      gtk_widget_show(label);
      gtk_widget_set_style(robot_checkbutton[i][j], team_color_style[i]);
      gtk_container_add(GTK_CONTAINER(robot_checkbutton[i][j]), label);
      gtk_widget_show(robot_checkbutton[i][j]);
      gtk_table_attach(GTK_TABLE(table[i]), robot_checkbutton[i][j],
                       0, 1, 1 + j, 2 + j,
                       (GtkAttachOptions)(GTK_FILL),
                       (GtkAttachOptions)(0), 0, 0);
      gtk_toggle_button_set_mode(
                                 GTK_TOGGLE_BUTTON(robot_checkbutton[i][j]), false);
      gtk_widget_add_accelerator(robot_checkbutton[i][j], "clicked",
                                 accel_group, GDK_1 + i * NUM_ROBOT + j,
                                 (GdkModifierType)0, GTK_ACCEL_VISIBLE);

      // signal
      gtk_signal_connect(GTK_OBJECT(robot_checkbutton[i][j]), "toggled",
                         GTK_SIGNAL_FUNC(on_robot_checkbutton_toggled),
                         reinterpret_cast<void*>(i));

      // text
      text[i][j] = gtk_text_new(0, 0);
      gtk_widget_show(text[i][j]);
      gtk_table_attach(GTK_TABLE(table[i]), text[i][j],
                       1, 2, 1 + j, 2 + j,
                       (GtkAttachOptions)(GTK_FILL),
                       (GtkAttachOptions)(GTK_FILL), 0, 0);
      gtk_widget_set_usize(text[i][j], 75, 1);
      gtk_text_set_line_wrap(GTK_TEXT(text[i][j]), FALSE);
      GTK_WIDGET_UNSET_FLAGS(text[i][j], GTK_CAN_FOCUS);
    }

    // separator
    GtkWidget* hseparator = gtk_hseparator_new();
    gtk_widget_show(hseparator);
    gtk_box_pack_start(GTK_BOX(vbox), hseparator, true, true, 0);

    // info
    GtkWidget* info_hbox = gtk_hbox_new(false, 3);
    gtk_widget_show(info_hbox);
    gtk_box_pack_start(GTK_BOX(vbox), info_hbox, true, true, 0);

    // score
    score_button[i] = gtk_button_new_with_label("   ++   ");

    gtk_widget_show(score_button[i]);
    gtk_box_pack_start(GTK_BOX(info_hbox), score_button[i], false, false, 0);

    gtk_widget_add_accelerator(score_button[i], "clicked", accel_group, GDK_R, (GdkModifierType)0, GTK_ACCEL_VISIBLE);
    gtk_widget_set_sensitive(score_button[i], false);

    GtkObject* adj = gtk_adjustment_new(0, 0, 1000, 1, 1, 1);
    score_spinbutton[i] = gtk_spin_button_new(GTK_ADJUSTMENT(adj),
                                              1, 0);
    gtk_widget_show(score_spinbutton[i]);
    gtk_box_pack_start(GTK_BOX(info_hbox), score_spinbutton[i],
                       true, true, 0);
    GTK_WIDGET_UNSET_FLAGS(score_spinbutton[i], GTK_CAN_FOCUS);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(score_spinbutton[i]), true);
    gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(score_spinbutton[i]),
                                      GTK_UPDATE_IF_VALID);
    gtk_widget_set_style(score_spinbutton[i], score_style);
    gtk_signal_connect(GTK_OBJECT(score_spinbutton[i]), "changed",
                       GTK_SIGNAL_FUNC(on_score_spinbutton_changed),
                       reinterpret_cast<gpointer>(i));


    // score
    gtk_signal_connect(GTK_OBJECT(score_button[i]), "clicked",
                       GTK_SIGNAL_FUNC(on_score_button_clicked), reinterpret_cast<gpointer>(i));
  }
  gtk_widget_show(window);

  // state
  gtk_signal_connect(GTK_OBJECT(ready_button), "clicked",
                     GTK_SIGNAL_FUNC(on_state_button_clicked),
                     reinterpret_cast<gpointer>(1));
  gtk_signal_connect(GTK_OBJECT(set_button), "clicked",
                     GTK_SIGNAL_FUNC(on_state_button_clicked),
                     reinterpret_cast<gpointer>(2));
  gtk_signal_connect(GTK_OBJECT(playing_button), "clicked",
                     GTK_SIGNAL_FUNC(on_state_button_clicked),
                     reinterpret_cast<gpointer>(3));
  gtk_signal_connect(GTK_OBJECT(final_button), "clicked",
                     GTK_SIGNAL_FUNC(on_state_button_clicked),
                     reinterpret_cast<gpointer>(5));

  // resend
  gtk_signal_connect(GTK_OBJECT(resend_button), "clicked",
                     GTK_SIGNAL_FUNC(on_resend_button_clicked), button_box);

  // kickoff
  gtk_signal_connect(GTK_OBJECT(kickoff_button), "clicked",
                     GTK_SIGNAL_FUNC(on_kickoff_button_clicked), 0);
  gtk_widget_set_style(GTK_WIDGET(kickoff_button),
                       gtk_widget_get_style(color_button[kickoff_index]));

  // close window
  gtk_signal_connect(GTK_OBJECT(window), "delete_event",
                     GTK_SIGNAL_FUNC(on_delete_event), 0);

  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  // initialize data
  for (int i = 0; i < NUM_TEAM; i++) {
    for (int j = 0; j < NUM_ROBOT; j++) {
      data[i][j].state = ROBOCUP_STATE_INITIAL;
      data[i][j].kickoff = ROBOCUP_KICKOFF_INVALID;
      data[i][j].ownScore = 0;
      data[i][j].teamColor = i ? ROBOCUP_TEAMCOLOR_BLUE : ROBOCUP_TEAMCOLOR_RED;
      data[i][j].opponentScore = 0;
      data[i][j].penalty = PENALTY_NONE;
    }
  }
   // load if available
  std::ifstream in(RECOVER_FILE);
  if (in.is_open()) {
    dialog = create_confirmation_window();
    gtk_widget_show(dialog);
  }
  else
  {
    update_color_label_button();
    update_kickoff_button();
  }
  display_state();
  log("start program");
}
