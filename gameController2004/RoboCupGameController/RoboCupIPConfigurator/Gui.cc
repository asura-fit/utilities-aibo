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

#include <gtk/gtk.h>
#include <stdio.h>
#include "TeamInfo.h"

#define NUMBER_FONT  "-adobe-helvetica-bold-r-normal--18-*-*-*-*-*-iso8859-*"
#define HOSTGW_CFG   "MS/OPEN-R/MW/CONF/HOSTGW.CFG"

const int PORT = 1070;

const char* HEADER =
	"#\n"
	"# This HOSTGW.CFG is generated by RoboCupIPConfigurator\n"
	"#\n"
	"\n";


static GtkWidget* window;
static GtkWidget* ip_entry[NUM_TEAM][NUM_ROBOT];
static GtkWidget* ip_table[NUM_TEAM];
static GtkWidget* num_label[NUM_TEAM][NUM_ROBOT];
static GtkWidget* ok_button;


static void
cat(const char* path)
{
    size_t s;
    char buf[BUFSIZ];

    FILE* fp = fopen(path, "r");

    while ((s = fread(buf, 1, BUFSIZ, fp)) > 0)
	fwrite(buf, 1, s, stdout);

    fclose(fp);
}


void
on_ok_button_clicked(GtkButton *button, gpointer user_data)
{
    FILE* fp = fopen("ip.txt", "w");
    if (fp == NULL) {
	g_printerr("Cannot open ip.txt.\n");
	gtk_exit(-1);
    }
    for (int i = 0; i < NUM_TEAM; i ++) {
	for (int j = 0; j < NUM_ROBOT; j ++) {
	    fprintf(fp, "%s\n", gtk_entry_get_text(GTK_ENTRY(ip_entry[i][j])));
	}
    }
    fclose(fp);

    fp = fopen(HOSTGW_CFG, "w");
    if (fp == NULL) {
	g_printerr("Cannot open %s.\n", HOSTGW_CFG);
	gtk_exit(-1);
    }
    fprintf(fp, HEADER);
    for (int i = 0; i < NUM_TEAM; i ++) {
	for (int j = 0; j < NUM_ROBOT; j ++) {
	    if (gtk_entry_get_text(GTK_ENTRY(ip_entry[i][j]))[0] == 0) {
		fprintf(fp, "# ");
	    }
	    fprintf(fp,
		    "TCPGateway.Player%c%d.RoboCupGameControlData.O  %d  %s\n",
		    'A' + i,
		    j + 1,
		    PORT,
		    gtk_entry_get_text(GTK_ENTRY(ip_entry[i][j])));
	}
    }
    fclose(fp);

    cat(HOSTGW_CFG);

    gtk_widget_destroy(ok_button);
    gtk_widget_destroy(window);

    gtk_main_quit();
}


static GtkStyle*
get_style(GdkColor fg[5], GdkColor bg[5], GdkColor base[5])
{
    GtkStyle *style = window ? gtk_style_copy(window->style) : gtk_style_new();

    for (int i = 0; i < 5; i ++) {
	if (fg) {
	    style->fg[i] = fg[i];
	}
	if (bg) {
	    style->bg[i] = bg[i];
	}
	if (base) {
	    style->base[i] = base[i];
	}
    }

    return style;
}


static GtkStyle*
team_color_style(int n)
{
    // team_color_style
    static GtkStyle* num_style[2];
    static GdkColor team_color[][5] = {
	{	// red
	    { 0, 0xffff, 0x4000, 0x4000 },	// GTK_STATE_NORMAL
	    { 0, 0x6000, 0x0000, 0x0000 },	// GTK_STATE_ACTIVE
	    { 0, 0xffff, 0x8000, 0x8000 },	// GTK_STATE_PRELIGHT
	    { 0,      0,      0, 0x9c40 },	// GTK_STATE_SELECTED
	    { 0, 0xffff, 0x4000, 0x4000 }	// GTK_STATE_INSENSITIVE
	}, {	// blue
	    { 0, 0x4000, 0x4000, 0xffff },	// GTK_STATE_NORMAL
	    { 0, 0x0000, 0x0000, 0x6000 },	// GTK_STATE_ACTIVE
	    { 0, 0x8000, 0x8000, 0xffff },	// GTK_STATE_PRELIGHT
	    { 0,      0,      0, 0x9c40 },	// GTK_STATE_SELECTED
	    { 0, 0x4000, 0x4000, 0xffff }	// GTK_STATE_INSENSITIVE
	}
    };

    static bool first = true;
    if (first) {
	for (int i = 0; i < NUM_TEAM; i++) {
	    num_style[i] = get_style(team_color[i], 0, 0);
	    gtk_style_set_font(num_style[i], gdk_font_load(NUMBER_FONT));
	}
	first = false;
    }

    return num_style[n];
}


void
on_half_radio_button_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    gtk_widget_set_sensitive(ok_button, true);
    for (int i = 0; i < NUM_TEAM; i ++) {
	gtk_widget_set_sensitive(ip_table[i], true);
    }

    const int half = reinterpret_cast<int>(user_data);
    if (half == 1 && gtk_toggle_button_get_active(togglebutton)) {
	for (int i = 0; i < NUM_TEAM; i ++) {
	    for (int j = 0; j < NUM_ROBOT; j ++) {
		gtk_widget_set_style(num_label[i][j], team_color_style(1 - i));
	    }
	}
    } else if (half == 2 && gtk_toggle_button_get_active(togglebutton)) {
	for (int i = 0; i < NUM_TEAM; i ++) {
	    for (int j = 0; j < NUM_ROBOT; j ++) {
		gtk_widget_set_style(num_label[i][j], team_color_style(i));
	    }
	}
    }
}


