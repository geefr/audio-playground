#Audio Visualiser
This is a basic audio visualiser, using shadertoy to provide visuals

Executable can be passed a filename, and will play that as the sole song, on repeat
If no filename provided will load the contents of the installation's music directory

Shaders will be loaded from the installatin's shader directory, will be displayed based on user's selection

Key bindings:
* s - Skip track
* x - Skip shader
* F11 - Toggle Fullscreen
* F12 - Toggle UI visibility
* ESC - Quit

Supported audio formats:
* mp3
* wav

Runtime dependencies:
* OpenGL 3.3 or higher
* glfw3 (Not packaged on Linux)

