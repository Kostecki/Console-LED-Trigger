import csv, os, textwrap

input_file = "shared/colors.csv"
hdr_path = "firmware/include/colors.h"
cpp_path = "firmware/src/colors.cpp"

# Read colors from CSV
rows = []
with open(input_file, newline="") as f:
    reader = csv.reader(f)
    next(reader, None)  # skip header
    for row in reader:
        r, g, b = map(int, row[:3])
        name = row[3] if len(row) > 3 else ""
        comment = f" // {name}" if name else ""
        rows.append(f"    Adafruit_NeoPixel::Color({r}, {g}, {b}),{comment}")

# Header: declarations only
hdr = textwrap.dedent(
    """\
    #pragma once
    #include <stdint.h>

    extern const uint32_t colors[];
    extern const uint8_t  NUM_COLORS;
    """
)

# CPP: single definition with array contents
cpp = "\n".join(
    [
        "#include <Adafruit_NeoPixel.h>",
        '#include "colors.h"',
        "",
        "const uint32_t colors[] = {",
        *rows,
        "};",
        "",
        "const uint8_t NUM_COLORS = sizeof(colors) / sizeof(colors[0]);",
        "",
    ]
)

os.makedirs(os.path.dirname(hdr_path), exist_ok=True)
os.makedirs(os.path.dirname(cpp_path), exist_ok=True)
with open(hdr_path, "w") as f:
    f.write(hdr)
with open(cpp_path, "w") as f:
    f.write(cpp)

print(f"Wrote {hdr_path} and {cpp_path} with {len(rows)} colors.")
