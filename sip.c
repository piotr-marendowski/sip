/* Author: Piotr Marendowski */
/* License: MIT */
/* Usage: sip [FILE] */
#include <Imlib2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BACKGROUND "#444444"

unsigned int imageheight, imagewidth;
unsigned int xcenter, ycenter;

/* Create window */
Window create_simple_window(Display *display, int width, int height, int x, int y);
/* Create gc */
GC create_gc(Display *display, Window window);
/* Set hints - program name etc. */
void set_hints(Display *display, Window window, char *filename);
/* Draw image */
void draw_image(Display *display, Window window, GC gc, Visual *visual, int depth,
            Colormap colormap, char *filename);
/* Set background color */
void set_background(char *rgb, Display *display, Window window);

int main(int argc, char *argv[])
{
    Display *display;
    int screen_num;
    Window window;
    unsigned int width, height;
    char *display_name = getenv("DISPLAY");
    GC gc;
    XEvent event;
    /* image stuff */
    Visual *visual;
    int depth;
    Colormap colormap;

    /* print help */
    if (argc != 2 || (strcmp(argv[1], "-h") == 0))
    {
        fprintf(stderr, "Usage: sip [FILE]\n");
        return 1;
    }

    char *filename = argv[1];
    /* check if image can be opened */
    FILE *fp;
    if (!(fp = fopen(filename, "r+")))
    {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        return 1;
    }

    /* open display */
    display = XOpenDisplay(display_name);
    if (display == NULL) {
        fprintf(stderr, "%s: cannot connect to X server '%s'\n", argv[0],
                display_name);
        return 1;
    }

    screen_num = DefaultScreen(display);
    width = DisplayWidth(display, screen_num);
    height = DisplayHeight(display, screen_num);
    xcenter = width / 2;
    ycenter = height / 2;

    window = create_simple_window(display, width, height, 0, 0);
    set_hints(display, window, filename);
    gc = create_gc(display, window);
    XSync(display, False);

    /* for images */
    visual = DefaultVisual(display, screen_num);
    depth = DefaultDepth(display, screen_num);
    colormap = DefaultColormap(display, screen_num);

    set_background(BACKGROUND, display, window);

    /* events */
    XSelectInput(display, window,
                 ExposureMask | KeyPressMask | ButtonPressMask |
                 StructureNotifyMask);

    while (1)
    {
        XNextEvent(display, &event);
        switch (event.type)
        {
            case Expose:
                draw_image(display, window, gc, visual, depth, colormap, filename);
                break;
            /* update the information for expose events */
            case ConfigureNotify:
                width = event.xconfigure.width;
                height = event.xconfigure.height;
                xcenter = width / 2;
                ycenter = height / 2;
                break;
            case KeyPress:
                if (XLookupKeysym(&event.xkey, 0) == XK_q)
                {
                    imlib_free_image();
                    XFreeGC(display, gc);
                    XCloseDisplay(display);
                    return 0;
                }
                break;
            default:
                break;
        }
    }

    XFreeGC(display, gc);
    XCloseDisplay(display);

    return 0;
}

Window create_simple_window(Display *display, int width, int height, int x, int y)
{
    int screen_num = DefaultScreen(display);
    Window window;

    window = XCreateSimpleWindow(
        display, RootWindow(display, screen_num), x, y, width, height, 0,
        WhitePixel(display, screen_num), BlackPixel(display, screen_num));

    XMapWindow(display, window);
    XFlush(display);

    return window;
}

GC create_gc(Display *display, Window window)
{
    GC gc;
    unsigned long valuemask = 0;
    XGCValues values;

    gc = XCreateGC(display, window, valuemask, &values);

    return gc;
}

void set_hints(Display *display, Window window, char *filename)
{
    XTextProperty window_name_property;
    XSizeHints *win_size_hints;
    XWMHints *win_hints;
    char str[100] = "sip ";
    char *window_name = strcat(str, filename);

    int rc = XStringListToTextProperty(&window_name, 1, &window_name_property);

    if (rc == 0) {
        fprintf(stderr, "XStringListToTextProperty - out of memory\n");
        exit(1);
    }

    XSetWMName(display, window, &window_name_property);

    win_size_hints = XAllocSizeHints();
    if (!win_size_hints) {
        fprintf(stderr, "XAllocSizeHints - out of memory\n");
        exit(1);
    }
    win_size_hints->flags = PSize | PMinSize;
    win_size_hints->min_width = 300;
    win_size_hints->min_height = 200;
    win_size_hints->base_width = 400;
    win_size_hints->base_height = 250;

    XSetWMNormalHints(display, window, win_size_hints);
    XFree(win_size_hints);

    win_hints = XAllocWMHints();
    if (!win_hints) {
        fprintf(stderr, "XAllocWMHints - out of memory\n");
        exit(1);
    }

    XSetWMHints(display, window, win_hints);

    XFree(win_hints);
}

void draw_image(Display *display, Window window, GC gc, Visual *visual, int depth,
          Colormap colormap, char *filename)
{
    Imlib_Image image;
    Pixmap pixmap;

    imlib_context_set_display(display);
    imlib_context_set_visual(visual);
    imlib_context_set_colormap(colormap);
    imlib_context_set_drawable(window);

    imlib_context_set_dither(1);
    imlib_context_set_display(display);
    imlib_context_set_visual(visual);

    image = imlib_load_image(filename);
    imlib_context_set_image(image);

    imagewidth = imlib_image_get_width();
    imageheight = imlib_image_get_height();

    pixmap = XCreatePixmap(display, window, imagewidth, imageheight, depth);
    imlib_context_set_drawable(pixmap);
    imlib_render_image_on_drawable_at_size(0, 0, imagewidth, imageheight);

    XCopyArea(display, pixmap, window, gc, 0, 0, imagewidth, imageheight,
              xcenter - (imagewidth / 2), ycenter - (imageheight / 2));
}

void set_background(char *rgb, Display *display, Window window)
{
    XColor color;
    Colormap colormap = DefaultColormap(display, DefaultScreen(display));
    XParseColor(display, colormap, rgb, &color);
    XAllocColor(display, colormap, &color);

    XSetWindowBackground(display, window, color.pixel);
}
