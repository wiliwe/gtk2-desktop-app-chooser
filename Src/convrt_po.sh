#!/bin/sh
msgfmt ./po/DesktopAppChooser.po -o ./po/DesktopAppChooser.mo
cp ./po/DesktopAppChooser.mo ./locale/zh_TW.UTF-8/LC_MESSAGES/
cp ./po/DesktopAppChooser.mo ./locale/zh_TW.utf-8/LC_MESSAGES/

