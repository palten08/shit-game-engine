 ---
  1. Dev Environment + Build Setup

  - Install a C compiler (MinGW/GCC or MSVC on your Windows machine)
  - Compile and run a "hello world" from the command line — understand what the compiler is actually doing
  - Download SDL2 development libraries for your platform
  - Figure out the compiler/linker flags needed to compile a program that includes SDL2 headers and links against the
  SDL2 library
  - Write a Makefile (or batch script, or CMakeLists.txt) so you aren't retyping the compile command every time
  - Get a program that just calls SDL_Init() and SDL_Quit() to compile and run without crashing — this proves your build
   is wired up correctly

  ---
  2. Window + Game Loop

  - Create an SDL window and an SDL renderer/texture that you can push pixels to
  - Implement the core loop structure: poll events, update, render, present
  - Handle the SDL_QUIT event so closing the window actually exits
  - Clear the screen to a solid color each frame to prove your pixel-pushing path works
  - Measure time between frames using SDL_GetTicks() or SDL_GetPerformanceCounter()
  - Calculate delta time in seconds and cap or track your frame rate
  - Add keyboard state polling (SDL_GetKeyboardState) — print something to the console when a key is held, just to
  verify input works

  ---
  3. Framebuffer + Pixel Drawing

  - Allocate a uint32_t array to serve as your framebuffer (width * height)
  - Write a put_pixel(x, y, color) function that sets a value in the buffer with bounds checking
  - Write a clear_buffer(color) function that fills the whole framebuffer
  - Figure out how to upload your framebuffer into an SDL texture and present it each frame
  - Understand how pixel color is packed into a 32-bit integer (ARGB or RGBA byte ordering)
  - Draw some test pixels at known positions to confirm coordinates work the way you think (is 0,0 top-left or
  bottom-left? which way does Y go?)

  ---
  4. Line Drawing

  - Research Bresenham's line algorithm (or DDA — either works)
  - Implement draw_line(x0, y0, x1, y1, color)
  - Test with horizontal, vertical, and diagonal lines
  - Test with lines that go in all directions (left-to-right, right-to-left, steep, shallow) — most first
  implementations only work for one quadrant
  - Draw a 2D wireframe rectangle by calling draw_line four times
  - Draw a 2D grid or crosshair to build confidence

  ---
  5. Math Foundation

  - Define a vec3 struct (x, y, z as floats)
  - Implement: vec3 add, subtract, scale, dot product, cross product, length, normalize
  - Define a mat4 struct (a 4x4 float array)
  - Implement: mat4 identity, mat4 multiply (matrix * matrix), mat4 * vec4 (you'll need to treat vec3 as vec4 with w=1
  for positions)
  - Implement a rotation matrix constructor for each axis (rotation around X, Y, Z by an angle)
  - Implement a translation matrix constructor
  - Implement a scaling matrix constructor
  - Test these by transforming known points and checking the results manually on paper — do not skip this, bugs here
  will haunt you for weeks

  ---
  6. 3D Projection (Wireframe)

  - Define a perspective projection matrix (research what field of view, aspect ratio, near plane, and far plane mean)
  - Hardcode a cube as 8 vertices (vec3 array) and 12 edges (pairs of vertex indices)
  - Each frame: multiply each vertex by a rotation matrix (increment the angle each frame to make it spin)
  - Then multiply by the projection matrix
  - Perform perspective division (divide x and y by z/w) to get 2D screen coordinates
  - Map the resulting -1 to 1 range to actual pixel coordinates
  - Draw lines between connected vertices using your line function
  - You should see a spinning wireframe cube — if it looks wrong, it's almost certainly a matrix bug

  ---
  7. Triangle Rasterization

  - Define a triangle as three 2D screen-space vertices (after projection)
  - Research and implement a triangle fill algorithm — either scanline or edge function (barycentric) approach
  - Scanline approach: sort vertices by Y, walk down edges, fill horizontal spans
  - Edge function approach: find the bounding box of the triangle, test each pixel to see if it's inside all three edges
  - Fill your spinning cube's faces as solid-color triangles instead of wireframe edges (a cube has 6 faces, each face
  is 2 triangles = 12 triangles total)
  - You'll immediately notice the problem: back faces are visible and faces draw in the wrong order

  ---
  8. Depth Buffering

  - Allocate a z-buffer — a float array the same size as your framebuffer, initialized to a far value each frame
  - During triangle rasterization, interpolate the depth (z) value across the triangle for each pixel
  - Before writing a pixel, check if its depth is closer than what's already in the z-buffer
  - Only write the pixel (and update the z-buffer) if it's closer
  - Your solid cube should now look correct from any angle — no more back faces showing through
  - Implement backface culling as well: compute the triangle's normal via cross product, skip it if it faces away from
  the camera. This is an optimization but also helps you understand surface orientation.

  ---
  9. Camera + Movement

  - Define a camera struct: position (vec3), yaw angle, pitch angle
  - Derive the forward vector from yaw and pitch using sin/cos (research Euler angles)
  - Derive the right vector as the cross product of forward and world-up (0,1,0)
  - Build a view matrix from the camera (research "look-at matrix" — it's constructed from position + the three camera
  axes)
  - Insert the view matrix into your transform pipeline: model → view → projection
  - WASD input: each frame, check which keys are held, add forward * speed * deltaTime or right * speed * deltaTime to
  the camera position
  - Mouse input: capture the mouse with SDL, read relative mouse motion each frame, add to yaw/pitch
  - Clamp pitch so you can't look past straight up or straight down (otherwise the math breaks)
  - You should now be able to fly around your cube with WASD + mouse

  ---
  10. Build a Simple Environment

  - Replace the single cube with a scene: a large flat quad for the floor (2 triangles), some walls, some scattered
  cubes or pillars
  - Define each object with its own model transform (position/rotation/scale in the world)
  - Organize this: a mesh struct (array of vertices + array of triangle indices) and an object/entity struct (mesh
  pointer + transform)
  - Loop over all objects each frame, transform and rasterize each one
  - Give different objects different colors so you can distinguish them
  - Tweak the floor size, wall placement, object count until you have a space that feels like a room or corridor

  ---
  11. Lighting

  - Define a directional light as just a vec3 direction (like "the sun is over there")
  - For each triangle, compute its surface normal using the cross product of two edges
  - Compute the dot product of the surface normal and the light direction
  - Use that value to darken or brighten the triangle's base color (clamp to 0–1 range so negatives don't wrap)
  - This is flat shading — one brightness per triangle. Your scene should now look dramatically more 3D.
  - Optional next step: Gouraud shading — compute lighting per-vertex instead of per-triangle, interpolate brightness
  across the surface for smooth gradients. This requires computing vertex normals by averaging the normals of adjacent
  faces.

  ---
  12. Texture Mapping (optional but rewarding)

  - Load a bitmap image into memory (BMP is the easiest format to parse by hand — the header is well-documented and
  pixels are stored raw)
  - Assign UV coordinates (0.0–1.0 range) to each vertex of your triangles
  - During rasterization, interpolate U and V across the triangle the same way you interpolate depth
  - Use the interpolated UV to sample a color from the texture image instead of using a flat color
  - You'll notice textures look warped — this is because you need perspective-correct interpolation (divide UVs by
  depth, interpolate, then multiply back). This is a great thing to research once you see the bug.

  ---
  General Tips For the Whole Journey

  - Keep old milestones as git branches or copies — you'll want to go back and look at simpler versions when something
  breaks
  - Printf debug aggressively — print vertex positions after each transform stage, print pixel counts, print matrix
  values
  - Render intermediate results — when debugging projection, draw the projected 2D points as dots before trying to
  connect them as triangles
  - Test math functions in isolation — write a separate small program that just exercises your vec3/mat4 code with known
   inputs and expected outputs
  - Read about what Doom, Quake, and early software renderers did — they solved these exact problems in C under much
  harder constraints. Michael Abrash's writings and Fabien Sanglard's book breakdowns are great non-tutorial reading.

















