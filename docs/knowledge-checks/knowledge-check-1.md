I asked Claude to quiz me on stuff I had been asking it already. I'm just assuming it gave me a quiz that isn't bullshit.
I'll probably have it do this a few more times here and there.

-----------------------------------------

Part 1: C Fundamentals

  1. What's the difference between these two declarations?
  int x = 5;
  int *p = &x;
  What does *p = 10; do to x?

  2. What's wrong with this code?
  int *data = malloc(100 * sizeof(int));
  data[50] = 42;
  // function ends here

  3. You have uint32_t *buffer = malloc(800 * 600 * sizeof(uint32_t));. How would you access the pixel at column 200,
  row 150?

  4. What does (uint32_t) do in (uint32_t)some_value? What's the general term for this?

  5. Why does 7 / 2 give you 3 in C, and what do you do if you want 3.5?

  ---
  Part 2: Build Process

  6. In your own words, what's the difference between what the compiler does and what the linker does?

  7. You add a new library called libfoo to your project. You've included its header with #include <foo.h> and the code
  compiles, but you get undefined reference to 'foo_init'. What did you forget?

  8. Why do you never need to write -lc in your Makefile?

  ---
  Part 3: SDL + Rendering

  9. Put these in the correct order for displaying a frame:
  - SDL_RenderPresent
  - SDL_LockTexture
  - Write pixels to the framebuffer
  - SDL_UnlockTexture
  - SDL_RenderCopy

  10. What is double buffering, and what visual problem does it prevent?

  11. What does SDL_PollEvent return when there are no events left in the queue?

  12. Why do you need both SDL_UnlockTexture AND SDL_RenderCopy? They sound like they'd do the same thing — what does
  each one actually do?

  ---
  Part 4: Pixel Format + Memory

  13. What color is 0xFF00FF00 in ARGB8888 format?

  14. What does | 0xFF000000 do to a value, and why would you use it on a color?

  15. Your 800x600 framebuffer is a flat array. What is the index of the very last pixel (bottom-right corner), and why
  is it that number and not 480,000?

  ---
  Part 5: Intuition Check (upcoming stuff)

  Don't worry about getting these right — just say what your gut tells you.

  16. You need to draw a diagonal line from point (100, 100) to point (400, 300) on your framebuffer. Why can't you just
   loop from x=100 to x=400 and set one pixel per column?

  17. You have a 3D cube defined as 8 vertices in space. What do you think needs to happen to those 3D coordinates
  before you can draw anything on your 2D framebuffer?

  18. Two triangles overlap on screen. Without a z-buffer, what do you think determines which one appears on top?

  19. In your game loop, you move the camera 5 units per frame. On a fast computer running at 300fps vs a slow one at
  30fps, what happens to movement speed, and how would you fix it?

  20. You want to know if a surface is facing toward or away from the camera. What mathematical concept between two
  vectors do you think could help determine that?

  ---
  Take your time, answer what you can, skip what you want. No trick questions.

❯ 1. The first is a plain variable declaration, setting a variable of type int named x equal to 5. The second is a
  pointer to an int, and it's named p and is set to the address of x. *p = 10 will make x = 10. Right?

● Right on all counts. Moving on whenever you're ready.

❯ 2. I think is an out of bounds access, but I'm not really sure why exactly given that I'm fairly certain the
  array is being allocated 200 bytes which seems like an index of 50 would still fall inside of?

● The index of 50 is fine — you allocated space for 100 ints, so indices 0–99 are all valid. The problem is you never
  called free(data). The memory is allocated but never released — that's a memory leak. The function ends, the pointer
  data is gone, but the 400 bytes are still reserved with no way to reclaim them.

  On to 3 whenever you're ready.

❯ 3. I think you would do buffer[150 * 800 + 200]

● Correct. Row 150, skip 800 pixels per row, add column 200. Next one.

❯ 4. Fairly certain that's typecasting. You're attempting to type cast whatever some_value is into a uint32_t

● Correct. Next.

