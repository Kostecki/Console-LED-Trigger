import csv
import os

# Paths related to project root
input_file = "shared/colors.csv"
output_file = "firmware/include/colors.h"

csv_path = os.path.join(input_file)
header_path = os.path.join(output_file)

with open(csv_path, newline="") as csvfile:
    reader = csv.reader(csvfile)
    lines = []

    # skip header
    next(reader, None)

    for row in reader:
        r, g, b = map(int, row[:3])
        name = row[3] if len(row) > 3 else ""
        comment = f" // {name}" if name else ""
        lines.append(f"    Adafruit_NeoPixel::Color({r}, {g}, {b}),{comment}")

header = [
    "#pragma once",
    "#include <Adafruit_NeoPixel.h>",
    "",
    "const uint32_t colors[] = {",
    *lines,
    "};",
    "",
    "constexpr uint8_t NUM_COLORS = sizeof(colors) / sizeof(colors[0]);",
]

with open(header_path, "w") as header_file:
    header_file.write("\n".join(header))

print(f"Generated colors.h with {len(lines)} entries.")
