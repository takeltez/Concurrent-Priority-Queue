# Concurrent Priority Queue

## About
Implementation of **High Performance Lock-Free Priority Queue** based on

*Anastasia Braginsky, Nachshon Cohen, Erez Petrank:<br>
CBPQ: High Performance Lock-Free Priority Queue.<br>
Euro-Par 2016: 460-474*

## Description

### Pre-requirements
- GNU Compiler: `gcc`
- GNU Make: `make`
- Python version at least 3.6: `python3`
- OpenMP: `libomp-dev`
- pdflatex: `texlive-latex-base` and `texlive-latex-extra`
- SLURM Scheduler: 	`slurm`
- srun: `slurm-client`

### Attention
- In `Makefile` at **line 27** specify your own partition using by `srun` command.<br>
- Run target `all` before any other target.<br>
- Run target `small-bench` before `small-plot` and `report` targets.

### Makefile targets

- `all`<br>
Build executable and shared library files `bin/queue` and `lib/queue.so` respectively.

- `small-bench`<br>
Execute short representative benchmark using `benchmark/benchmark.py` and collect data in `data` directory.

- `small-plot`<br>
Generate `plot.pdf` in `plots` directory.

- `report`<br>
Generate `report.pdf` in `report` direcroty.

- `zip`<br>
Archive all necessary files and directories.

- `clean`<br>
Clean all utility files and directories.
