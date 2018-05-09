import sys

input_lines = []
with open(sys.argv[1], mode = "r") as kitty_file:
    for line in kitty_file.readlines():
        line = line.replace("\n", "")
        if len(line) > 0:
            input_lines.append(line.replace("\n", ""))

print("char COMMANDS[][Sizes::string_length] = {")
for line in input_lines:
    print("    \"%s\"," % (line))
print("};")
