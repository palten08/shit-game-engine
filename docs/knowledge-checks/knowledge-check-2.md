Can you give me a quiz on everything I've done so far and based on the questions I've been asking?

● Quiz

  Answer as much or as little as you want. No trick questions.

  ---
  1. What happens to the data in memory when you call free() on a pointer?

  2. Why does the stack grow downward (from high addresses to low addresses) instead of upward?

  3. You have a 64-bit CPU. A push instruction always moves the stack pointer by how many bytes, regardless of the value
   being pushed?

  4. What is the difference between the address bus and the data bus?

  5. A process uses 500MB of RAM. Another process uses 300MB. Can they both have a pointer with the value 0x00400000?
  Why or why not?

  6. What does the -> operator do in C, and when do you use it instead of .?

  7. Why does field ordering matter inside a struct, but not the ordering of structs within a file (aside from
  definition order)?

  8. What's wrong with this code?

  int x = 10;
  int y = 20;
  x,y = y,x;

  9. You have a game loop that clears the screen, draws a square, and presents — but the square flickers. What's the
  most likely cause?

  10. Your game moves a character 5 pixels per frame. On a fast computer running at 120 FPS, the character moves _____
  pixels per second. On a slow computer at 30 FPS, it moves _____ pixels per second. How does delta time fix this?

  11. What's the difference between #include "types.h" and #include <SDL2/SDL.h>?

  12. In a union, how much memory is used if one member is 8 bytes and another is 12 bytes?

  13. Your Bresenham's line function has for (int x = x1; x <= x2; x++). Why does a line from (300, 200) to (200, 300)
  draw nothing?

  14. Why is it wasteful to call SDL_LockTexture and SDL_UnlockTexture inside a per-pixel drawing function?

  15. A CPU has 16 architectural general-purpose registers but ~224 physical registers. What are the extra physical
  registers used for and why?

❯ 1. The stack pointer is incremented by the size of whatever the thing you're freeing is, essentially "reclaiming"
   the stack space so something else can use the address that you just freed. The data will still be there until
  something else writes over it though, and there's no guarantee it will be your program. I'm also not sure if it's
   the stack pointer in this case or something else, is it the heap? Maybe I was right the first time

