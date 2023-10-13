# Simple Image Previewer
Simple Image Previewer written in C, using [imlib2](https://docs.enlightenment.org/api/imlib2/html/) to handle images, and [Xlib](https://x.org/releases/current/doc/libX11/libX11/libX11.html) as GUI interface.

<img src="screen1.png" alt="screen1" width="600"/>

## Limitations
It can only open one image at a time and always centers it. It does not have any editing capabilities or the ability to move the image around.

## Compile
```
make
```
Needs root privileges to make an executable in `/usr/local/bin`.

## Usage
```
sip [FILE]
```

## MIT License
