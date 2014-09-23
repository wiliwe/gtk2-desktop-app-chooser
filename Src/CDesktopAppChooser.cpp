/*! \file CDesktopAppChooser.cpp
    \brief Provide the user an UI to select currently installed X desktop applications.
  
    \author William.L
    \date 2008-09-11
    \version 1.0

    \b Change_History: 
    \n (1) 2008-09-11 William.L initial version. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib/gi18n.h>  // For multi-language.

#include "CDesktopAppChooser.h"


/* The title of the frame widget. The tile string is enclosed with GNU gettext hint for multi-language. */
#define WINDOW_TITLE  _("Desktop Application Chooser")

/*! \def DEFAULT_ICON_PATH
    \brief The path to the default icon used when there has no icon for a chosen application.
*/
#define DEFAULT_ICON_PATH "./img/"

/*! \def DEFAULT_ICON
    \brief The full name of the default icon used when there has no icon for a chosen application.
*/
#define DEFAULT_ICON DEFAULT_ICON_PATH"none.png"

/* The X desktop main application menu. */
#define  APPLICATIONS_MENU       "applications.menu"
#define  DEFAULT_APP_ICON        "application-x-executable"
#define  DEFAULT_APP__MIME_ICON  "gnome-mime-application-x-executable"

/*! \def IMG_SIZE
    \brief The size of the icon shown in the tree-view. The unit is "pixel"
*/
#define IMG_SIZE 48

/*! \def IMG_SIZE_SHOW
    \brief The size of the icon chosen to be shown as the appearance of a menu item.
    
    It could be one of the values 16 / 22 / 24 / 32 pixels.
*/
#define IMG_SIZE_SHOW 32


/*! \enum APPS_MENU_ITEM_IDX 
    \brief The application items tree-view column.
*/
enum APPS_MENU_ITEM_IDX 
{
  COLUMN_ICON = 0,
  COLUMN_TEXT,
  COLUMN_NODEDATA,
  NUM_COLS
};

//------------------------ Callback Functions
/*!	\fn static gboolean on_file_apply(GtkButton *button, CDesktopAppChooser *thisObject)
    \brief The callback function to retrieve user wanting node-data from the selected node.

    \param[in] button. The button this callback connects to.
    \param[in] thisObject. The instance of class CDesktopAppChooser.
    \return TRUE or FALSE
*/
static gboolean on_file_apply(GtkButton *button, CDesktopAppChooser *thisObject)
{
  GtkWidget *treeview = NULL;

  if(!button || !thisObject)
    return false;

  /* To get the data of the selected tree node. */
  treeview = thisObject->m_GetWidget(APPCHOOSER_GtkTreeView);
  if(treeview)
  {
     GtkTreeIter iter;
     GtkTreeModel *model =NULL;
     gpointer value;

     if(gtk_tree_selection_get_selected( GTK_TREE_SELECTION(thisObject->m_GetWidget(APPCHOOSER_GtkTreeSelection)), &model, &iter)) 
     {
        gtk_tree_model_get(model, &iter, COLUMN_NODEDATA, &value, -1);

        if(value)
        {
           APP_ITEM_INFO *appInfo = (APP_ITEM_INFO*)value;

           /* Try to get the context storing selected application item contents. */
           if( thisObject->m_GetSelectedAppItem() )
           {
              APP_ITEM_INFO *storeSelected = thisObject->m_GetSelectedAppItem();

              /* Clear out old data. */
              if(storeSelected->name)
                g_free(storeSelected->name);

              if(storeSelected->icon)
                g_free(storeSelected->icon);

              if(storeSelected->exec)
                g_free(storeSelected->exec);

              if(storeSelected->comment)
                g_free(storeSelected->comment);

			  if(storeSelected->desktopfile)
                g_free(storeSelected->desktopfile);

              /* To set all members to be NULL. */
              storeSelected->name = NULL;
              storeSelected->icon = NULL;
              storeSelected->exec = NULL;
              storeSelected->comment = NULL;
              storeSelected->desktopfile = NULL;

              /* Store the selected node's data. */
              if(appInfo->name)
                storeSelected->name = (gchar*)g_strdup(appInfo->name);

              if(appInfo->icon)
              {
                 gchar *dirName = g_path_get_dirname(appInfo->icon);

                 /* To check if it has path name. */
                 if( !dirName || (*dirName == '.') )
                 {
                    GtkIconTheme *theme = gtk_icon_theme_get_default();
                    GtkIconInfo *info = gtk_icon_theme_lookup_icon( theme, appInfo->icon, IMG_SIZE_SHOW,  GTK_ICON_LOOKUP_USE_BUILTIN );

                    if(G_UNLIKELY(!info))
					{
                       /* To search the icon in alternative paths. */
                       storeSelected->icon = thisObject->m_GetIconFullName(appInfo->icon, IMG_SIZE_SHOW);

                       if(storeSelected->icon == NULL)
						  storeSelected->icon = (gchar*)g_strdup(DEFAULT_ICON);  /* If it can not find the icon file name specified in the ".desktop" file. */
                    }
                    else
                    {
                       if(dirName)
                         g_free(dirName);

                       /* To reset the icon name in full path form. */
                       g_free(storeSelected->icon);
                       storeSelected->icon = NULL;
                       storeSelected->icon = g_strdup( gtk_icon_info_get_filename(info) );

                       if(storeSelected->icon)
                          ;
                       else
                       {
                          /* To search the icon in alternative paths. */
                          storeSelected->icon = thisObject->m_GetIconFullName(appInfo->icon, IMG_SIZE_SHOW);
                       }
                    }
                 }
                 else
                    storeSelected->icon = (gchar*)g_strdup(appInfo->icon);
              }
              else
                 storeSelected->icon = (gchar*)g_strdup(DEFAULT_ICON);  /* If the icon field in the ".desktop" is empty, set the default icon full name. */

              if(appInfo->exec)
                storeSelected->exec = (gchar*)g_strdup(appInfo->exec);

              if(appInfo->comment)
                storeSelected->comment = (gchar*)g_strdup(appInfo->comment);

              if (appInfo->desktopfile)
                storeSelected->desktopfile = (gchar*)g_strdup(appInfo->desktopfile);

              /* To set the flag to be true for that there has one application item has been chosen. */
              thisObject->m_SetIsChosen(true);
           } // end of "if"									
        } // end of "if"
		else
          return true;
    }
  }

  /* To deinitialize all member variables and release allocated memory. */
  thisObject->m_DeinitValue();

  /* To destroy dialog window. */
  gtk_widget_destroy( thisObject->m_GetWidget(APPCHOOSER_GtkWindow_Main) );

  return true;
}

