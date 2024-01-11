import subprocess
import threading
import time
import queue

c_cwd = "/home/bergschaf/PycharmProjects/Pyduino2/riscv_emulator/"
debug_file = c_cwd + "debug_elfLoader"

def check_compiled():
    subprocess.run(["make", "debug_elfLoader"], cwd=c_cwd)

def get_value(s):
    return int(s.decode("utf-8")[:-1].split(":")[1], 16)
def run_debugger():
    check_compiled()
    # run the debug executable in a new thread and get the output while it runs
    p = subprocess.Popen(["./debug_elfLoader"],stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE, cwd=c_cwd)
    # read the output while it runs
    p.stdout.readline()
    p.stdout.readline()
    while True:
        print(p.stdout.readline())

if __name__ == '__main__':
    run_debugger()



