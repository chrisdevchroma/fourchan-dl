If you find bugs and/or have feature requests, please use the tracking system provided by sourceforge.

Release notes

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
