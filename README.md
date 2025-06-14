# OptrixOS
A new OS that revolutionizes compatibility and can run cross platform apps such as exe and Linux based Debian based Distrubutions but has its own file system

## Ring Buffers + Separation
The PS/2 controller delivers bytes for both the keyboard and mouse on the same data port.
OptrixOS now uses ring buffers to keep these streams separated. Keyboard bytes are
queued into a keyboard buffer while mouse bytes are grouped into packets and stored
in a mouse buffer. Calling `ps2_flush_buffers()` frequently pulls all pending bytes from
the controller and routes them to the correct handler so the wrong code never sees
the wrong data.
