# SNAKES

Welcome to the project **SNAKES** – simple and fun game for two players based on MZ_APO board.

---

## Compilation and launch

The best way to compile and launch the project is by using provided Makefile.

Commands for compilation and launch:
- Compile the project: `make all`
- Compile the project and run: `make run`
- Clean machine and binary code files: `make clean`

Installation is not needed.

## Hints
Before running the project don't forget to change IP address of your board in the Makefile on the line 17.

Be advised that by default program uses HX8357-C display. If you have HX8357_B or ILI9481 display, uncomment corresponding macro in mzapo_parlcd.c

## Working scheme

1. User launches program (`snakes_manager.c`)
2. `snakes_manager.c` initializes utils modules (`display_utils.c`, `game_utils.c`, `hardware_utils.c`, `gui_utils.c`)
3. `cosnts.h` keeps information about consts.
4. `gameobjects.h` keeps information about objects in the game
5. `snake.c` is responsible for controlling snakes. It updates information about snakes using `hardware_utils.c`
6. `hardware_utils.c` is responsible for the interaction with hardware. It updates state of the hardware and reads current state of the knobs
7. `gui_utils.c` is responsible for managing gui and it updates interface
8. `game_utils.c` is responsible for managing the game. It updates state of the game
9. `display_utils.c` is responsible for drawing images on the display and it updates display
10. User see current state of the game on the display

## Structure of the project

```
.
├── Makefile
├── display_utils.c
├── font_prop14x16.c
├── font_rom8x16.c
├── game_utils.c
├── gui_utils.c
├── hardware_utils.c
├── include
│   ├── consts.h
│   ├── display_utils.h
│   ├── font_types.h
│   ├── game_utils.h
│   ├── gameobjects copy.h
│   ├── gameobjects.h
│   ├── gui_utils.h
│   ├── hardware_utils.h
│   ├── mzapo_parlcd.h
│   ├── mzapo_phys.h
│   ├── mzapo_regs.h
│   ├── serialize_lock.h
│   └── snake.h
├── my_file1.c
├── mzapo_parlcd.c
├── mzapo_phys.c
├── output.s
├── serialize_lock.c
├── snake.c
├── snakes_manager.c
└── structure.txt

1 directory, 28 files
```
