import subprocess
import threading
import time
import queue

c_cwd = "/home/bergschaf/PycharmProjects/Pyduino2/riscv_emulator/"
debug_file = c_cwd + "debug"

def check_compiled():
    subprocess.run(["make", "debug"], cwd=c_cwd)

def get_value(s):
    return int(s.decode("utf-8")[:-1].split(":")[1], 16)
def run_debugger():
    check_compiled()
    # run the debug executable in a new thread and get the output while it runs
    p = subprocess.Popen(["./debug"],stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE, cwd=c_cwd)
    # read the output while it runs
    p.stdout.readline()
    p.stdout.readline()
    out = []
    while True:
        out.append(p.stdout.readline())
        if out[-1] == b'end\n':
            res = {}
            res["PC"] = get_value(out[0])
            out = out[1:-1]
            for i in range(32):
                res[i] = get_value(out[i])
            yield res

            out = []
            # write n to stdin to continue
            p.stdin.write(b"n")
            p.stdin.flush()





