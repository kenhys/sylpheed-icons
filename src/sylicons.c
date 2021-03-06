/*
 * SylIcons -- 
 * Copyright (C) 2012-2015 HAYASHI Kentaro
 *
 */

#include "config.h"

#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include "sylmain.h"
#include "plugin.h"
#include "folder.h"
#include "procmsg.h"
#include "compose.h"
#include "alertpanel.h"
#include "foldersel.h"
#include "headerview.h"
#include "messageview.h"
#include "procheader.h"
#include "sylplugin_factory.h"
#include "sylicons.h"
#include "copying.h"

#define PLUGIN_NAME N_("SylIcons Plug-in")
#define PLUGIN_DESC N_("Show Mail user agent icon by X-Mail header")

static SylPluginInfo info = {
  N_(PLUGIN_NAME),
  VERSION,
  "HAYASHI Kentaro",
  N_(PLUGIN_DESC)
};

SylIconsOption g_opt;

static void init_done_cb(GObject *obj, gpointer data);
static void app_exit_cb(GObject *obj, gpointer data);

static void exec_sylicons_menu_cb(void);

gulong app_exit_handler_id = 0;

void plugin_load(void)
{
  gpointer mainwin;
  GList* folder_list = folder_get_list();
  Folder *cur_folder;
  GList *cur;
  gint i;

  syl_init_gettext(SYLICONS, "lib/locale");
  g_print("sylicons plug-in loaded!\n");

  syl_plugin_add_menuitem("/Tools", NULL, NULL, NULL);
  syl_plugin_add_menuitem("/Tools", _("SylIcons [sylicons]"), exec_sylicons_menu_cb, NULL);

  mainwin = syl_plugin_main_window_get();
  syl_plugin_main_window_popup(mainwin);

  g_signal_connect_after(syl_app_get(), "init-done", G_CALLBACK(init_done_cb),
                         NULL);
  g_signal_connect(syl_app_get(), "app-exit", G_CALLBACK(app_exit_cb), NULL);

  syl_plugin_signal_connect("messageview-show",
                            G_CALLBACK(messageview_show_cb), NULL);
  g_print("sylicons plug-in loading done\n");


  for (i = 0, cur = folder_list; cur != NULL; cur = cur->next, i++) {
    cur_folder = FOLDER(cur->data);
    debug_print("[PLUGIN] folder[%d] name %s\n",i, cur_folder->name);
    if (FOLDER_TYPE(cur_folder) == F_MH) {
      if (cur_folder->data!=NULL){
        SYLPF_DEBUG_VAL("folder index", i);
        SYLPF_DEBUG_STR("folder data", (gchar *)cur_folder->data);
      }
      if (LOCAL_FOLDER(cur_folder)->rootpath!=NULL){
        SYLPF_DEBUG_VAL("folder index", i);
        SYLPF_DEBUG_STR("folder rootpath", LOCAL_FOLDER(cur_folder)->rootpath);
        g_opt.folder_path = g_strdup(LOCAL_FOLDER(cur_folder)->rootpath);
        break;
      }
    }
  }
}

void plugin_unload(void)
{
  g_print("sylicons plug-in unloaded!\n");
}

SylPluginInfo *plugin_info(void)
{
  return &info;
}

gint plugin_interface_version(void)
{
    return 0x0108;
    /* return SYL_PLUGIN_INTERFACE_VERSION; */
}

static void init_done_cb(GObject *obj, gpointer data)
{
  g_print("sylicons: %p: app init done\n", obj);
}

static void app_exit_cb(GObject *obj, gpointer data)
{
  g_print("sylicons: %p: app will exit\n", obj);
}

static void prefs_ok_cb(GtkWidget *widget, gpointer data)
{
  GList* folder_list = folder_get_list();

  Folder *cur_folder;
  GList *cur;
  gint i;
  Folder *mh_folder;

  gint mhn = 0;
  for (i = 0, cur = folder_list; cur != NULL; cur = cur->next, i++) {
    cur_folder = FOLDER(cur->data);
    debug_print("[PLUGIN] folder[%d] name %s\n",i, cur_folder->name);
    if (FOLDER_TYPE(cur_folder) == F_MH) {
      if (cur_folder->data!=NULL){
        SYLPF_DEBUG_VAL("folder index", i);
        SYLPF_DEBUG_STR("folder data", (gchar *)cur_folder->data);
      }
      if (LOCAL_FOLDER(cur_folder)->rootpath!=NULL){
        SYLPF_DEBUG_VAL("folder index", i);
        SYLPF_DEBUG_STR("folder rootpath", LOCAL_FOLDER(cur_folder)->rootpath);
        if (mhn == 0) {
          mh_folder = cur_folder;
        }
        mhn++;
      }
    }
  }
  if (mhn != 1){
    syl_plugin_alertpanel_message(_("SylIcons"), _("does not support multiple MH folder"), ALERT_ERROR);
  } else {
    if (mh_folder != NULL){
      LOCAL_FOLDER(mh_folder)->rootpath = g_strdup(gtk_entry_get_text(GTK_ENTRY(g_opt.folder_entry)));
    }
    folder_write_list();
  }
  gtk_widget_destroy(GTK_WIDGET(data));
}