/*! \fn static gboolean on_close(GtkButton *button, CDesktopAppChooser *thisObject)
    \brief The callback function just to close the applications picker and do nothing.

    \param[in] button. The GTK button object this callback function connects to.
    \param[in] thisObject. The instance of class CDesktopAppChooser.
    \return TRUE or FALSE
*/
static gboolean on_close(GtkButton *button, CDesktopAppChooser *thisObject)
{
  if(!button || !thisObject)
    return false;

  /* To de-initialize all member variables and release allocated memory. */
  thisObject->m_DeinitValue();

  /* To destroy dialog window. */
  gtk_widget_destroy( thisObject->m_GetWidget(APPCHOOSER_GtkWindow_Main) );

  return true;
}

/*! \fn static gboolean cb_remove_node_data(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
    \brief The callback function for removing all node-data items from the tree tree-view

    \param[in] model. The tree model for GtkTreeView.
    \param[in] path. The logical position of a row in the model. 
    \param[in] iter. The iterator of the model.
    \param[in] data. The application specific data.
    \return TRUE or FALSE
*/
static gboolean cb_remove_node_data(GtkTreeModel *model, GtkTreePath *path,
                                    GtkTreeIter *iter, gpointer data)
{
  gpointer value;
  path = path;
  data = data;

  gtk_tree_model_get(model, iter, COLUMN_NODEDATA, &value, -1);

  if(value)
  {
     APP_ITEM_INFO *appInfo = (APP_ITEM_INFO*)value;

     /* Clear out old data. */
     if(appInfo->name)
       g_free(appInfo->name);

     if(appInfo->icon)
       g_free(appInfo->icon);

     if(appInfo->exec)
       g_free(appInfo->exec);

     if(appInfo->comment)
       g_free(appInfo->comment);

     if(appInfo->desktopfile)
       g_free(appInfo->desktopfile);

     /* To set all members to be NULL. */
     appInfo->name = NULL;
     appInfo->icon = NULL;
     appInfo->exec = NULL;
     appInfo->comment = NULL;
     appInfo->desktopfile = NULL;

     g_free(value);
  } // end of "if"

  return FALSE;  /* do not stop walking the store, call us with next row */
}

//--------------- Class Methos Implementation.
/*! \fn CDesktopAppChooser::CDesktopAppChooser()
    \brief CDesktopAppChooser constructor
*/
CDesktopAppChooser::CDesktopAppChooser()
{
  m_pwParent = NULL;
  m_MenuTree = NULL;
  m_RootDir = NULL;
  m_TreeViewTree = NULL;
  m_TreeSelection = NULL;
  m_TreeStore = NULL;
  m_bIsChosen = false;

  memset(&m_SelectedAppItemInfo, 0, sizeof(APP_ITEM_INFO));

  for(int i=0; i<N_APPCHOOSER_WIDGET_IDX ;i++)
     m_pWidgets[i] = NULL;
}

/*! \fn CDesktopAppChooser::~CDesktopAppChooser()
    \brief CDesktopAppChooser destructor
*/
CDesktopAppChooser::~CDesktopAppChooser()
{
  m_pwParent = NULL;
  m_bIsChosen = false;
}

/*! \fn void CDesktopAppChooser::m_GetWindowSize(int &nWidth, int &nHeight)
    \brief To get the dimension of the Desktop App Chooser window.

    \param[out] nWidth.
    \param[out] nHeight.
*/
void CDesktopAppChooser::m_GetWindowSize(int &nWidth, int &nHeight)
{
  nWidth = this->m_nMaxWidth;
  nHeight = this->m_nMaxHeight;
	
  return;
}

