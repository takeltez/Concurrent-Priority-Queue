import ctypes
import os
import datetime
import sys, string, os
import subprocess

from ctypes import *

class return_values(ctypes.Structure):
	_fields_ = [('time', ctypes.c_double*9)]

class Benchmark:
	'''
	Class representing a benchmark. It assumes any benchmark sweeps over some
	parameter xrange using the fixed set of inputs for every point. It provides
	two ways of averaging over the given amount of repetitions:
	- represent everything in a boxplot, or
	- average over the results.
	'''
	def __init__(self, bench_function, xrange, basedir, name):
		self.bench_function = bench_function
		self.xrange = xrange
		self.basedir = basedir
		self.name = name
		self.data = {}

	def run(self):
		'''
		Runs the benchmark with the given parameters. Collects
		repetitions_per_point data points and writes them back to the data
		dictionary to be processed later.
		'''
		result = self.bench_function()

		i=0
		for x in self.xrange:
			self.data[x] =result.time[i]
			i+=1

	def write_avg_data(self):
		'''
		Writes averages for each point measured into a dataset in the data
		folder timestamped when the run was started.
		'''
		with open(f"{self.basedir}/../data/{self.name}.data", "w")\
				as datafile:
			datafile.write(f"x y\n")

			for x, box in self.data.items():
				datafile.write(f"{x} {box}\n")

if __name__ == "__main__":

	basedir = os.path.dirname(os.path.abspath(__file__))
	binary = ctypes.CDLL( f"./lib/queue.so" )

	num_threads = [1,8,16,24,32,40,48,56,64]

	list_functions = [binary.set_mixed_bench_50_del, 
						binary.set_mixed_bench_80_del, 
						binary.set_mixed_bench_20_del, 
						binary.set_delete_only_bench, 
						binary.set_insert_only_bench]

	for i in range(len(list_functions)):
		list_functions[i].restype = return_values
		smallbench = Benchmark(list_functions[i], num_threads, basedir, "smallbench"+str(i+1))
		smallbench.run()
		smallbench.write_avg_data()
