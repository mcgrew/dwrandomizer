#!/usr/bin/env python3

import argparse
import random
import sys

warps_addr = (0xf3d8, 0xf50b)
zones_addr = (0xf54f, 0xf5b3)
chest_addr = (0x5ddd, 0x5e59)
mp_req_addr = ()
enemy_stats_addr = (0x535b, )


def randomize(args):

	if not args.seed:
		args.seed = random.randint(0, sys.maxsize)
	print("Randomizing %s using random seed %d..." % (args.filename, args.seed))
	random.seed(args.seed)
	flags = ""
	prg = "PRG?"

	input_file = open(args.filename, 'rb')
	rom_data = bytearray(input_file.readall())

	if args.force:
		print("Verifying checksums...")
		result = verify_checksum(rom_data)
		if not result:
			print("Checksum does not match any known ROM, aborting.")
			print("Rerun with -f to force randomization.")
			sys.exit(-1)
	else:
		print("Skipping checksums...")

	enemy_stats = rom_data[slice(*enemy_stats_addr)]
	warp_data = rom_data[slice(*warp_addr)]
	chest_data = rom_data[slice(*chest_addr)]
	enemy_zones = rom_data[slice(*zones_addr)]
	mp_reqs = rom_data[slice(*mp_req_addr)]

	if args.chests:
		print("Shuffling chest contents...")
		flags += "c"
		chest_data = shuffle_chests(chest_data)
		rom_data[slice(*chest_addr)] = chest_data

	if args.towns:
		print("Shuffling town locations...")
		flags += "t"
		warp_data = shuffle_towns(warp_data)
		rom_data[slice(*enemy_stats_addr)] = enemy_stats

	if args.enemies:
		print("Randomizing enemy zones...")
		flags += "e"

	if args.patterns:
		print("Randomizing enemy attack patterns...")
		flags += "p"
		enemy_stats = randomize_patterns(enemy_stats)
		rom_data[slice(*enemy_stats_addr)] = enemy_stats

	if not args.nobuffdrops:
		print("Increasing XP/Gold drops to remake levels...")
		flags += "b"
		enemy_stats = buff_drops(enemy_stats)
		rom_data[slice(*enemy_stats_addr)] = enemy_stats

	if args.lowermp:
		print("Lowering MP requirements to remake levels...")
		flags += "m"
		mp_reqs = lower_mp_reqs(mp_reqs)
		rom_data[slice(*mp_req_addr)] = mp_reqs

	outputfilename = "Dragon Warrior.%s.%s.%d.nes" % (prg, flags, args.seed))
	print("Writing output file %s..." % outputfilename)
#	outputfile = open(outputfilename, 'wb')
#	outputfile.write(rom_data)

def verify_checksum(rom_data):
	return True

def randomize_chests(chest_data):
	"""
	Shuffles the contents of all chests in the game. Checks are used to ensure no
	quest items are located in Charlock chests.

	:Parameters:

	rtype: 
	return: 
	"""
	return chest_data

def randomize_attack_patterns(enemy_stats):
	"""
	Randomizes attack patterns of enemies (whether or not they use spells/fire 
	breath).

	:Parameters:
		enemy_stats : bytearray

	rtype: bytearray
	return: The new randomized enemy stats.
	"""
	return enemy_stats

def shuffle_towns(warp_data):
	"""
	Shuffles the locations of towns on the map.

	:Parameters:
		warp_data : bytearray
			The warp data read from the ROM.

	rtype: bytearray
	return: The shuffled warp_data
	"""
	return warp_data

def randomize_zones(zone_info):
	"""
	Randomizes which enemies are present in each zone.

	:Parameters:
		zone_info : bytearray
			The enemy zone info from the ROM.

	rtype: bytearray
	return: The newly randomized zone info
	"""
	return zone_info

def buff_drops(enemy_stats):
	"""
	Raises enemy XP and gold drops to those of the remakes.

	:Parameters:
		enemy_stats : bytearray
			The enemy stats read from the ROM

	rtype: bytearray
	return: The new enemy_stats after buffing drops.
	"""
	return enemy_stats

def lower_mp_reqs(mp_data):
	"""
	Lowers the MP requirements of spells to that of the remake

	:Parameters:
		mp_data : bytearray
			The MP data read from the ROM

	rtype: bytearray
	return: The new MP requirement data.
	"""
	return mp_data

def main():
	parser = argparse.ArgumentParser(prog="DWRandomizer")
	parser.add_argument("-b","--nobuffdrops", action="store_true",
			help="Do not bring enemy XP/Gold drops up to that of the remake. This "
					 "will make grind times much longer.")
	parser.add_argument("-c","--chests" action="store_false",
			help="Do not randomize chest contents.")
	parser.add_argument("-f","--force", action="store_false", 
			help="Skip checksums and force randomization. This may produce an invalid"
					 " ROM if the incorrect file is used.")
	parser.add_argument("-e","--enemies", action="store_false", 
			help="Do not randomize enemy zones.")
	parse.add_arguments("-m","--lowermp", action="store_true",
			help="Lower MP requirements of spells to that of the remakes.")
	parser.add_argument("-p","--patterns", action="store_false", 
			help="Do not randomize enemy attack patterns.")
	parser.add_argument("-s","--seed", type=int, 
			help="Specify a seed to be used for randomization.")
	parser.add_argument("-t","--towns", action="store_false", 
			help="Do not randomize towns.")
	parser.add_argument("filename", help="The rom file to use for input")
	args = parser.parse_args()
	randomize(args)


if __name__ == "__main__":
	main()
