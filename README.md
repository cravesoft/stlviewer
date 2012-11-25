# STL Viewer

This is the README for the software STLViewer,
a free cross-platform viewer of STL files.

The STLViewer home web site is

    http://stlviewer.sourceforge.net

If you want to send bug reports or feature requests,
please submit them to the sourceforge bug reporting page
and feature request tracker respectively.

Icons are partly retrieved from the silk icon set,
which can be found at http://www.famfamfam.com and
from the WooFunction set, which can be found at
http://www.woothemes.com/2009/09/woofunction/

## Screenshots

![Screenshot1](http://sourceforge.net/projects/stlviewer/screenshots/240422)
![Screenshot1](http://sourceforge.net/projects/stlviewer/screenshots/240424)
![Screenshot1](http://sourceforge.net/projects/stlviewer/screenshots/240426)

## Installation 

### Get the source

Clone the git repository:

``` bash
git clone git@github.com:cravesoft/stlviewer.git
```

### Install dependencies

For Ubuntu, simply enter the following command that will install all necessary packages:

``` bash
sudo apt-get install libqtcore4 libqtgui4 libqt4-opengl
```

### Compile

``` bash
qmake && make
```

This will generate an executable called `stlviewer`.