static void prefs_cancel_cb(GtkWidget *widget, gpointer data)
{
  gtk_widget_destroy(GTK_WIDGET(data));
}

static void exec_sylicons_menu_cb(void)
{
  /* show modal dialog */
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *confirm_area;
  GtkWidget *ok_btn;
  GtkWidget *cancel_btn;
  GtkWidget *notebook;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(window), 8);
  gtk_window_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_modal(GTK_WINDOW(window), TRUE);
  gtk_window_set_policy(GTK_WINDOW(window), FALSE, TRUE, FALSE);
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
  gtk_widget_realize(window);

  vbox = gtk_vbox_new(FALSE, 6);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(window), vbox);


  /* notebook */ 
  notebook = gtk_notebook_new();
  /* main tab */
  create_config_main_page(notebook, g_opt.rcfile);
  /* about, copyright tab */
  create_config_about_page(notebook, g_opt.rcfile);

  gtk_widget_show(notebook);
  gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

  confirm_area = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(confirm_area), GTK_BUTTONBOX_END);
  gtk_box_set_spacing(GTK_BOX(confirm_area), 6);


  ok_btn = gtk_button_new_from_stock(GTK_STOCK_OK);
  GTK_WIDGET_SET_FLAGS(ok_btn, GTK_CAN_DEFAULT);
  gtk_box_pack_start(GTK_BOX(confirm_area), ok_btn, FALSE, FALSE, 0);
  gtk_widget_show(ok_btn);

  cancel_btn = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancel_btn, GTK_CAN_DEFAULT);
  gtk_box_pack_start(GTK_BOX(confirm_area), cancel_btn, FALSE, FALSE, 0);
  gtk_widget_show(cancel_btn);

  gtk_widget_show(confirm_area);

  gtk_box_pack_end(GTK_BOX(vbox), confirm_area, FALSE, FALSE, 0);
  gtk_widget_grab_default(ok_btn);
  gtk_widget_show(vbox);

  gtk_window_set_title(GTK_WINDOW(window), _("SylIcons"));

  g_signal_connect(G_OBJECT(ok_btn), "clicked",
                   G_CALLBACK(prefs_ok_cb), window);
  g_signal_connect(G_OBJECT(cancel_btn), "clicked",
                   G_CALLBACK(prefs_cancel_cb), window);
  gtk_widget_show_all(window);

}

typedef struct _Mailer {
    gchar *head;
    gchar *image;
} Mailer;