/*! \fn void CDesktopAppChooser::m_CreateInitValue(void)
    \brief Create initial value

    \param[in] NONE
    \return NONE
*/
void CDesktopAppChooser::m_CreateInitValue(void)
{
  /* To create the tree-store model. There has tree fields: 
         { Pixel-Buffer, Text, GPointer }.
  */
  m_TreeStore = gtk_tree_store_new(NUM_COLS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER);

  /* To fill tree store(model) by reading Desktop Menu(.menu) file. */
  m_LoadAndBuildAppsMenuTree();
}

/*! \fn gboolean CDesktopAppChooser::m_InitLayoutUI(GtkWidget *pwGtkParent, int nPosX, int nPosY)
    \brief Initialize Desktop App Chooser UI(widgets).

    \param[in] pwGtkParent. The top-level window object.
    \param[in] nPosX. The X-axis position of Desktop App Chooser window.
    \param[in] nPosY. The Y-axis position of Desktop App Chooser window.
    \return TRUE or FALSE
*/
gboolean CDesktopAppChooser::m_InitLayoutUI(GtkWidget *pwGtkParent, int nPosX, int nPosY)
{
  GtkWidget	*window = NULL, *scrollWin = NULL;
  GtkWidget	*buttonApply = NULL, *buttonClose = NULL;
  GtkWidget *pFixedContainer = NULL;
  GtkWidget	*treeView = NULL;
  gboolean bRet = TRUE;

  /* To store the top-level GTK dialog window. */
  if(pwGtkParent != NULL)
  {
     m_pwParent = pwGtkParent;
  }

//-------------- Create a main window for the file add/remove dialog.
  /* Create a new window object */
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
  /* Set the size of the new window. */
  gtk_widget_set_size_request(window, 330, 400);

  /* Set the title of the window. */
  gtk_window_set_title(GTK_WINDOW(window), WINDOW_TITLE);

  /* Set the window to be irresizable. */
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

  /* If you set the deletable property to FALSE using this function,
     GTK+ will do its best to convince the window manager not to show a close button. */
  gtk_window_set_deletable(GTK_WINDOW(window), FALSE);

  /* The event mask of the widget determines, what kind of event will a particular widget receive.
     This set all bit-flags of the Desktop App Chooser window to receive all events. */
  gtk_widget_add_events(window, GDK_ALL_EVENTS_MASK);

  /* Set the position of the Desktop App Chooser window on the desktop. */
  gtk_window_move(GTK_WINDOW(window), nPosX, nPosY);

  /* Connect the "destroy" event to a signal handler.
     This event occurs when we call gtk_widget_destroy() on the window,
     or if we return 'FALSE' in the "delete_event" callback. */
  gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

  /* Store the required widgets ... */
  m_pWidgets[APPCHOOSER_GtkWindow_Main] = window;

//-------------- Create the gtk fixed container
  /* The main fixed window in main window */
  pFixedContainer = gtk_fixed_new();

  /* Add the main fixed window into the main window */
  gtk_container_add(GTK_CONTAINER (window), pFixedContainer);	

//-------------- Create a scrolled window widget instance.
  /* Create a scroll window object. */
  scrollWin = gtk_scrolled_window_new(NULL, NULL);

  /* Set the scroll showing policy. */
  gtk_scrolled_window_set_policy((GtkScrolledWindow*)(scrollWin), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  /* Set the shadow type of the scrolling window. */
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrollWin), GTK_SHADOW_ETCHED_IN);
	
//-------------- Create a TreeView widget instance with list-type model(e.g. data)
  /* Create a tree view object. */
  treeView = m_CreateTreeView();

  /* Hide the header of the tree view. */
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeView), FALSE);

  /* Get the tree view selection object from the tree view object. */
  m_TreeSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));

  /* Add the tree view widget into scrolling window. */
  gtk_container_add(GTK_CONTAINER(scrollWin), treeView);

  /* Set the scrolling window's size. */
  gtk_widget_set_size_request(scrollWin, 310, 340);

  /* Put scrolling window to the fixed windows. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), scrollWin, 10, 10);
	
  /* Store the required widgets. */
  m_pWidgets[APPCHOOSER_GtkTreeView] = treeView;	
		
  /* Get and store the tree view selection object. */
  m_pWidgets[APPCHOOSER_GtkTreeSelection] = (GtkWidget*)gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));

