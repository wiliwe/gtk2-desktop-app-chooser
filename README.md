GTK+ 2.0 App - Desktop App Chooser
====================

* Author: William.L  
* Email: <wiliwe@gmail.com>  
* SlideShare: <http://www.slideshare.net/wiliwe/>  

Purpose
-------
As a tool written in GTK+ 2.0 library to let a user browse installed X desktop applications and choose an application to retrieve its desktop entry contents for further usage:  
######_GTK+ 2.0 App - Desktop App Chooser_######
<http://www.slideshare.net/wiliwe/>  

Folders
-------
####_Src_####
  Contains source codes and shell scripts to retrieve gettext string and conver it to MO file.  
  Just run the command `make` to build code.  
  
  `get_text.sh` - to retrieve gettext enclosed string into a .po file and rename this .po file to .pot file.
  `convrt_po.sh` - to convert translated .po file into .mo file and copy the .mo file into the sub-directories under
Src/local/.
####_Src/doxygen_####
  Contains Doxygen configuration files for generating document with or without graphs.  
  There have two generated HTML document(with graph) archives compressed in tarball and 7zip,  
  the contents of them are the same.. 
####_Src/po_####
  Contains GNU gettext PO, POT and MO files.
####_Src/local_####
  Contains GNU gettext MO files which are for Traditional Chinese with UTF-8 encoding.
####_Ref_Docs_####
  Contains two reference document archives compressed using tarball and 7zip, the contents of them are the same.
