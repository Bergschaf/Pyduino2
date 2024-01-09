import py_debug_wrapper
import c_debug_wrapper


if __name__ == '__main__':
    c_d = c_debug_wrapper.run_debugger()
    py_d = py_debug_wrapper.run_debugger()
    num_steps = 0
    while True:
        num_steps += 1
        c = next(c_d)
        py = next(py_d)
        if c != py:
            print("ERROR")
            print(c)
            print(py)
            break
        if num_steps % 1000 == 0:
            print(num_steps)
