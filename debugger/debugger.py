import py_debug_wrapper
import c_debug_wrapper
import termcolor


if __name__ == '__main__':
    c_d = c_debug_wrapper.run_debugger()
    py_d = py_debug_wrapper.run_debugger()
    num_steps = 0
    while True:
        num_steps += 1
        c = next(c_d)
        py = next(py_d)
        if c != py:
            print(f"Error at step {num_steps}:")
            # print hex values for easier debugging
            print(f"PC: {c['PC']:08X} | {py['PC']:08X}")
            # highlight differences
            for i in range(32):
                if c[i] != py[i]:
                    print(termcolor.colored(f"{i}: {c[i]:08X} | {py[i]:08X}", "red", force_color=True))
                else:
                    print(f"{i}: {c[i]:08X} | {py[i]:08X}")
            input()
        if num_steps % 1000 == 0:
            print(num_steps)