void
InitGui()
{
    // window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "RoboCup IP Configurator");
    gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, TRUE);

    // vbox
    GtkWidget* vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // half selection
    GtkWidget* half_hbox = gtk_hbox_new(FALSE, 3);
    gtk_widget_show(half_hbox);
    gtk_box_pack_start(GTK_BOX(vbox), half_hbox, TRUE, TRUE, 0);
    // def
    GtkWidget* def_radio_button = gtk_radio_button_new(0);
    gtk_signal_connect(GTK_OBJECT(def_radio_button), "toggled",
            GTK_SIGNAL_FUNC(on_half_radio_button_toggled), (gpointer) 0);
    // 1st
    GtkWidget* first_half_radio_button = gtk_radio_button_new_with_label(
            gtk_radio_button_group(GTK_RADIO_BUTTON(def_radio_button)),
            "First half");
    gtk_widget_show(first_half_radio_button);
    gtk_box_pack_start(GTK_BOX(half_hbox),
            first_half_radio_button, TRUE, TRUE, 0);
    gtk_signal_connect(GTK_OBJECT(first_half_radio_button), "toggled",
            GTK_SIGNAL_FUNC(on_half_radio_button_toggled), (gpointer) 1);
    // 2nd
    GtkWidget* second_half_radio_button = gtk_radio_button_new_with_label(
            gtk_radio_button_group(GTK_RADIO_BUTTON(def_radio_button)),
            "Second half");
    gtk_widget_show(second_half_radio_button);
    gtk_box_pack_start(GTK_BOX(half_hbox),
            second_half_radio_button, TRUE, TRUE, 0);
    gtk_signal_connect(GTK_OBJECT(second_half_radio_button), "toggled",
            GTK_SIGNAL_FUNC(on_half_radio_button_toggled), (gpointer) 2);

    //
    GtkWidget* hsep = gtk_hseparator_new();
    gtk_widget_show(hsep);
    gtk_box_pack_start(GTK_BOX(vbox), hsep, TRUE, TRUE, 0);

    // hbox
    GtkWidget* ip_hbox = gtk_hbox_new(FALSE, 3);
    gtk_widget_show(ip_hbox);
    gtk_box_pack_start(GTK_BOX(vbox), ip_hbox, TRUE, TRUE, 0);

    for (int i = 0; i < NUM_TEAM; i ++) {
	// table
	ip_table[i] = gtk_table_new(NUM_ROBOT, NUM_TEAM, FALSE);
	gtk_widget_set_sensitive(ip_table[i], false);
	gtk_widget_show(ip_table[i]);
	gtk_container_add(GTK_CONTAINER(ip_hbox), ip_table[i]);
	gtk_container_set_border_width(GTK_CONTAINER(ip_table[i]), 5);
	gtk_table_set_row_spacings(GTK_TABLE(ip_table[i]), 3);
	gtk_table_set_col_spacings(GTK_TABLE(ip_table[i]), 3);

	for (int j = 0; j < NUM_ROBOT; j ++) {
	    // label
	    gchar* str = g_strdup_printf("  %d  ", i * NUM_ROBOT + j + 1);
	    num_label[i][j] = gtk_label_new(str);
	    g_free(str);
	    gtk_widget_set_style(num_label[i][j], team_color_style(i));
	    gtk_widget_show(num_label[i][j]);
	    gtk_table_attach(GTK_TABLE(ip_table[i]), num_label[i][j],
			     0, 1, j,  j + 1,
			     (GtkAttachOptions) (GTK_FILL),
			     (GtkAttachOptions) (0), 0, 0);

	    // entry
	    ip_entry[i][j] = gtk_entry_new_with_max_length(15);
	    gtk_widget_show(ip_entry[i][j]);
	    gtk_table_attach(GTK_TABLE(ip_table[i]), ip_entry[i][j],
			     1, 2, j, j + 1,
			     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			     (GtkAttachOptions) (0), 0, 0);
	    gtk_widget_set_usize(ip_entry[i][j], 120, -2);
	    if (teamInfo.GetIPAddress(i, j)) {
		gtk_entry_set_text(GTK_ENTRY(ip_entry[i][j]),
				   teamInfo.GetIPAddress(i, j));
	    } else {
		str = g_strdup_printf("192.168.%d.%d", i + 7, j + 1);
		gtk_entry_set_text(GTK_ENTRY(ip_entry[i][j]), str);
		g_free(str);
	    }
	}
    }

    // button
    ok_button = gtk_button_new_with_label("Ok");
    gtk_widget_set_sensitive(ok_button, false);
    gtk_widget_show(ok_button);
    gtk_box_pack_start(GTK_BOX(vbox), ok_button, FALSE, FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(ok_button), 5);

    // signal
    gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
		       GTK_SIGNAL_FUNC(on_ok_button_clicked), NULL);

    // finish!
    gtk_widget_show(window);
}