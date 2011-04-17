If you find bugs and/or have feature requests, please use the tracking system provided by sourceforge.

As of version 0.9.6 each release is tested under Windows 7 professional (amd64) and Xubuntu 10.10 (amd64).
When building from source make sure that you copy the "au" executable into the same folder as "fourchan-dl".

Release notes
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
