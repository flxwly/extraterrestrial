import glob
import logging
import subprocess
from datetime import datetime, timedelta
from os import system

import sys
import time
from watchdog.events import PatternMatchingEventHandler
from watchdog.observers import Observer

is_strip = False

# SFML = " -I\"%SFML32_HOME%/include\" -L\"%SFML32_HOME%/lib\" -lsfml-graphics -lsfml-window -lsfml-system"
SFML = " -I\"%SFML32_HOME%/include\" -L\"%SFML32_HOME%/lib\" -lsfml-graphics -lsfml-window -lsfml-system"


def compile_code(out_path, file_name, optimisation_level, more_warnings):
    system('cls')
    code_path = "./code/"

    file_list = glob.glob(code_path + "**/*.*", recursive=True)

    command = "g++" + " -shared -static "

    if 0.5 < optimisation_level < 3.5:
        print("Using optimisation level: " + str(optimisation_level))
        command += "-O" + str(int(optimisation_level)) + " " + \
                   "--param max-inline-insns-single=1000 "
    if more_warnings:
        command += "-Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization " \
                   "-Wendif-labels -Winit-self -Winline -Wlogical-op -Wmissing-include-dirs " \
                   "-Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wpacked -Wpointer-arith " \
                   "-Wredundant-decls -Wshadow -Wsign-promo -Wswitch-default -Wswitch-enum " \
                   "-Wvariadic-macros -Wwrite-strings"

    # -Wfloat-equal
    for file_path in file_list:
        print(file_path)
        command += " " + file_path
    command += " -o " + "\"" + out_path + file_name + "\""  # + SFML

    print("\n" + command + "\n")
    subprocess.run(command, shell=True)
    print("\nOutput: " + str(out_path) + file_name)
    sys.stdout.flush()


class Event(PatternMatchingEventHandler):

    def __init__(self, command_line_args, patterns=None):
        super(Event, self).__init__(patterns=patterns)
        self.last_modified = datetime.now()
        self.command_line_args = command_line_args

    def on_any_event(self, event):
        if datetime.now() - self.last_modified > timedelta(seconds=5):
            system('cls')
            compile_code(
                self.command_line_args[0],
                self.command_line_args[1],
                self.command_line_args[2],
                self.command_line_args[3])
            self.last_modified = datetime.now()


def handle_arguments(command_line_args):
    out_path = "./"
    file_name = "ai.dll"
    auto_update = True if "-A" in sys.argv else False
    more_warnings = True if "-W" in sys.argv else False
    optimisation_level = 0

    if "-o" in command_line_args:
        i = command_line_args.index("-o") + 1
        if len(command_line_args) <= i:
            print("No out path found. Please provide an out path after using -o")
            exit()
        else:
            out_path = command_line_args[i]
    else:
        print("Please provide a out path. Use -o [out path]")

    if "-f" in command_line_args:
        i = command_line_args.index("-f") + 1
        if len(command_line_args) <= i:
            print("No filename found. Please provide a filename after using -f")
            exit()
        else:
            file_name = command_line_args[i]
    else:
        print("Please provide a filename. Use -f [Filename]")

    if "-O" in command_line_args:
        i = command_line_args.index("-O") + 1
        if len(command_line_args) <= i:
            print("No optimisation level found. Please provide an optimisation level after using -O")
            exit()
        else:
            try:
                optimisation_level = int(command_line_args[i])
            except ValueError:
                pass


    return [out_path, file_name, optimisation_level, more_warnings, auto_update]


if __name__ == "__main__":

    args = handle_arguments(sys.argv)

    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s - %(message)s',
                        datefmt='%Y-%m-%d %H:%M:%S')
    event_handler = Event(command_line_args=args, patterns=["*.cpp", "*.hpp"])

    if args[4]:
        observer = Observer()
        observer.schedule(event_handler, ".", recursive=True)
        observer.start()

        try:
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            observer.stop()
        observer.join()
    else:
        compile_code(
            args[0],
            args[1],
            args[2],
            args[3])
