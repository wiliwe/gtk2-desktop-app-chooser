/*! \file    CDesktopAppChooser.h
    \brief   Provide the user an UI to select currently installed X desktop applications.

    \author  William.L
    \date    2008-09-11
    \version 1.0

    \b Change_History: 
    \n 1) 2008-09-11 William.L initialized. 
*/

#ifndef __CDESKTOPAPPCHOOSER_H
#define __CDESKTOPAPPCHOOSER_H

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GMENU_I_KNOW_THIS_IS_UNSTABLE  /* This definition must be added else it will fail to build the image. */
#include <gmenu-tree.h>	 /* GNOME Menus library header. */

/* The alternative icons searching path. */
#define ICON_SEARCH_PATH_PIXMAPS  "pixmaps"
#define ICON_SEARCH_PATH_HICOLOR  "icons/hicolor"
#define ICON_SEARCH_PATH_HICOLOR_SCALABLE  "icons/hicolor/scalable/apps"
//
#define ICON_SEARCH_PATH_GNOME                "icons/gnome"
#define ICON_SEARCH_PATH_GNOME_SCALABLE       "icons/gnome/scalable"
#define ICON_SEARCH_PATH_GNOME_SCALABLE_APPS  "icons/gnome/scalable/apps"

/* Extension names of images. */
#define EXT_NAME_PNG  ".png"
#define EXT_NAME_XPM  ".xpm"
#define EXT_NAME_SVG  ".svg"

/*! \enum  APPCHOOSER_WIDGET_IDX
    \brief The indices of the widget array.
*/
enum APPCHOOSER_WIDGET_IDX {
  APPCHOOSER_GtkWindow_Main = 0,
  APPCHOOSER_GtkTreeView,
  APPCHOOSER_GtkTreeSelection,
  APPCHOOSER_GtkTreeStore,
  N_APPCHOOSER_WIDGET_IDX
};

/*! \struct APP_ITEM_INFO
    \brief The application item's information. This follows freedesktop.org Desktop Entry specification.
*/
typedef  struct {
  gchar *name;
  gchar *icon;
  gchar *exec;
  gchar *comment;
  gchar *desktopfile;
} APP_ITEM_INFO;

/*! \class CDesktopAppChooser
    \brief The X desktop applications chooser GUI class
*/
class CDesktopAppChooser
{
  private:
    GtkWidget *m_pwParent;  /*!< Store the pointer to the parent(top-level) window. */
    int m_nMaxWidth;   /*!< The width of the Desktop App Chooser window. */
    int m_nMaxHeight;  /*!< The height of the Desktop App Chooser window. */

    /* GtkTreeView relevant variables. */
    GtkWidget *m_TreeViewTree;
    GtkTreeSelection *m_TreeSelection;  /*!< The selection instance gotten from the created TreeView. */
    GtkTreeStore  *m_TreeStore;         /*!< The GtkTreeStore type memer variable. */
    GtkTreeIter m_TopLevelNodeIter, m_ChildNodeIter;  /*!< The tree iterate objects represent top-level and child nodes respectively. */
    GtkWidget *m_pWidgets[N_APPCHOOSER_WIDGET_IDX];   /*!< This is used to store widget instances for accessing in the event handle callback function. */
    APP_ITEM_INFO  m_SelectedAppItemInfo;             /*!< To store the information about the selected system installed application. */
    gboolean m_bIsChosen;  /*!< To indicate if a applicatoin is chosen. */

    /* GNOME Menus relevant variables. */   
    GMenuTree *m_MenuTree;          /*!< The application menu tree. */
    GMenuTreeDirectory *m_RootDir;  /*!< The directories' content. */

  public:
    /* The constructor and the destructorof class CDesktopAppChooser. */
    CDesktopAppChooser();
    ~CDesktopAppChooser();

    void m_CreateInitValue(void);
    void m_DeinitValue(void);
    void m_GetWindowSize(int &nWidth, int &nHeight);
    GtkWidget* m_GetWidget(int idx) { return m_pWidgets[idx]; }  /*!< To get the widget object */
    gboolean m_InitLayoutUI(GtkWidget *pwGtkParent, int nPosX, int nPosY);
    gboolean m_DoModal(void);   /*!< For dialog window.  */

    /* Desktop Entry relevant functions. */
    APP_ITEM_INFO* m_GetSelectedAppItem(void) { return  &m_SelectedAppItemInfo; }  /*!< To retrive the chosen application item's Desktop Entry information object. */         
    gchar* m_GetSelectedAppItem_Name(void) { return (m_SelectedAppItemInfo.name != NULL)? m_SelectedAppItemInfo.name : (gchar*)""; }  /*!< To retrive the chosen application's Desktop Entry "name" value. */         
    gchar* m_GetSelectedAppItem_Icon(void) { return (m_SelectedAppItemInfo.icon != NULL)? m_SelectedAppItemInfo.icon : (gchar*)""; }  /*!< To retrive the chosen application's Desktop Entry "icon" value. */         
    gchar* m_GetSelectedAppItem_Exec(void) { return (m_SelectedAppItemInfo.exec != NULL)? m_SelectedAppItemInfo.exec : (gchar*)""; }  /*!< To retrive the chosen application's Desktop Entry "exec" value. */         
    gchar* m_GetSelectedAppItem_Comment(void) { return (m_SelectedAppItemInfo.comment != NULL)? m_SelectedAppItemInfo.comment : (gchar*)""; }  /*!< To retrive the chosen application's Desktop Entry "comment" value. */         
    gchar* m_GetSelectedAppItem_DesktopEntry(void) { return (m_SelectedAppItemInfo.desktopfile != NULL)? m_SelectedAppItemInfo.desktopfile : (gchar*)""; }  /*!< To retrive the chosen application item's desktop entry file full path. */         

    /* GNOME Menus relevant functions */
    GtkTreeModel* m_CreateAndFillModel(void);
    GtkWidget* m_CreateTreeView(void);
    gboolean m_LoadAndBuildAppsMenuTree(void);  /*!< To load the main application menu content and build a tree representing menu contents. */ 
    gboolean m_AddAppsMenuTopLevelNode(GMenuTreeDirectory *appsDir);  /*!< To addd top-level tree nodes. */
    gboolean m_AddAppsMenuLeafNode(GMenuTreeDirectory *appsDir);       /*!< To create the leaves of the applications menu contents. */
    GdkPixbuf* m_LoadIcon( const gchar* name, gint size, gboolean use_fallback );  /*!< To load a icon's image. */
    GdkPixbuf* m_LoadIconFile( const char* file_name, int size );
    GdkPixbuf* m_LoadThemeIcon( GtkIconTheme* theme, const char* icon_name, int size );
    gchar* m_GetIconFullName(const char* file_name, int size);
    //
    void m_SetIsChosen(gboolean chosen) { m_bIsChosen = chosen; }  /*!< Set the bool value indicating if an application item is chosen. */
    gboolean m_GetIsChosen(void) { return m_bIsChosen; }  /*!< Get the bool value indicating if an application item is chosen. */
};
#endif /* __CDESKTOPAPPCHOOSER_H	*/