//-------------- Create button widget instances     
  /* The button to confirm the user has chosen wanted application. */
  /* Create a button instance. */
  buttonApply = gtk_button_new_with_label(_("Apply"));  /* gtk_button_new_from_stock(GTK_STOCK_APPLY); */  // If you want to use inbuilt button, use gtk_button_new_from_stock() instead.

  /* Set the location in the fixed container. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), buttonApply, 240, 365);

  /* Set the widget's size. */
  gtk_widget_set_size_request(buttonApply, 80, 30);

  /* Set the signal connection for the "add" button */
  g_signal_connect(GTK_OBJECT(buttonApply), "clicked", G_CALLBACK(on_file_apply), this);

  /* The button to exit Desktop App Chooser. */
  /* Create a button instance. */
  buttonClose = gtk_button_new_from_stock(GTK_STOCK_CLOSE); /* gtk_button_new_with_label(_("Close")); */  // If you want to use multi-language, use button_new_with_label() instead.

  /* Set the location of the button in the fixed container. */
  gtk_fixed_put(GTK_FIXED(pFixedContainer), buttonClose, 150, 365);

  /* Set the button's size. */
  gtk_widget_set_size_request(buttonClose, 80, 30);

  /* Set the signal connection for the "Cancel" button.
     This will cause the window to be destroyed by
     calling gtk_widget_destroy(window) when "clicked" event happens.
     The destroy signal could come from here, or the window manager. */
  g_signal_connect(GTK_OBJECT(buttonClose), "clicked", G_CALLBACK(on_close), this);

  return bRet;	
}

/*! \fn void CDesktopAppChooser::m_DeinitValue(void)
    \brief Deinitialize values

    \param[in] NONE
    \return NONE
*/
void CDesktopAppChooser::m_DeinitValue(void)
{
  if(m_pWidgets[APPCHOOSER_GtkTreeView])
  {
     GtkTreeModel *model =NULL;
     GtkTreeIter iter;

     m_TreeStore = GTK_TREE_STORE( gtk_tree_view_get_model(GTK_TREE_VIEW(m_pWidgets[APPCHOOSER_GtkTreeView])) );

     /* Make sure the model stays with us after the tree view unrefs it */
     g_object_ref( (GObject*)m_TreeStore );

     /* Detach model from view */
     gtk_tree_view_set_model(GTK_TREE_VIEW(m_pWidgets[APPCHOOSER_GtkTreeView]), NULL);
     model = (GtkTreeModel*)m_TreeStore;

     /* Initializes iter with the first iterator in the tree
        (the top-level item, e.g. the one at the path "0") */
     if(gtk_tree_model_get_iter_first(model, &iter) == FALSE) 
       return;

     /* To remove the node's data first. */
     gtk_tree_model_foreach(model, (GtkTreeModelForeachFunc)cb_remove_node_data, NULL);

     /* Then, to clear all tree model data. */
     gtk_tree_store_clear(m_TreeStore);
     g_object_unref(m_TreeStore); 
     m_TreeStore = NULL;
  }
}

/*! \fn gboolean CDesktopAppChooser::m_DoModal(void)
    \brief Desktop App Chooser runs in modal mode.

    \return TRUE or FALSE
*/
gboolean CDesktopAppChooser::m_DoModal(void)
{
  /* Sets a window modal or non-modal. */
  gtk_window_set_modal(GTK_WINDOW(m_pWidgets[APPCHOOSER_GtkWindow_Main]), TRUE);

  if( m_pwParent != NULL )
  {
     /* Dialog windows should be set transient for the main application window they were spawned from.
        This allows window managers to e.g. keep the dialog on top of the main window, or center the
        dialog over the main window. */
     gtk_window_set_transient_for(GTK_WINDOW(m_pWidgets[APPCHOOSER_GtkWindow_Main]), GTK_WINDOW(m_pwParent));

     /* Creates the GDK (windowing system) resources associated with a widget. */
     gtk_widget_realize(m_pWidgets[APPCHOOSER_GtkWindow_Main]);

     /* Sets hints about the window management functions to make available via buttons on the window frame. */
     gdk_window_set_functions(m_pWidgets[APPCHOOSER_GtkWindow_Main]->window, (GdkWMFunction)(GDK_FUNC_CLOSE|GDK_FUNC_MOVE));
  }

  /* Show all widgets */
  gtk_widget_show_all(m_pWidgets[APPCHOOSER_GtkWindow_Main]);

  /* Start to run. */
  gtk_main();

//-------------- When the modal is terminated, it must grab the current list store of the tree-view, else it will make a big big trouble!	
  /* To decrease the reference counter of the menu directory object. */
  gmenu_tree_item_unref(m_RootDir);

  /* To decrease the reference counter of the menu tree object. */
  gmenu_tree_unref(m_MenuTree);

  return TRUE;
}

//-------------------------- GtkTreeView
/*! \fn GtkTreeModel* CDesktopAppChooser::m_CreateAndFillModel(void)
    \brief Create and fill tree model contents.

    \param[in] NONE
    \return TRUE or FALSE
*/
GtkTreeModel* CDesktopAppChooser::m_CreateAndFillModel(void)
{
   return GTK_TREE_MODEL(m_TreeStore);
}

