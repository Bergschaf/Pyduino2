py_memory = "py_memory"
c_memory = "c_memory"

f1 = open(py_memory, "r")
f2 = open(c_memory, "r")

f1 = f1.readlines()
f2 = f2.readlines()
print(len(f1), len(f2))
print(hex(len(f1)), hex(len(f2)))
for i in range(len(f1)-1000,0,-1):
    if f1[i] != f2[i]:
        print(f"Error at line {i} | {hex(i)}:")
        print(f"py: {f1[i]}")
        print(f"c : {f2[i]}")
        input()