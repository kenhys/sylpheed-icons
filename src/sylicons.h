/*
 * SylIcons -- 
 * Copyright (C) 2012-2013 HAYASHI Kentaro
 */

#ifndef __SYLICONS_H__
#define __SYLICONS_H__

#define SYLICONS "sylicons"
#define SYLICONSRC "syliconsrc"

#define GET_RC_BOOLEAN(keyarg) g_key_file_get_boolean(g_opt.rcfile, SYLICONS, keyarg, NULL)

typedef struct _SylIconsOption SylIconsOption;

struct _SylIconsOption {
  /* General section */

  /* full path to ghostbiffrc*/
  gchar *rcpath;
  /* rcfile */
  GKeyFile *rcfile;
  
  GtkWidget *folder_entry;
  gchar *folder_path;
};
    
static void messageview_show_cb(GObject *obj, gpointer msgview,
                                MsgInfo *msginfo, gboolean all_headers);

static GtkWidget *create_config_main_page(GtkWidget *notebook, GKeyFile *pkey);
static GtkWidget *create_config_about_page(GtkWidget *notebook, GKeyFile *pkey);

#endif /* __SYLICONS_H__ */
