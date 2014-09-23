/*! \file main.cpp
    \brief To demostrate how to use X desktop application chooser.

    \author William.L
    \date 2008-09-11
    \version 1.0

    \b Change_History: 
    \n 1) 2008-09-11 William.L initial version. 
*/

#include <stdio.h>
#include <glib/gi18n.h>   // For GNU gettext i18n, multi-language
#include <locale.h>  // For setlocale() function.

#include "CDesktopAppChooser.h"

//#define TEST

#ifdef TEST
/* For GNU gettext i18n, multi-language */
#define PACKAGE   "DesktopAppChooser"
#define LOCALEDIR "./locale"

int main(int argc, char* argv[])
{
  CDesktopAppChooser appChooser;

  /* For GNU gettext i18n, multi-language */
  setlocale(LC_ALL, "");    // Clear out LC_ALL environment variable.
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  /* First of all, call gtk_init() to initialize GTK type system.

     If you do not call this first of all GTK codes,
     you will get a error as : 
           "You forgot to call g_type_init()"
  */
  gtk_init (&argc, &argv);

  printf("Initialize data model \n");
  appChooser.m_CreateInitValue();  

  printf("Prepare UI layout \n");
  appChooser.m_InitLayoutUI(NULL, 700, 400);  // No top level parent window, so the first parameter is set to NULL.

  printf("Start to show dialog \n");
  if( appChooser.m_DoModal() )
    printf("X Desktop App Chooser dialog terminated!\n\n");
		
  /* To set Name, Exec and Comment fields of the dialog. */
  if(appChooser.m_GetIsChosen())
  {
     printf("[ Desktop Entry : %s ]\n", (char*)appChooser.m_GetSelectedAppItem_DesktopEntry() );

     printf("APP Name : %s\n", (char*)appChooser.m_GetSelectedAppItem_Name() );
     printf("APP Command : %s\n", (char*)appChooser.m_GetSelectedAppItem_Exec() );
     printf("APP Icon Path : %s\n", (char*)appChooser.m_GetSelectedAppItem_Icon() );
     printf("APP Comment : %s\n", (char*)appChooser.m_GetSelectedAppItem_Comment() );

     printf("\n");
  }

  return 0;
}
#endif
