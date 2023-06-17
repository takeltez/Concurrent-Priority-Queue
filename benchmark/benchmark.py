import ctypes
import os
import datetime
import sys, string, os
import subprocess

from ctypes import *

class threads_times(ctypes.Structure):
	_fields_ = [('time', ctypes.c_double*9)]

class Benchmark:
	def __init__(self, bench_function, xrange, basedir, name):
		self.bench_function = bench_function
		self.xrange = xrange
		self.basedir = basedir
		self.name = name
		self.data = {}

	def run(self):
		result = self.bench_function()

		i=0
		for x in self.xrange:
			self.data[x] = result.time[i]
			i+=1

	def write_avg_data(self):
		with open(f"{self.basedir}/../data/{self.name}.data", "w")\
				as datafile:
			datafile.write(f"x y\n")

			for x, box in self.data.items():
				datafile.write(f"{x} {box}\n")

if __name__ == "__main__":

	basedir = os.path.dirname(os.path.abspath(__file__))
	binary = ctypes.CDLL(f"{basedir}/../lib/queue.so")

	num_threads = [1,8,16,24,32,40,48,56,64]

	bench_list = [binary.set_mixed_bench_50_del,
					binary.set_mixed_bench_80_del,
					binary.set_mixed_bench_20_del,
					binary.set_delete_only_bench,
					binary.set_insert_only_bench]

	for i in range(len(bench_list)):
		bench_list[i].restype = threads_times
		smallbench = Benchmark(bench_list[i], num_threads, basedir, "smallbench"+str(i+1))
		smallbench.run()
		smallbench.write_avg_data()
