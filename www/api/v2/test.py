#!/usr/bin/env python3

import math
import random

# CGI Headers
print("Content-Type: text/html\r")
print("\r")
print("<html><head><title>Python3 ASCII Circle</title></head><body>")
print("<pre>")  # Start preformatted text

# Mountain parameters
width = 80   # Characters per line
height = 20  # Number of rows

# Generate mountain range using random heights
peaks = [random.randint(5, height - 5) for _ in range(10)]  # Random peak heights
spacing = width // len(peaks)  # Distance between peaks

# Generate landscape
for y in range(height, -1, -1):
    line = ""
    for x in range(width):
        # Calculate which peak we're near
        peak_index = x // spacing
        if peak_index < len(peaks) and y <= peaks[peak_index] - abs(x % spacing - spacing // 2):
            line += "M"  # Mountain peak
        elif y == 0:
            line += "_"  # Ground
        else:
            line += " "  # Sky
    print(line)

print("</pre>")
print("</body></html>")