● You caught your own mistake mid-answer — free() is for heap memory (stuff you malloc'd), not the stack. The stack
  cleans itself up automatically when functions return.

  For the heap, there's no pointer like ESP that gets adjusted. The C runtime's memory allocator just marks that chunk
  as "available for reuse" in its internal bookkeeping. No pointer moves, no data is zeroed — the allocator just updates
   its free list.

  You got the important part right though: the data stays there until something overwrites it.

  Go ahead with the rest whenever you're ready.

❯ 2. I think it has something to do with architecture decisions made years ago by folks pioneering the industry,
  and the convention just stuck because nobody else offered up a different means of doing so? I don't think there's
   an actual scientific or technical reason, just "some company was the first to do it that way and everybody else
  did it too"

● Right that there's no deep technical reason and it could work either way. But there was a practical motivation — it
  wasn't purely arbitrary.

  The stack starts at the top of the address space and grows down, while the heap starts at the bottom and grows up.
  They grow toward each other, which means they can flexibly share the space between them without needing a hard
  boundary decided in advance. A program that uses lots of stack but little heap is fine, and vice versa.

  That's the design reason. The convention sticking across the industry — you're right about that part.

❯ 3. Pretty sure that'll be 8 bytes. 64 bit divided by 8 equals 8?

● Yep. 64 bits / 8 bits per byte = 8 bytes. Even if you push the value 1, it takes 8 bytes on the stack.

❯ 4. The address bus is what the CPU uses to tell the memory controller where it wants the data from or where it
  wants some data it has written to

● And the data bus?

❯ The data bus is where the CPU and memory controller will actually put the stuff it's giving to the other
  component

● Yep, exactly. Address bus says where, data bus carries what. And the control bus (which you didn't mention but we
  covered) says which operation — read or write.

❯ 5. I'm going to say yes because of virtual memory. That's just a "local" logical pointer to them, it's mapped to
  some actual physical memory address somewhere on the system

● Exactly right. Every process gets its own virtual address space, so 0x00400000 in process A and 0x00400000 in process
  B map to completely different physical RAM through the page table. Neither process knows or cares about the other.

❯ 6. I think it's dereferencing. Like, if I have a struct called AppContext and it has a member "resolution_x", and
   I've declared an instance of AppContext called app_context, I can access that member by doing
  app_context.resolution_x to get or set it. But if I want another function to be able to do something with
  app_context, I have to send it a pointer. And that function then needs to dereference the pointer to be able to
  get the actual struct. So the syntax changes to that -> arrow

● That's it. -> dereferences the pointer and accesses the member in one step. app_context->resolution_x is the same as
  (*app_context).resolution_x, just cleaner to read.

❯ 7. I think I only remember the answer to the ordering within the struct, it's because of alignment in memory for
  all of the members of the struct. The entire struct will have the alignment of the largest of its members, and if
   you have a struct with varied sizing of members you want to try to group them together. I don't think I quite
  remember why though. As for the ordering of structs in a file, I don't think it matters but I can't really
  remember why. I'm gonna guess it's because the compiler already allocates the memory for them based on size or
  something. That sounds super wrong and is an absolute guess so I won't be shocked if that's wrong

● The struct field ordering — you've got the right idea but the "why" is: the compiler inserts invisible padding bytes
  between fields to keep each one aligned to its natural boundary. If you put a char (1 byte) before a double (8 bytes),
   there's 7 bytes of wasted padding in between. Grouping by size minimizes that waste.

  The ordering of structs in a file — it's simpler than you think, and you actually ran into this exact bug earlier. The
   compiler reads top to bottom in one pass. If struct B uses struct A as a member, A must be defined above B in the
  file. Otherwise the compiler says "unknown type name." That's the only ordering constraint — it's not about memory
  allocation, it's about the compiler needing to have seen a type before you use it.

  That was your LineData/SquareData inside Scene error.

❯ 8. That's interpreted language syntax which won't work in C. I forget its actual behavior in C if you try to do
  that, I just know it won't work like how you think it is if you write that. You have to go the more verbose way
  and do temp variables instead

● Right. In C the comma is an operator that evaluates both sides but only keeps the rightmost value. So x,y = y,x
  evaluates x (discards it), then does y = y (nothing), then evaluates x (discards it). Nobody gets swapped. Temp
  variable is the way.

❯ 9. Sounds like single buffering or something. I don't see any explicit mentions to locking and unlocking, so that
   seems to me like you're attempting to directly modify the buffer that's being shown on screen

● Close — the most likely cause we actually ran into was that the drawing functions were each calling SDL_RenderPresent
  individually. So the screen updates multiple times per frame: once showing the cleared screen (black), once showing
  the square. The monitor catches both states alternating rapidly.

  The fix was to do all drawing into the buffer first, then present once at the end. That's double buffering — draw to a
   back buffer the player can't see, then flip it to the screen all at once.

❯ 10. I don't really know how to explain it in actual concrete technical terms, but pinning everything to delta
  time scales the results of things uniformally no matter how long it took between frames. If you're running at a
  higher frame rate, the result of something is smaller than if it were running at powerpoint slideshow framerate
  to give the illusion that uniform behavior is happening

● That's the right understanding. To fill in the concrete numbers:

  - 120 FPS × 5 pixels per frame = 600 pixels per second
  - 30 FPS × 5 pixels per frame = 150 pixels per second

  The character moves 4x faster on the fast machine. With delta time, you'd write something like position += 300 *
  delta_time instead of position += 5. Delta time is large on slow frames and small on fast frames, so the distance per
  second stays consistent regardless of frame rate.

❯ 11. The double quote notation is for local files, the angle bracket notation is for something installed in the
  system library path

● Yep. Double quotes check the local directory first, then fall back to system paths. Angle brackets skip local and go
  straight to system paths.

❯ 12. I haven't a clue so I'm going to guess that, due to context clues behind what "union" means in English, it
  would actually take up 20 bytes

● The opposite direction — a union's size is its largest member, so 12 bytes. All members share the same memory,
  overlapping at the same starting address. That's the whole point of a union — two different views of the same chunk of
   memory, not two separate allocations added together.

❯ 13. My earliest implementation of the algorithm was incapable of handling anything that didn't go left-to-right.
  For reasons I'm still unclear about as I definitely need a more thorough breakdown of how Bresenham's works later

● Right. The specific reason is that the loop condition x <= x2 is false immediately when x1 (300) is already greater
  than x2 (200), so the loop body never executes. The line is right-to-left but the loop only steps left-to-right.

  We can do a deeper Bresenham's walkthrough whenever you want.

❯ 14. If a function does lock and unlock, and it's paired with any other number of functions that modify the frame
  buffer and also lock and unlock the texture, the performance cost is substantial and immediately noticeable in
  the frame rate

● Right. Lock once, do all your drawing, unlock once. Not lock/unlock per pixel or per shape. Your line drawing function
   was doing it 800 times for a single diagonal line.

❯ 15. I think some of them are divided out to the other logical units like the VPU and SSE units. As for the
  remainder, I couldn't tell ya

● The ~224 physical registers are all in the integer/general-purpose unit — they're used for register renaming. When
  multiple instructions write to the same architectural register (like RAX), the CPU assigns each write to a different
  physical register behind the scenes so the instructions can execute in parallel without conflicting. The Register
  Alias Table maps the 16 names the programmer sees to the ~224 physical registers, allocating and freeing them as
  instructions flow through the pipeline.

  The FPU, SSE, and AVX units have their own separate physical register files for the same reason.

  ---
  Overall you did well — the core concepts are sticking. The areas I'd revisit:

  - Unions — remember they overlay, not combine. Same memory, different views.
  - Bresenham's — worth a proper walkthrough so the math clicks, not just the fix
  - Struct alignment — you know the "what" but the "why" (padding bytes between misaligned fields) could be sharper

  Everything else was solid or close enough that you self-corrected. The heap/stack distinction on question 1 was a good
   catch.