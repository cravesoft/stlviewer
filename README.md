# STLViewer

This is the README for the software STLViewer,
a free cross-platform viewer of STL files.

If you want to send bug reports or feature requests,
please submit them to the github issues and pull requests
pages respectively.

Icons are partly retrieved from the Font Awesome icon set,
which can be found at
http://fortawesome.github.io/Font-Awesome/

## Screenshots

![Screenshot1](http://www.cravesoft.com/stlviewer/images/screenshot1-small.png)

## Installation 

### Get the source

Clone the git repository:

``` bash
git clone git@github.com:cravesoft/stlviewer.git
```

### Install dependencies

For Ubuntu, simply enter the following command that will install all necessary packages:

``` bash
sudo apt-get install libqt4-dev libqt4-opengl-dev qt4-qmake
```

### Compile

``` bash
qmake && make
```

This will generate an executable called `stlviewer`.
