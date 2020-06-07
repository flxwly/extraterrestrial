import glob
import logging
import subprocess
import sys
import time
from datetime import datetime, timedelta

from watchdog.events import PatternMatchingEventHandler
from watchdog.observers import Observer

is_strip = False
more_warnings = True

auto_update = False

optimisation_level = 3


def compile_code():
    code_path = "./code/"
    out_path = "./"

    file_list = glob.glob(code_path + "*.cpp", recursive=False)

    command = "\"" + "g++" + "\"" + " -shared -static "
    if 0.5 < optimisation_level < 3.5:
        print("Using optimisation level: " + str(optimisation_level))
        command += "-O" + str(int(optimisation_level)) + " " + \
                   "--param max-inline-insns-single=1000 "
    if more_warnings:
        command += "-Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization " \
                   "-Wendif-labels -Winit-self -Winline -Wlogical-op -Wmissing-include-dirs " \
                   "-Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wpacked -Wpointer-arith " \
                   "-Wredundant-decls -Wshadow -Wsign-promo -Wswitch-default -Wswitch-enum " \
                   "-Wunsafe-loop-optimizations -Wvariadic-macros -Wwrite-strings"

    # -Wfloat-equal
    for file_path in file_list:
        print(file_path)
        command += " " + file_path
    command += " -o " + "\"" + out_path + "extraterrestrial.dll\""

    # command = command + " & REM 2> errors.txt strip --strip-unneeded \"" + out_path + "extraterrestrial.dll\" pause"]

    if is_strip:
        command += " &  strip --strip-unneeded \"" + out_path + "extraterrestrial.dll\""
    print("compiling...")
    subprocess.call(command, shell=True)
    print("finished")
    sys.stdout.flush()


class Event(PatternMatchingEventHandler):

    def __init__(self, patterns=None):
        super(Event, self).__init__(patterns=patterns)
        self.last_modified = datetime.now()

    def on_any_event(self, event):
        if datetime.now() - self.last_modified > timedelta(seconds=5):
            compile_code()
            self.last_modified = datetime.now()


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s - %(message)s',
                        datefmt='%Y-%m-%d %H:%M:%S')
    path = sys.argv[1] if len(sys.argv) > 1 else '.'
    event_handler = Event(patterns=["*.cpp", "*.hpp"])
    compile_code()

    if auto_update:
        observer = Observer()
        observer.schedule(event_handler, path, recursive=True)
        observer.start()

        try:
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            observer.stop()
        observer.join()