❯ 5. Fairly certain division in every single programming language doesn't give you a remainder, so you'd have to
  use modulo (or however it's spelled). As to why, I'm not sure I know

● Close but not quite. Modulo (%) gives you the remainder — 7 % 2 would give you 1.

  The issue is that 7 / 2 gives you 3 because both operands are integers, and integer division in C truncates the
  decimal part. It doesn't round — it chops it off.

  To get 3.5, you'd cast one of them to a floating point type: (double)7 / 2 or 7.0 / 2. As long as one side is a
  float/double, C does floating-point division and keeps the fractional part. This is exactly what came up with your
  delta time calculation earlier.

  On to 6.

❯ 6. The compiler just takes your .c file (or files) and turns into an object file which I think is just bytecode.
  The linker resolves any external references to libraries and functions you used

● Good enough. One small clarification — object files are machine code (native CPU instructions), not bytecode. Bytecode
   is an intermediate format used by languages like Java or Python that runs on a virtual machine. The C compiler
  produces actual instructions your CPU executes directly.

  But the core idea is right: compiler translates your code, linker wires up the references. On to 7.

❯ 7. Seems like you never linked the library, right?

● Yep. Missing -lfoo in the linker flags. On to 8.

❯ 8. Pretty sure -lc is for the standard C library which is in /usr/include (or something) and is always included
  because basically everything needs it

● Right. GCC links it automatically every time — it's the one library you never have to specify. The headers are in
  /usr/include, the library itself (libc.so) is in /usr/lib or similar. On to 9.

❯ 9. SDL_LockTexture, Write pixels to the framebuffer, SDL_UnlockTexture, SDL_RenderCopy, SDL_RenderPresent

● Perfect. On to 10.

❯ 10. Double buffering is where one buffer is actively being scanned by the monitor while the other buffer is being
   written to. It's supposed to alleviate artifacts like tearing and flickering, triple buffering even more so
  apparently

● Exactly right. On to 11.

❯ 11. I think it returns false

● Correct — returns 0 (false), which is how the inner while loop knows to stop polling and move on to updating and
  rendering. On to 12.

❯ 12. UnlockTexture writes the changes you made to it while it was locked to the texture itself, and I think
  RenderCopy writes the texture to the back buffer?

● Spot on. On to 13.

❯ 33. I think that's fully opaque green

● Right. FF alpha, 00 red, FF green, 00 blue. On to 14.

❯ 14. I think it's a bit wise OR, which you can use to basically combine the full alpha bytes from 0xFF000000 with
  the remaining 6 bytes from another 32 bit hexadecimal value

● Right idea. Small terminology note — it's the remaining 6 hex digits (3 bytes / 24 bits), not 6 bytes. But the concept
   is correct: OR the alpha in without touching the color channels. On to 15.

❯ 15. I believe it's 479,999 because everything in computer programming is 0 indexed

● Correct. 480,000 pixels, indices 0–479,999. On to the intuition questions — just say what your gut tells you, no wrong
   answers here.

❯ 16. I'm pretty certain going linearly from 100 to 400 will create a straight line across the same row you started
   on, not a diagonal one that goes down to the right where it should

● Right. You'd be setting one pixel per column but never changing the row. The line needs to also move downward from
  y=100 to y=300 as it goes across, and it won't step down evenly — some columns might need to stay on the same row for
  a step before dropping. That's the problem line-drawing algorithms like Bresenham's solve. On to 17.

❯ 17. I think they would have to be converted to screen-space coordinates? Because I'm making an assumption that
  the coordinates they're defined as are ones that represent the 8 points in 3 dimensional space like the real
  world. We have to approximate 3D on a flat 2D screen so actual 3D coordinates are not useful to us

● Exactly right. That transformation from 3D world coordinates to 2D screen coordinates is the projection step — and the
   key part is perspective division, where things farther from the camera get smaller. That's what makes it look 3D on a
   flat screen. On to 18.

❯ 18. I'm going to guess that it's whichever one was the last one to be drawn?

● Exactly. Without a z-buffer, whatever gets drawn last just overwrites whatever was there before — it's painter's
  order. The z-buffer fixes this by checking depth at each pixel so it doesn't matter what order you draw them in. On to
   19.

❯ 19. The camera's movement would vary on the two computers because of the frame rate. So you would have to clamp
  movement like that by tieing it to the delta time since the delta time represents how much time has elapsed
  between frames. So if you run the game at 4000 frames per second or 4, the camera is moving the same amount
  (although the camera movement at 4 FPS is going to be extremely unresponsive compared to the higher framerate)

● Nailed it. Multiply movement by delta time so it's distance-per-second instead of distance-per-frame. And you're right
   that low FPS still feels terrible even though the speed is correct — the movement is the same over time but the
  updates are so infrequent it feels choppy. Last one.

❯ 20. I think that's called a normal?

● A normal is the vector that represents which way the surface is facing, yes. The operation you'd use to compare the
  surface normal against the camera direction is the dot product — it tells you how much two vectors point in the same
  direction. If the result is positive they're roughly facing the same way, if it's negative they're facing each other.
  That's how you'd determine if a surface is facing toward or away from the camera.

  ---
  You did really well on this. The C fundamentals and SDL rendering pipeline are solid. The intuition questions about 3D
   stuff were all on the right track — you clearly have a sense for what's coming even before researching it. The only
  gaps were minor terminology things (bytes vs hex digits, bytecode vs machine code). You're in a good spot to start
  tackling line drawing