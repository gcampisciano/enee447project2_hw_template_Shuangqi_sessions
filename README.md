# ENEE447 Project 2


## Environment setup
- SD card: format it to be FAT32 if you haven't done so
- 32-bit cross-compiler:
	follow the setup in [project 1](https://github.com/sklaw/enee447project1_hw_template) if you haven't done so
    
## You're supposed to modify the following files
- [callout.c](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/callout.c#L53-L91)

## One-line synopsis of this project
Implement the 3 functions in [callout.c](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/callout.c#L53-L91) so that [kernel.c](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/kernel.c#L47-L82) will act like an operating system calling functions at specified time.
- Optional extra credits: read [p2.pdf](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/p2.pdf) for details.

## Recommended Readings
- [list.h](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/llist.h): so you know how to use linked list for this project. You can also find some usage of the list library [here in callout.c](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/callout.c#L33-L50)
- [log.h](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/log.h): you can find some debug/logging utilities that you may need when you debug your program on board. Some usages of debug/logging can be found in [kernel.c](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/kernel.c)
- [p2.pdf](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/p2.pdf)

## How to test your solution
- Compile this code with `make` command, copy the files (not the folder) in `things_to_copy_to_your_sd_card` to your SD card
- Setup UART communication like you did for [project 1](https://github.com/sklaw/enee447project1_hw_template) (so you can receive debug messages sent from the board)
- Boot your board, if the UART messages indicate the events are happenning in accordance with `create_timeoutq_event` calls in [kernel.c](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/kernel.c#L48-L62), then your solution is good! 
- Your solution's output should be something similar to these:
    - ![](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/images_used_by_README/minicom1.png)
    - ![](https://github.com/sklaw/enee447project2_hw_template_Shuangqi_sessions/blob/master/images_used_by_README/minicom2.png)


## Concepts to Know (Written by Grant Hoover)

**Problem:** We want programs (or simple routines/functions) to run at specific times and, sometimes, at regular intervals.

Reasons for wanting this:
- Checking for I/O at regular intervals
- Possibly calling kernel routines for program scheduling (context switching)
- Watchdog timers – ensure programs are not taking too long
- Allow a function to sleep for set intervals (e.g. audio sampling)

Why doesn’t our previous (Project 1) *while()* loop work?
- What happens if a program halts (blocking)?
- I/O takes *a lot* of time. Nothing can happen while we wait for it to be ready.

Possible solutions:
1. Spawn a new thread for each sequence. New problem: There are a limited number of cores so you can still get blocking. It’s also very difficult to share a timer between threads as they are spawning and dying. We need a central timekeeper.
2. Use a dynamic list of tasks to be run and times at which to run them.
   This is called a “callout table” in UNIX or a “timeout queue” generally.

### Timeout Queue Implementation

We can use a linked list to keep track of these tasks.

What goes into each node?
- Time to run/execute
- How many repeats?
- How long (time) between repeats
- Function pointer
- Function data/arguments
- Pointer to the next node

Example:

Tasks are added to the timeout queue in the following order:
1. Run X(5) at time = 3
2. Run Y(A) at time = 10
3. Run Z(0.275) at time = 4

The timeout queue looks like the following after each addition:

1. &#43;--------&#43;  
&nbsp;&#124; time: 3 &#124;  
&nbsp;&#124; func: X &#124;  
&nbsp;&#124; data: 5 &#124;  
&#43;--------&#43;

2. &#43;--------&#43;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#43;--------&#43;       
&nbsp;&#124; time: 3 &#124;----->&#124; time: 7 &#124;  
&nbsp;&#124; func: X &#124;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#124; func: Y &#124;  
&nbsp;&#124; data: 5 &#124;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#124; data: A &#124;  
&#43;--------&#43;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#43;--------&#43;

2. &#43;--------&#43;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#43;-------------&#43;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#43;--------&#43;       
&nbsp;&#124; time: 3 &#124;----->&#124; time: 1 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#124;----->&#124; time: 6 &#124;  
&nbsp;&#124; func: X &#124;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#124; func: Z &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#124;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#124; func: Y &#124;  
&nbsp;&#124; data: 5 &#124;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#124; data: 0.275 &nbsp;&#124;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#124; data: A &#124;  
&#43;--------&#43;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#43;-------------&#43;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#43;--------&#43;

Why insert Z before Y in the list?

This way, we know that everything past Z in the list will execute after Z. If we keep *relative* times between successive nodes, we don’t have to traverse the entire list when checking if a node is ready to execute. If the HEAD is not ready (its time value doesn’t equal 0), none of the nodes are ready.

Another reason for using relative times: When we check the time, we only need to update the time value of the HEAD to reflect the amount of time *until* it should run.


### Functions

`int bring_timeoutq_current()`
   
* Updates the time value of the HEAD node based on the time that has passed since the last check (we keep this previous time in a global variable called then_usec), updates then_usec, and returns the amount of time left until the HEAD should execute.  

`void create_timeoutq_event(int timeout, int repeat, pfv_t function, namenum_t data)`
  
* Creates a new node and inserts it into the timeout queue.  

`int handle_timeout_event()`
  
* Checks if HEAD time value is less than some minimum time (ready to be executed), updates the time value of the second node if there is one, removes the HEAD from the list, runs its function, and puts it back into the queue if the task needs repeated (otherwise add the node back to the “freelist”). Returns 1 if an event is handled.  

Kernel *while()* loop now checks the amount of time until the next event should run, waits for that time, and then handles the next event.

### Important Notes

Function pointers take event pointers as arguments.

**Remember to check for empty lists and null pointers!**






