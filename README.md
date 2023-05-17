# Concurrent Priority Queue

## How to use

### On Host machine
Pre-requirements:
- GCC Compiler
- GNU Make
- Python version at least 3.6
- OpenMP: `libomp-dev`
- pdflatex: `texlive-latex-base` and `texlive-latex-extra`

### Makefile targets

**Note:**<br>
Run target `all` before any other target!<br>
Run target `small-bench ` before `small-plot` and `report` targets!

- `all`<br>
Build executable and shared library files `bin/queue` and `lib/queue.so` respectively.

- `small-bench`<br>
Execute short representative benchmark `bench/becnmark.py` and collect data in `data` directory.

- `small-plot`<br>
Generate `plot.pdf` in `plots` directory.

- `report`<br>
Generate `report.pdf` in `report` direcroty.

- `zip`<br>
Archive all necessary files and directories.

- `clean`<br>
Clean all utility files and directories.