static Mailer x_mailer[] = {
    {"Microsoft Office Outlook", "ms_outlook12.png"},
    {"Microsoft Outlook Express", "ms_outlook_express.png"},
    {"Microsoft Outlook", "ms_outlook.png"},
    {"Microsoft CDO", "ms_cdo.png"},
    {"Microsoft-Entourage", "ms_entourage.png"},
    {"Mew", "mew.png"},
    {"Thunderbird", "thunderbird.png"},
    {"Wanderlust", "wanderlust.png"},
    {"Becky!", "becky.png"},
    {"Sylpheed", "sylpheed.png"},
    {"Claws Mail", "claws-mail.png"},
    {"Mutt", "mutt.png"},
    {"Alpine", "alpine.png"},
    {"KMail/", "kmail.png"},
    {"Shuriken", "shuriken_pro.png"},
    {"Eudora", "eudora.png"},
    {"Apple Mail", "apple_mail.png"},
    {"iPhone Mail", "apple_iphone.png"},
    {"iPod Mail", "apple_ipodtouch.png"},
    {"iPad Mail", "apple_ipad.png"},
    {"Evolution", "evolution2.png"},
    {"Zendesk Mailer", "zendesk.png"},
    {"PHPMailer", "phpmailer.png"},
    {"phpmailer", "phpmailer.png"},
    {"CakePHP Email Component", "cakephp.png"},
    {"phpBB", "phpbb.png"},
    {"phplist", "phplist.png"},
    {"AtMail PHP", "atmail.png"},
    {"PunBB Mailer", "punbb.png"},
    {"MyBB", "mybb.png"},
    {"PHP/", "php.png"},
    {"MailBee", "mailbee.png"},
    {"AOL", "aol.png"},
    {"Airmail", "airmail.png"},
    {"EDMAIL", "edmail.png"},
    {"MailChimp Mailer", "mailchimp.png"},
    {"Zimbra", "zimbra.png"},
    {"Roving Constant Contact", "roving_constant_contact.png"},
    {"YahooMailWebService", "yahoo.png"},
    {"Create Send", "createsend.png"},
    {"createsend", "createsend.png"},
    {"StreamSend", "streamsend.png"},
    {"Talisma", "talisma.png"},
    {"AWeber", "aweber.png"},
    {"sparrow", "sparrow.png"},
    {"Kana Connect", "kana.png"},
    {"SendBlaster", "sendblaster.png"},
    {"TWIG", "twig.png"},
    {"Drupal", "drupal.png"},
    {"vBulletin Mail", "vbulletin.png"},
    {"Mailout Interactive Mail Manager", "mailout.png"},
    {"MessagingEngine.com", "messagingengine.png"},
    {"AT&T Message Center", "att.png"},
    {"CodeIgniter", "codeigniter.png"},
    {"Perl script", "perl.png"},
    {"osCommerce", "oscommerce.png"},
    {"Yamail", "yandex_ru.png"},
    {"Tellmatic", "tellmatic.png"},
    {"WritelyMailer", "writely.png"},
    {"getresponse.com", "getresponse.png"},
    {"ColdFusion", "coldfusion.png"},
    {"WAMailer", "wamailer.png"},
    {"WorkCentre", "xerox_workcentre.png"},
    {"Verizon Webmail", "verizon.png"},
    {"GoldMine", "goldmine.png"},
    {"Blat", "blat.png"},
    {"vacation", "vacation.png"},
    {"Sympa", "sympa.png"},
    {"One.com webmail", "one_com.png"},
    {"CouchSurfing Mailer", "couchsurfing.png"},
    {"Chilkat Software Inc", "chilkat.png"},
    {"Contactology", "contactology.png"},
    {"Coremail Webmail", "coremail.png"},
    {"CLEAR Net", "clearnet.png"},
    {"CashU System", "cashu.png"},
    {"Roundcube Webmail", "roundcube.png"},
    {"Heirloom mailx", "heirloom_mailx.png"},
    {"Sendmail", "sendmail.png"},
    {"Laszlo Mail", "laszlomail.png"},
    {"G2", "google_groups.png"},
    {"SquirrelMail/", "squirrelmail.png"},
    {"ExpressionEngine", "expressionengine.png"},
    {"Gnus", "gnus.png"},
    {"eZ Components", "ezcomponents.png"},
    {"ICS", "ics.png"},
    {"Postbox", "postbox.png"},
    {"SOGoMail", "sogomail.png"},
    {"Neotonic Trakken", "neotonic.png"},
    {"NetToolWorks.NET Mail", "nettoolworks.png"},
    {"git-send-email", "git.png"},
    {"Mail.Ru", "mail_ru.png"},
    {"Neotonic", "neotonic.png"},
    {"Xerox Mailer", "xerox_workcentre.png"},
    {"Icedove", "icedove.png"},
    {"Mozilla", "mozilla.png"},
};

