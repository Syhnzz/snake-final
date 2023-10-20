

# A simple Snake game in C and SDL 1.2

Here is a simple implementation of the "Snake" game, written C and SDL 1.2.

A [ring buffer](https://en.wikipedia.org/wiki/Circular_buffer) data structure is used
which allows for efficiently moving the snake.

The core code structure of `init()`, `update()`, `draw()` is modelled after [PICO-8](https://www.lexaloffle.com/pico-8.php).


![Screenshot_2020-09-07_12-19-33](https://user-images.githubusercontent.com/223396/92408921-8ca4df80-f104-11ea-9e6f-15f74557f4ab.png)

Commande pour compiler mingw32-make -f Makefile