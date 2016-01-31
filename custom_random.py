#!/usr/bin/env python3

import time
import math

__seed = int(time.time())

def seed(x):
	global __seed
	__seed = x
	
def __next():
	global __seed
	x = math.sin(__seed)
	__seed += 1
	return x % 1.0
	
def uniform(a, b):
	return a + (b - a) * __next()
	
def randint(a, b):
	return a + math.floor((b - a + 1) * __next())
	
def choice(list):
	return list[math.floor(len(list) * __next())]
	
def shuffle(list):
	a, b = list[:], []
	while len(a):
		x = math.floor(len(a) * __next())
		b.append(a[x])
		del a[x]
		
	for i,x in enumerate(b):
		list[i] = x
	return list