/*! \fn GtkWidget* CDesktopAppChooser::m_CreateTreeView(void)
    \brief Create and initialize tree view(GtkTreeView) object.

    \param[in] NONE.
    \return GtkWidget object.
*/
GtkWidget* CDesktopAppChooser::m_CreateTreeView(void)
{
  GtkTreeViewColumn *col = NULL;
  GtkCellRenderer *rendererText = NULL, *rendererPixBuf = NULL;
  GtkWidget *view = NULL;
  GtkTreeModel *model = NULL;

  /* To create a GtkTreeView ble*/
  view = gtk_tree_view_new();

  /* To create a column object. */
  col = gtk_tree_view_column_new();
  //gtk_tree_view_column_set_title(col, "Application Name");
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

  /* To create a PixelBuffer type of cell renderer object. */
  rendererPixBuf = gtk_cell_renderer_pixbuf_new();
  gtk_tree_view_column_pack_start(col, rendererPixBuf, FALSE);	
  gtk_tree_view_column_add_attribute(col, rendererPixBuf, "pixbuf", COLUMN_ICON);	

  /* To create a text type of cell renderer object. */	
  rendererText = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, rendererText, TRUE);
  gtk_tree_view_column_add_attribute(col, rendererText, "text", COLUMN_TEXT);

  /* To create the tree model. */
  model = m_CreateAndFillModel();
  gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
  g_object_unref(model); 	

  return view;
}

//----------------------------------- GMenus Reading Applications ".menu" file
/*========== All things about applications menu starting from here! ==========*/
/*! \fn gboolean CDesktopAppChooser::m_LoadAndBuildAppsMenuTree(void)
    \brief To load the main application menu(.menu) file. 

    \param[in] NONE
    \return TRUE or FALSE
*/
gboolean CDesktopAppChooser::m_LoadAndBuildAppsMenuTree(void)
{
  GSList *directoryList = NULL;

  /*------------ THE ENTRY POINT !!! -------------*/
  /* To open the applications .menu file. */
  m_MenuTree = gmenu_tree_lookup( APPLICATIONS_MENU, GMENU_TREE_FLAGS_NONE );

  /* To store the parsed direcotry contents. */
  m_RootDir = gmenu_tree_get_root_directory( m_MenuTree );

  /* To build the top-level tree node. */
  for(directoryList = gmenu_tree_directory_get_contents( m_RootDir ); 
      directoryList; 
      directoryList = directoryList->next )
  {
     GMenuTreeDirectory *tmpDir = (GMenuTreeDirectory*)directoryList->data;

     /* To check its type. */
     if( G_UNLIKELY(gmenu_tree_item_get_type((GMenuTreeItem*)tmpDir) != GMENU_TREE_ITEM_DIRECTORY) )
       continue;

     /* To build top-level(Directory) nodes. */          
     m_AddAppsMenuTopLevelNode(tmpDir);

     /* To build child nodes(applications). */
     m_AddAppsMenuLeafNode(tmpDir);		
  }

  return true;
}

/*! \fn gboolean CDesktopAppChooser::m_AddAppsMenuTopLevelNode(GMenuTreeDirectory *appsDir)
    \brief To create top-level(Directory) nodes.

    \param[in] appsDir.
    \return TRUE or FALSE
*/
gboolean CDesktopAppChooser::m_AddAppsMenuTopLevelNode(GMenuTreeDirectory *appsDir)
{
  GdkPixbuf *pixbuf = NULL;

  /* To get PixelBuffer of the Directory icon. */
  pixbuf = m_LoadIcon(gmenu_tree_directory_get_icon(appsDir), IMG_SIZE, TRUE ); 

  /* To set the columns' content. */
  gtk_tree_store_append(m_TreeStore, &m_TopLevelNodeIter, NULL);
  gtk_tree_store_set(m_TreeStore, &m_TopLevelNodeIter,
                     COLUMN_ICON, pixbuf,
                     COLUMN_TEXT, gmenu_tree_directory_get_name(appsDir),
                     COLUMN_NODEDATA, NULL,
                     -1);

  /* pixbuf has a referece count of "1" now, as the tree store has added its own reference. 
     So to decrease the reference count of pixbuf. */
  g_object_unref(pixbuf);

  return true;
}

