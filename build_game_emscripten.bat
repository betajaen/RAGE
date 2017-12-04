emcc main.c collision.c draw.c object.c level.c -g -O2 -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 --embed-file assets --shell-file emscriptem_shell.html -o bin/game.html
