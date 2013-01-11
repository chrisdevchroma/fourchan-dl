If you find bugs and/or have feature requests, please use the tracking system provided by sourceforge.

As of version 0.9.6 each release is tested under Windows 7 professional (amd64) and Xubuntu 10.10 (amd64).

When updating from 1.2.0 to 1.3.0 please close the updater executable manually.

Release notes
Version 1.5.0
* Addressed instability issues
* More debug capabilities 

Version 1.4.3
* SSL error on windows7 fixed
* Download status now working correctly

Version 1.4.2
* Small bugfix release

Version 1.4.1
* Added slideshow for the internal viewer
* Save path now supports formatting
 - e.g. C:/%b/%n will result in C:/wg/1234678
 - details on what formattings strings are supported are in the info dialog for each plugin
* Added proper debug system
* Added possibility to rebuild thumbnails for each thread and re-download a complete thread
* Some UI tweaks

Version 1.4.0
* Added internal image viewer
* Added "Close to systray"
* Added Autosave function (every 10 minutes)

Version 1.3.1
* Added checkbox to prevent closing of overview threads (e. g. boards.4chan.org/wg/2). -> Download ALL the 4chan!
* Hopefully fixed bug #3434272
* Added fancy progress bar for Win7 users

Version 1.3.0
* Realised that one QNetworkAccessManager only opens up to 5 HTTP requests. If more simultaneous requests are desired an appropriate number of AccessManagers are created
* Added a plugin system to support others chans than 4chan
* Added plugin for 4chan
* Added plugin for 2chan (not thoroughly tested, because my japanese is not existent)
* The update files are now available from sourceforge in the webupdate folder
* The Qt system files can be updated
* Fixed issue that download stalls with one missing file
* Program menu is (nearly) back to the old version

Version 1.2.0
* Added folder shortcuts - They can be defined in the options dialog
* Download manager now identifies as Opera (just for the statistics :D)
* Added thread overview widget for easier navigation
* Added a history of recently closed threads
* Replaced the toolbar with a normal menu to achieve the history function
* Added a lot of shortcuts:
 - Global
   Ctrl+A   Add single thread
   Ctrl+M   Add multiple threads
   Ctrl+O   Bring up thread overview widget
   Ctrl+S   Start all threads
   Ctrl+T   Stop all threads
   Ctrl+N   Configuration dialog (couldn't think of a better one)
   Ctrl+W   Close current tab
   Ctrl+Q   Close program
 - Only in thumbnail view
   Return/Enter Open image
   Delete       Delete image
   R            Reload image
* Preventing opening an already opened thread
* Not available images will not cause the thread to be closed

Version 1.1.0
* Fixed program crash when a lots of tabs were open
* Some UI enhancements
* Added a dialogue to open multiple threads
 - It watches the clipboard, so copying a link or HTML source code with 4chan links will make them appear in this list
* To fix the mentioned code there is now only _one_ download thread. This means that the overall download with lots of images may appear slower,
  since it is only downloading the images tabwise and not all tabs at the same
* There is also only one thread which creates thumbnails, so this will also may appear slower than before. All CPU is used though.
* This leads to less memory usage, which is worth all the waiting though
* The thumbnail cache is enabled per default and it cannot be changed anymore due to a bug in the linux version
  - The linux version still complains about using QPixmap outside the UI thread, but I am not using it outside. So at the moment I will generously ignore that.
* I updated my Qt framework so you may download the complete package for the latest Qt libraries

Version 1.0.0
* Added thumbnail cache
* Fixed bug 3302967

Version 0.9.8
* Added support for proxies - I couldn't verify complete functionality with enabled proxy though, because all free proxies are banned

Version 0.9.7
* Added Error Handler for image download
  - When an image is unavailable it will be rescheduled for download
  - This should fix bug 3271618
* Added buttons to open thread and download folder
* Double click on image now opens it
* Added a blacklist system
  - When deleting a file the url is blacklisted and will not be downloaded again
  - The blacklist can be turned of in the configuration dialog
  - To keep the list as small as necessary it will check at a given interval if the images are still available (not 404ed)

Version 0.9.6
* Using different RegExp to retrieve files to get rid of "(...)" patterns - Thanks to Chris Moeller (kode54)
* Added define which makes Qt3 support unnecessary when compiling with Qt versions < 4.7 - Thanks to Salvador Parra Camacho
* Added ico file to resources that the application icon is displayed in taskbar
* Supporting top level URIs e. g. http://www.4chan.org/wg/1 will download images of all threads on that page
* Program now comes with an updater (au.exe) which replaces the executable once a new version is available (currently windows and linux only)

Version 0.9.5
* Automatic reloading of partially received images
* Added freely configurable rescan timer values
* Added option to select if directory values should be saved within the session (inheriting directory value when creating a new tab)
* Added progress bar indicating how many thumbnails are in the queue to be rendered
* Fixed bug, where reloading of files with option "Save with original filename" lead to reloading wrong image
* Fixed wrong renaming of images when original filenames are equal due to elided text at 4chan post
* Some smaller bugfixes

- Storing of rescanning values have changed in settings.ini, so at the first start of this version the values have to be reset

Version 0.9.4
* Moved thumbnails creation into own thread to not block the UI
* for each thread files with the same filename are saved with an appended counter
* bugfixes concerning the progress bar
* Bug fixed that savepath is empty after cancelling the folder selection dialog

Version 0.9.3
* Saving state of each tab so that it automatically starts if it was running the last time
* Automatic checking if new version is available
* Changed Progressbar text content (always showed one more)

Version 0.9.2
* Added Configuration Dialog with options to
 - resume from previous session
 - select default download folder
 - set size for thumbnail images
 - select rendering method for thumbnails (HQ uses a lot more CPU)
 - set the maximum number of concurrent downloads per tab (if you are experiencing corrupted downloads decreasing this number may help)
 - select tab position
* Settings can be changed while having downloads in progress, only thumbnail size is ignored once a thumbnail has been created

Version 0.9.1
* Tabs are changing name now to see current download status even when tab is not active
* Option for preserving original filenames added

Version 0.9
* Initial release
* Supports multiple tabs
* Automatic url pasting from clipboard when opening a new tab
* Rembering of savepath in one program instance (not persistent yet)
* Image preview and simple file operations
* When deleting a file it will not be downloaded again, despite of watching the image thread in a new tab


For convenience for older versions:
Current version 1.3.0