/*! \fn gboolean CDesktopAppChooser::m_AddAppsMenuLeafNode(GMenuTreeDirectory *appsDir)
    \brief To create leaf nodes(applications).

    \param[in] appsDir
    \return TRUE or FALSE
*/
gboolean CDesktopAppChooser::m_AddAppsMenuLeafNode(GMenuTreeDirectory *appsDir)
{
  GSList *itemList = NULL;

  if( G_UNLIKELY(!appsDir) )
    return false;

  for( itemList = gmenu_tree_directory_get_contents(appsDir);
       itemList;
       itemList = itemList->next )
  {
     GMenuTreeItem  *item = (GMenuTreeItem*)itemList->data;
     GMenuTreeItemType type = gmenu_tree_item_get_type(item);

     if( type == GMENU_TREE_ITEM_DIRECTORY )
     {
        /* To pars sub-directory items and add it to the menu tree.
           Here using recursive parse. */
        m_AddAppsMenuLeafNode( (GMenuTreeDirectory*)item );
     }
     else if( type == GMENU_TREE_ITEM_ENTRY )
     {
        GdkPixbuf *pixbuf = NULL;
        const gchar *icon_name = gmenu_tree_entry_get_icon( (GMenuTreeEntry*)item );
        APP_ITEM_INFO *appInfo = NULL;
				
        /* To determine if the current item need not to be shown. If it is, continue. */
        if(gmenu_tree_entry_get_is_nodisplay( (GMenuTreeEntry*)item ) || 
           gmenu_tree_entry_get_is_excluded( (GMenuTreeEntry*)item ) )
          continue;
					
        /* To create the icon for the currently read node. */
        if(icon_name)
          pixbuf = m_LoadIcon(icon_name, IMG_SIZE, TRUE );
        else
          pixbuf = m_LoadIcon(DEFAULT_APP__MIME_ICON, IMG_SIZE, TRUE );  // If there has no icon name in .desktop file, using the system default icon for application.

        /* To create a object containing information about current leaf node. */
        appInfo = new APP_ITEM_INFO ;

        /* Zero out the new allocated memory. */
        memset(appInfo, 0x00, sizeof(APP_ITEM_INFO));

        if(appInfo)
        {
           gchar *name = (gchar*)gmenu_tree_entry_get_name( (GMenuTreeEntry*)item ) ;
           gchar *exec = (gchar*)gmenu_tree_entry_get_exec( (GMenuTreeEntry*)item );
           gchar *comment = (gchar*)gmenu_tree_entry_get_comment( (GMenuTreeEntry*)item );

           #if 1
           gchar *desktopfile = (gchar*)gmenu_tree_entry_get_desktop_file_path( (GMenuTreeEntry*)item );
           #else
           gchar *desktopfile = (gchar*)gmenu_tree_entry_get_desktop_file_id( (GMenuTreeEntry*)item );
           #endif

           if(name)
             appInfo->name = (gchar*)g_strdup(name);

           if(icon_name)
             appInfo->icon = (gchar*)g_strdup(icon_name);

           if(exec)
             appInfo->exec = (gchar*)g_strdup(exec);

           if(comment)
             appInfo->comment = (gchar*)g_strdup(comment);

           if(desktopfile)
             appInfo->desktopfile = (gchar*)g_strdup(desktopfile);
        }

        /* Add a tree leaf. */
        gtk_tree_store_append(m_TreeStore, &m_ChildNodeIter, &m_TopLevelNodeIter);
        gtk_tree_store_set(m_TreeStore, &m_ChildNodeIter,
                           COLUMN_ICON, pixbuf,
                           COLUMN_TEXT, gmenu_tree_entry_get_name( (GMenuTreeEntry*)item ),
                           COLUMN_NODEDATA, appInfo,
                           -1);					
				
        /* pixbuf has a referece count of "1" now, as the tree store has added its own reference. 
           So to decrease the reference count of pixbuf. */
        g_object_unref(pixbuf);
    }
  } // The end of for() 

  return true;
}

/*! \fn GdkPixbuf* CDesktopAppChooser::m_LoadIcon(const gchar* name, gint size, gboolean use_fallback)
    \brief To load a icon's image contents.

    \param[in] name.
    \param[in] size. 
    \param[in] use_fallback.
    \return PixelBuffer object representing the designated icon.
*/
GdkPixbuf* CDesktopAppChooser::m_LoadIcon(const gchar* name, gint size, gboolean use_fallback)
{
  GtkIconTheme *theme = NULL;
  gchar *icon_name = NULL, *suffix = NULL;
  GdkPixbuf *icon = NULL;

  if(name)
  {
    if( g_path_is_absolute( name) )
      icon = gdk_pixbuf_new_from_file_at_scale( name, size, size, TRUE, NULL );
    else
    {
      theme = gtk_icon_theme_get_default();
      suffix = strchr((char*)name, '.' );
	
      if(suffix)  /* Having file extension, it is the basename of icon file */
      {
        /*Try to find it in "pixmaps", "icons/hicolor" and "icons/hicolor/scalable/apps" directories */
        icon = m_LoadIconFile( name, size );

        if(G_UNLIKELY(!icon))   /* unfortunately, it is not found */
        {
           /* Let's remove the suffix and see if this name can match an icon in current icon theme */
           icon_name = g_strndup(name, (suffix-name) );
           icon = m_LoadThemeIcon(theme, icon_name, size );
           g_free( icon_name );
        }
      }
      else  /* no file extension, it could be an icon name in the icon theme */
        icon = m_LoadThemeIcon( theme, name, size );
    }
  }
	
  if(G_UNLIKELY(!icon) && use_fallback)  /* fallback to generic icon */
  {
     theme = gtk_icon_theme_get_default();
     icon = m_LoadThemeIcon(theme, DEFAULT_APP_ICON, size);

     if( G_UNLIKELY(!icon) )  /* fallback to generic icon */
       icon = m_LoadThemeIcon(theme, DEFAULT_APP__MIME_ICON, size );
  }
	
  return icon;
}