static void messageview_show_cb(GObject *obj, gpointer msgview,
				MsgInfo *msginfo, gboolean all_headers)
{
  MessageView *messageview;
  HeaderView *headerview;
  GtkWidget *hbox;
  GList* wl;
  gint i;
  gpointer gdata;
  gchar *msg_path;
  GSList* hl;
  gchar *path = NULL;
  gboolean gface;
  Header *header;
  guint mindex;
  guint mmax;
  GError *gerr = NULL;
  GdkPixbuf *pbuf;
  GtkWidget *icon;

#if DEBUG
  g_print("[DEBUG] sylicons: %p: messageview_show (%p), all_headers: %d: %s\n",
	  obj, msgview, all_headers,
	  msginfo && msginfo->subject ? msginfo->subject : "");
#endif
  
  if (!msgview) {
    g_print("[DEBUG] msgview is NULL\n");
    return;
  }

  messageview = (MessageView*)msgview;
  if (!messageview) {
    g_print("[DEBUG] messageview is NULL\n");
    return;
  }

  headerview = messageview->headerview;
  if (!headerview) {
    g_print("[DEBUG] headerview is NULL\n");
    return;
  }

  hbox = headerview->hbox;
  if (!hbox) {
    g_print("[DEBUG] hbox is NULL\n");
    return;
  }

  wl = gtk_container_get_children(GTK_CONTAINER(hbox));

  i = g_list_length(wl)-1;

  /* search recently added GtkImage */
  while (i >= 0) {
    gdata = g_list_nth_data(wl, i);
    if (GTK_IS_IMAGE(gdata) && gdata != headerview->image) {
      /* remove from hbox */
      g_print("[DEBUG] GTK_IS_IMAGE %p\n", gdata);
#if DEBUG
      g_print("[DEBUG] remove icon: %p\n", gicon);
#endif
      gtk_container_remove(GTK_CONTAINER(hbox), GTK_WIDGET(gdata));
    }
    i--;
  }

  /* check X-Mailer or User-Agent */
  
  msg_path = procmsg_get_message_file_path(msginfo);

#if DEBUG
  g_print("[DEBUG] msg_path:%s\n", msg_path);
#endif
  hl = procheader_get_header_list_from_file(msg_path);

  gface = FALSE;
  for (i=0; i<g_slist_length(hl); i++){
    header = g_slist_nth_data(hl, i);
    if (header && header->name && header->body) {
      if (strcmp(header->name, "X-Face") == 0) {
	/* skip to display MUA icon */
	gface = TRUE;
	break;
      } else if (strcmp(header->name, "X-Mailer") == 0 ||
		 strcmp(header->name, "User-Agent") == 0) {
#if DEBUG
	g_print("[DEBUG] name:%s body:%s\n", header->name, header->body);
#endif
        mindex = 0;
        mmax = sizeof(x_mailer)/sizeof(Mailer);
	for (mindex = 0; mindex < mmax; mindex++){
	  if (header->body && x_mailer[mindex].head &&
	      g_strrstr(header->body, x_mailer[mindex].head) != NULL) {
	    path = g_strconcat(get_rc_dir(),
			       G_DIR_SEPARATOR_S,
			       "plugins",
			       G_DIR_SEPARATOR_S,
			       SYLICONS,
			       G_DIR_SEPARATOR_S,
			       x_mailer[mindex].image, NULL);
	    break;
	  }
	}
      }
    }
  }

#if 0
  GtkImage *icon = gtk_image_new_from_file(path);
#else
  if (path && g_file_test(path, G_FILE_TEST_IS_REGULAR | G_FILE_TEST_EXISTS)) {
#if DEBUG
    g_print("[DEBUG] MUA icon path: %s\n", path);
#endif

    pbuf = gdk_pixbuf_new_from_file(path, &gerr);
    if (gerr) {
      g_error("%s", gerr->message);
      return;
    }
    icon = gtk_image_new_from_pixbuf(pbuf);
    gtk_box_pack_end(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
    gtk_widget_show(icon);
#if DEBUG
    g_print("[DEBUG] MUA new icon path: %p\n", icon);
#endif

  } else {
#if DEBUG
    g_print("[DEBUG] MUA icon path 404: %s\n", path ? path : "");
#endif
  }
#endif
}

static GtkWidget *create_config_main_page(GtkWidget *notebook, GKeyFile *pkey)
{
  GtkWidget *vbox;
  GtkWidget *label;

  debug_print("create_config_main_page\n");
  if (notebook == NULL){
    return NULL;
  }
  /* startup */
  if (pkey!=NULL){
  }
  vbox = gtk_vbox_new(FALSE, 6);

  label = gtk_label_new(_("General"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);
  gtk_widget_show_all(notebook);

  return vbox;
}

/* about, copyright tab */
static GtkWidget *create_config_about_page(GtkWidget *notebook, GKeyFile *pkey)
{
  GtkWidget *hbox, *vbox;
  GtkWidget *misc;
  GtkWidget *scrolled;
  GtkWidget *label;
  GtkTextBuffer *tbuffer;
  GtkWidget *tview;
  
  debug_print("create_config_about_page\n");
  if (notebook == NULL){
    return NULL;
  }
  hbox = gtk_hbox_new(TRUE, 6);
  vbox = gtk_vbox_new(FALSE, 6);
  gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 6);

  misc = gtk_label_new("SylIcons");
  gtk_box_pack_start(GTK_BOX(vbox), misc, FALSE, TRUE, 6);

  misc = gtk_label_new(PLUGIN_DESC);
  gtk_box_pack_start(GTK_BOX(vbox), misc, FALSE, TRUE, 6);

  /* copyright */
  scrolled = gtk_scrolled_window_new(NULL, NULL);

  tbuffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(tbuffer, _(copyright), strlen(copyright));
  tview = gtk_text_view_new_with_buffer(tbuffer);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tview), FALSE);
  gtk_container_add(GTK_CONTAINER(scrolled), tview);

  gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 6);

  label = gtk_label_new(_("About"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), hbox, label);
  gtk_widget_show_all(notebook);
  return NULL;
}