#################################################################################
#-------------------------------------------------------------------------------#
# Addendum for attemping more of this myself by removing SDL from the equation: #
#-------------------------------------------------------------------------------#
#################################################################################

  Everything from stages 4 onward (line drawing, math, rasterization, z-buffer, camera, lighting, textures) stays
  identical — that's all your code running on your framebuffer regardless. What changes is everything beneath it: how
  you get a window, how you display pixels, and how you receive input. That's stages 1–3 being replaced with a much
  bigger chunk of work.

  Here's what the no-SDL path looks like on Linux. You'd be talking directly to X11 (the windowing system).

  ---
  1. Dev Environment + Build Setup

  - Install X11 development headers: sudo apt install libx11-dev
  - Learn the compiler/linker flags for X11: -lX11 at minimum
  - Update your Makefile with the new flags
  - This is the one external dependency you can't avoid on Linux — X11 is how every program talks to the display server.
   SDL is a wrapper around this same thing.

  ---
  2. X11 Window Creation

  - Learn what a Display connection is — XOpenDisplay() opens a connection to the X server, which is the process that
  actually controls your screen
  - Understand the concept of a screen and a root window — X11 has a hierarchy of windows, yours will be a child of the
  root
  - Create a window with XCreateSimpleWindow() — specify position, size, border, colors
  - Map the window with XMapWindow() — creating it doesn't make it visible, mapping does
  - Set WM_DELETE_WINDOW protocol — without this, clicking the X button does nothing because X11 doesn't give you close
  events by default. You have to explicitly register for them using XInternAtom and XSetWMProtocols. This is the kind of
   thing SDL hides from you.
  - Give the window a title with XStoreName()
  - Call XFlush() or XSync() to make sure your commands actually get sent to the X server — X11 buffers commands

  ---
  3. X11 Event Loop

  - Tell X11 which events you care about using XSelectInput() with an event mask — you opt into each event type
  individually (key press, key release, pointer motion, exposure, structure notify, etc.)
  - Poll or block for events with XPending() and XNextEvent()
  - Handle Expose events — X11 sends these when your window needs to be redrawn (was covered by another window, was
  minimized, etc.). You must handle these or your window will be blank.
  - Handle ClientMessage events — this is how the WM_DELETE_WINDOW close button comes through. You compare the event
  data against the atom you registered.
  - Handle KeyPress and KeyRelease — X11 gives you keycodes (hardware scan codes), which you convert to keysyms (logical
   key identities) using XLookupKeysym(). This is two layers of indirection compared to SDL just giving you SDLK_ESCAPE.
  - Handle MotionNotify for mouse movement — gives you x,y coordinates relative to the window
  - Handle ButtonPress and ButtonRelease for mouse clicks
  - Understand the difference between event-driven (blocking on XNextEvent) and polling (checking XPending in a game
  loop). You want polling.

  ---
  4. Getting Pixels On Screen (the hard part)

  This is where the real pain lives. X11 gives you several options, all more manual than SDL's texture upload.

  Option A: XImage (software, simpler)

  - Allocate your framebuffer as a uint32_t array as before
  - Create an XImage struct using XCreateImage() — this wraps your pixel buffer in a format X11 understands
  - Understand pixel format negotiation — your display might be 24-bit RGB, 32-bit ARGB, or something else. Query the
  visual/depth of your window and match your buffer format to it. Getting this wrong means corrupted colors.
  - Each frame, call XPutImage() to copy your framebuffer into the window
  - This is slow because it copies pixel data over the X11 connection every frame, but it works and it's straightforward

  Option B: XShm (shared memory, faster)

  - Learn about MIT-SHM extension — instead of copying pixels over the socket to the X server, you allocate a shared
  memory segment that both your process and the X server can access
  - Install: sudo apt install libxext-dev
  - Create a shared memory segment with shmget() and shmat()
  - Create an XImage backed by shared memory using XShmCreateImage()
  - Attach the segment to the X server with XShmAttach()
  - Each frame, write to the shared buffer and call XShmPutImage() — much faster because no copy
  - Handle cleanup: XShmDetach, shmdt, shmctl to free the shared memory
  - This is the closest equivalent to what SDL does internally on X11

  Either way, understand:

  - Byte ordering — X11 might want pixels as BGRA while you're writing RGBA. Check XImage->byte_order and
  XImage->bits_per_pixel.
  - Stride/padding — rows of pixels might need to be padded to a certain alignment. XImage->bytes_per_line tells you the
   actual row width in bytes, which might differ from width * 4.
  - Flushing — after XPutImage, call XFlush() or nothing appears

  ---
  5. Mouse Capture + Relative Motion

  - For a game camera, you need relative mouse motion (how far the mouse moved this frame), not absolute position
  - X11 gives you absolute coordinates by default. You need to manually compute deltas: store last position, subtract
  from current position.
  - Warp the cursor back to center each frame with XWarpPointer() so the mouse doesn't hit the screen edge and stop
  generating motion
  - Hide the cursor — create a blank cursor with XCreatePixmapCursor() using an empty pixmap and set it on your window
  with XDefineCursor(). There's no "hide cursor" function — you make an invisible one.
  - Grab the pointer with XGrabPointer() if you want to confine it to your window — this is how you prevent the mouse
  from clicking outside your game

  ---
  6. Keyboard State Tracking

  - X11 gives you events (key went down, key went up), not continuous state
  - For a game you need to know "is W held right now," not "W was pressed at some point"
  - Build a key state array: on KeyPress set keys[keycode] = 1, on KeyRelease set keys[keycode] = 0
  - Query this array in your update step to determine movement
  - Beware key autorepeat — X11 generates repeated KeyPress/KeyRelease pairs when a key is held. You need to detect and
  filter these or your key state will flicker. XAutoRepeatOff() is the nuclear option; detecting repeat events by
  comparing timestamps is the proper one.

  ---
  7. Frame Timing Without SDL

  - No SDL_GetTicks() — use clock_gettime(CLOCK_MONOTONIC) from <time.h>
  - Compute delta time yourself from the difference between two timespec structs (seconds + nanoseconds, be careful with
   the arithmetic)
  - Implement frame rate limiting with nanosleep() if you want to cap FPS
  - No SDL_GetPerformanceCounter equivalent — clock_gettime with CLOCK_MONOTONIC is the standard way

  ---
  8. Clean Shutdown

  - Destroy the XImage (and shared memory segment if using XShm)
  - Destroy the window with XDestroyWindow()
  - Close the display connection with XCloseDisplay()
  - Same reverse-order principle as before, just more things to clean up

  ---
  What you gain by doing this

  - Deep understanding of how windowing actually works on Linux — every GUI toolkit and game library sits on top of X11
  (or Wayland)
  - Experience with a real, old, battle-scarred C API — X11 is from 1987 and it shows. It's verbose, quirky, and teaches
   you to read documentation carefully
  - Shared memory, pixel formats, byte ordering — low-level systems concepts that matter in graphics and systems
  programming
  - Appreciation for why abstraction layers like SDL exist

  What it costs

  - Stages 2–6 above are roughly equivalent to what SDL gives you for free in a few function calls
  - It's Linux-only. Porting to Windows later means learning the Win32 API, which is a completely different set of APIs
  for the same concepts. SDL gives you portability for free.
  - Debugging X11 issues is painful — errors are often asynchronous and cryptic
  - You'll spend significant time fighting the platform layer before you get to the interesting part (the actual 3D
  engine)

  ---
  My honest take: the 3D math, rasterization, and rendering pipeline are the interesting learning. X11 is more of an
  endurance test in reading documentation for an old API. But if your goal is to truly understand what happens all the
  way down, it's a worthwhile detour. You could also build with SDL first, get the engine working, and then rip SDL out
  and replace it with X11 as a separate challenge — that way you know the rendering code works and can isolate platform
  bugs.