/*! \fn GdkPixbuf* CDesktopAppChooser::m_LoadIconFile(const char* file_name, int size)
    \brief Try to find it in "pixmaps", "icons/hicolor", "icons/hicolor/scalable/apps" directories.

    \param[in] file_name. The icon name for searching.
    \param[in] size. The width(height) of the icon for searching. 
    \return PixelBuffer object representing the designated icon.
*/
GdkPixbuf* CDesktopAppChooser::m_LoadIconFile(const char* file_name, int size)
{
  GdkPixbuf* icon = NULL;
  char *file_path = NULL;
  const gchar **dirs = (const gchar**)g_get_system_data_dirs();   /* To read the environment variable which is specified in freedesktop.org Base Directory. */
  const gchar **dir = NULL;
  gchar *sizeName = NULL;

  for( (dir = dirs); *dir; ++dir )
  {
     /* Searching in "/usr/share/pixmaps" directory */
     if(g_strrstr(file_name, ".") &&
        (g_strrstr(file_name, EXT_NAME_PNG) || g_strrstr(file_name, EXT_NAME_XPM) || g_strrstr(file_name, EXT_NAME_SVG)) )
     {
        file_path = g_build_filename( *dir, ICON_SEARCH_PATH_PIXMAPS, file_name, NULL );
        icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

        if(file_path)		
          g_free(file_path);

        file_path = NULL;		

        if( icon )
		  break;
     }
     else
     {
        gchar *tempFileName = NULL;

        /* Try to load icon file with ".png" extension. */
        tempFileName = g_strdup_printf("%s%s", file_name, EXT_NAME_PNG);
        file_path = g_build_filename( *dir, ICON_SEARCH_PATH_PIXMAPS, tempFileName, NULL );
        icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

        if(tempFileName)
          g_free(tempFileName);

        tempFileName = NULL;

        if(file_path)		
          g_free(file_path);

        file_path = NULL;		

        if( icon )
          break;

        /* Try to load icon file with ".xpm" extension. */
        tempFileName = g_strdup_printf("%s%s", file_name, EXT_NAME_XPM);
        file_path = g_build_filename( *dir, ICON_SEARCH_PATH_PIXMAPS, tempFileName, NULL );
        icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

        if(tempFileName)
          g_free(tempFileName);

        tempFileName = NULL;

        if(file_path)		
          g_free(file_path);

        file_path = NULL;		

        if( icon )
          break;

        /* Try to load icon whose name in the end of  ".svg" */
        tempFileName = g_strdup_printf("%s%s", file_name, EXT_NAME_SVG);
        file_path = g_build_filename( *dir, ICON_SEARCH_PATH_PIXMAPS, tempFileName, NULL );
        icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

        if(tempFileName)
          g_free(tempFileName);

        tempFileName = NULL;

        if(file_path)		
          g_free(file_path);

        file_path = NULL;		

        if( icon )
          break;
     }	

     /* Searching in "/usr/share/icons/hicolor/SizexSize/apps", 
        "/usr/local/share/icons/hicolor/SizexSize/apps" directories.
        These are needed to assign the directory name formed in size. */
     sizeName = g_strdup_printf("%s/%dx%d/apps", ICON_SEARCH_PATH_HICOLOR, size, size);
     file_path = g_build_filename( *dir, sizeName, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)		
       g_free( file_path );

     if(sizeName)		
       g_free(sizeName);

     file_path = NULL;	
     sizeName = NULL;	

     if( icon )
       break;

     /* Searching in "/usr/share/icons/hicolor/scalable/apps",
        "/usr/local/share/icons/hicolor/scalable/apps" directories. */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_HICOLOR_SCALABLE, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)		
       g_free( file_path );

     file_path = NULL;	

     if( icon )
       break;

     /* Searching in "/usr/share/icons/gnome/scalable" directory. */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_GNOME_SCALABLE, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)
       g_free( file_path );

     file_path = NULL;	

     if( icon )
       break;

     /* Searching in "/usr/share/icons/gnome/scalable/apps" directory. */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_GNOME_SCALABLE_APPS, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)
       g_free( file_path );

     file_path = NULL;	

     if( icon )
       break;

     /* Searching in "/usr/share/icons/gnome/SizexSize/apps" directory.
		These are needed to assign the directory name formed in size. */
     sizeName = g_strdup_printf("%s/%dx%d/apps", ICON_SEARCH_PATH_GNOME, size, size);
     file_path = g_build_filename( *dir, sizeName, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(file_path)
       g_free (file_path);

     if(sizeName)
       g_free(sizeName);

     file_path = NULL;
     sizeName = NULL;

     if( icon )
       break;
  }

  return icon;
}

