======================================================================================
                                   ZipALot Readme
======================================================================================

ZipALot Version 3.2

What's that?
======================================================================================

Most of the unpack programs only can handle one archive at once. 

ZipALot takes all the archives from one directory and extracts them to another one.

ZipALot can currently handle ZIP, RAR and ACE (V.2) archives.

Filter options allow you to control the extraction process. 
Tell which archives you want to extract, which files you want to have extracted from
the archives and which not.
With a simple click you extract i.e. all your images from dozens of archives!

Advanced rules give you even more powerful options...
You can enter multiple filter rules in the "Extract" and "Don't extract" fields.
Separate them with a slash ("/").

An example how to use these filter rules:

Archives:      images*.zip//
Extract:        *.jpg/*.jpeg/*.gif
Don't extract:  old*/tmp*

All ZIP-files that begin with "images" are processed (i.e. images1.zip, images2.zip,
imagesNew.zip...). All files with the extension jpg, jpeg and gif are extracted but
those beginning with "old" or "tmp".

The NEW special Warez-Wizard makes the installation of your warez even more
comfortable ;-)

If there's an option you miss, feel free to contact me... (see below)

Some tipps:
======================================================================================
Right-click on a directory in the windows explorer and choose "Extract archives
with ZipALot". This will open ZipALot with the choosen directory.

Drop a directory on ZipALot to make it the current source-directory.

Use the warez-wizard to auto-detect self-extracting archives (*.exe).

When running the warez-wizard, hover over the ligh-bulb. It gives you advices on
how to install the program.

LICENSE
======================================================================================
ZipALot is licensed under the GNU General Public License (GPL) and is therefore
freely distributable. You can get the source-code at 
http://www.ractive.ch/gpl/download/ZipALot_src.zip

INSTALLATION
======================================================================================
Run the installation program ZipALot.exe or ZipALot_linked.exe :-)

PROBLEMS
======================================================================================
If you get error messages like "...MFCxx.DLL not found..." you don't have installed
the MFC libraries.
You can download a linked version of ZipALot at:
http://www.ractive.ch/gpl/download/ZipALot_linked.exe

If you find bugs or if you miss a feature, contact me (see below).

CONTACT
======================================================================================
Author:   Jean-Pierre Bergamin <james@ractive.ch>
Homepage: http://www.ractive.ch/gpl/zipalot.html
E-mail:   zipalot@ractive.ch
======================================================================================