/*! \fn GdkPixbuf* CDesktopAppChooser::m_LoadThemeIcon(GtkIconTheme* theme, const char* icon_name, int size)
    \brief To load a icon contents found in theme icon pool.

    \param[in] theme.
    \param[in] icon_name. 
    \param[in] size.
    \return PixelBuffer object representing the designated icon.
*/
GdkPixbuf* CDesktopAppChooser::m_LoadThemeIcon(GtkIconTheme* theme, const char* icon_name, int size)
{
  GdkPixbuf *icon = NULL;
  const char *file = NULL;
  GtkIconInfo *info = gtk_icon_theme_lookup_icon(theme, icon_name, size, GTK_ICON_LOOKUP_USE_BUILTIN);

  if( G_UNLIKELY(!info) )
    return NULL;

  file = gtk_icon_info_get_filename( info );

  if( G_LIKELY( file ) )
    icon = gdk_pixbuf_new_from_file( file, NULL );
  else
    icon = gtk_icon_info_get_builtin_pixbuf( info );

  gtk_icon_info_free( info );

  if( G_LIKELY( icon ) )
  {
	int height = gdk_pixbuf_get_height(icon);
    int width = gdk_pixbuf_get_width(icon);

    /* Scale down the icon if it's too big to be shown. */
    if(G_UNLIKELY( (height > size) || (width > size) ))
    {
      GdkPixbuf *scaled = NULL;

      if( height > width )
      {
         width = size * height / width;
		 height = size;
      }
      else if( height < width )
      {
         height = size * width / height;
         width = size;
      }
      else
         height = width = size;

      scaled = gdk_pixbuf_scale_simple( icon, width, height, GDK_INTERP_BILINEAR );
      g_object_unref( icon );
      icon = scaled;
    }
  }

  return icon;
}

/*! \fn gchar* CDesktopAppChooser::m_GetIconFullName(const char* file_name, int size )
    \brief Try to find it in "pixmaps", "icons/hicolor", "icons/hicolor/scalable/apps" directories.

    \param[in] file_name.
    \param[in] size.
    \return Newly allocated string representing the icon's full name.
*/
gchar* CDesktopAppChooser::m_GetIconFullName(const char* file_name, int size)
{
  GdkPixbuf *icon = NULL;
  char *file_path = NULL;
  const gchar **dirs = (const gchar**)g_get_system_data_dirs();
  const gchar **dir = NULL;
  gchar *sizeName = NULL; 
	
  for( (dir = dirs); *dir; ++dir )
  {
     /* Searching in "/usr/share/pixmaps" */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_PIXMAPS/*"pixmaps"*/, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if( icon )
     {
        /* pixbuf has a referece count of "1" now, as the tree store has added its own reference. 
           So to decrease the reference count of pixbuf. */
        g_object_unref(icon);
        break;
     }
     else
     {
        if(file_path)		
          g_free(file_path);

        file_path = NULL;		
     }

     /* Searching in "/usr/share/icons/hicolor/SizexSize/apps", "/usr/local/share/icons/hicolor/SizexSize/apps" directories.
        These are needed to assign the directory name formed in size. */
     sizeName = g_strdup_printf( "%s/%dx%d/apps", ICON_SEARCH_PATH_HICOLOR, size, size);
     file_path = g_build_filename( *dir, sizeName, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(sizeName)		
       g_free(sizeName);

     sizeName = NULL;	
		
     if( icon )
     {
        /* pixbuf has a referece count of "1" now, as the tree store has added its own reference. 
           So to decrease the reference count of pixbuf. */
        g_object_unref(icon);
        break;
     }
     else
     {
        if(file_path)		
          g_free( file_path );

        file_path = NULL;	
     }

     /* Searching in "/usr/share/icons/hicolor/scalable/apps", "/usr/local/share/icons/hicolor/scalable/apps" directories. */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_HICOLOR_SCALABLE, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if( icon )
     {
        /* pixbuf has a referece count of "1" now, as the tree store has added its own reference. 
           So to decrease the reference count of pixbuf. */
        g_object_unref(icon);
        break;
     }
     else
     {
        if(file_path)		
          g_free( file_path );

        file_path = NULL;			
     }

     /* Searching in "/usr/share/icons/gnome/scalable". */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_GNOME_SCALABLE, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if( icon )
     {
        /* pixbuf has a referece count of "1" now, as the tree store has added its own reference. 
           So to decrease the reference count of pixbuf. */
        g_object_unref(icon);
        break;
     }
     else
     {
        if(file_path)
          g_free( file_path );

        file_path = NULL;	
     }

     /* Searching in "/usr/share/icons/gnome/scalable/apps". */
     file_path = g_build_filename( *dir, ICON_SEARCH_PATH_GNOME_SCALABLE_APPS, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if( icon )
     {
        /* pixbuf has a referece count of "1" now, as the tree store has added its own reference. 
           So to decrease the reference count of pixbuf. */
	    g_object_unref(icon);
        break;
     }
     else
     {
        if(file_path)
          g_free( file_path );

        file_path = NULL;	
     }

     /* Searching in "/usr/share/icons/gnome/SizexSize/apps".
        These are needed to assign the directory name formed in size. */
     sizeName = g_strdup_printf( "%s/%dx%d/apps", ICON_SEARCH_PATH_GNOME, size, size );
     file_path = g_build_filename( *dir,  sizeName, file_name, NULL );
     icon = gdk_pixbuf_new_from_file_at_scale( file_path, size, size, TRUE, NULL );

     if(sizeName)
       g_free(sizeName);

     sizeName = NULL;

     if ( icon )
     {
        /* pixbuf has a referece count of "1" now, as the tree store has added its own reference. 
           So to decrease the reference count of pixbuf. */
        g_object_unref(icon);			
        break;		
     }
	 else
     {
        if(file_path)
          g_free (file_path);

        file_path = NULL;
     }
  }

  return file_path;
}
