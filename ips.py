#!/usr/bin/env python3

import struct
import argparse

def create_ips(file1_content, file2_content):
  return Patch.create(file1_content, file2_content).encode()

def apply_ips(file_content, patch_content):
  patch = Patch(patch_content)
  return patch.apply(file_content)

class Patch:
  records = [] 
  def __init__(self, ips_content=None):
    if ips_content and ips_content[:5] == b'PATCH' and ips_content[-3:] == b'EOF':
      # trim 'PATCH' from the beginning and 'EOF' from the end.
      ips_ptr = 0
      ips_content = ips_content[5:-3]
      # parse the patches
      while (ips_ptr < len(ips_content)):
        record_meta = struct.unpack_from(">BHH", ips_content, ips_ptr)
        ips_ptr += 5
        record_addr = record_meta[0] << 16 | record_meta[1]
        record_size = record_meta[2]
        if record_size:
          record_content = struct.unpack_from("B" * record_size, ips_content, 
              ips_ptr)
          ips_ptr += record_size
          self.records.append(Record(record_addr, record_content))
        else: #run length encoded
          record_size = struct.unpack_from(">H", ips_content, ips_ptr)[0]
          ips_ptr += 2
          record_content = struct.unpack_from("B", ips_content, ips_ptr)[0]
          ips_ptr += 1
          self.records.append(Record(record_addr, record_content, record_size))
    
  def apply(self, orig_content):
    if not isinstance(orig_content, bytearray):
      orig_content = bytearray(orig_content)
    for record in self.records:
      record.apply(orig_content)
    return orig_content

  def encode(self):
    encoded =  b''.join([r.encode() for r in self.records])
    return b''.join((b'PATCH', encoded, b'EOF'))

  def add_record(self, address, content):
    for r in self.records:
      if r.address == address:
        r.set_content(content)
        return
    self.records.append(Record(address, content))

  def clear(self):
    self.records = []

  def combine(self, patch):
    self.records = self.records + patch.records

  @staticmethod
  def create(orig_content, patched_content):
    p = Patch()
    if not len(orig_content) <= len(patched_content):
      raise ValueError("Original file is larger than patched file.")
    diff_start = -1
    diff_end = -1
    for i in range(len(patched_content)):
      if i >= len(orig_content) or not orig_content[i] == patched_content[i]:
        if diff_start < 0:
          diff_start = i
        diff_end = i
      if diff_end >= 0 and (i - diff_end >= 5 or i == len(patched_content) - 1):
        p.add_record(diff_start, patched_content[diff_start:diff_end+1])
        diff_start = -1
        diff_end = -1

    return p

class Record:
  def __init__(self, address, content=None, rle_size=None):
    self.address = address 
    self.rle_size = rle_size #RLE records only
    if content is not None:
      self.set_content(content)

  def set_addr(self, addr):
    self.address = addr

  def set_content(self, content):
    try:
      if len(content) > 1 and self.rle_size:
        raise ValueError("RLE records may only contain one byte of content, "
          "%d bytes provided" % len(content))
      self.content = bytearray(content)
    except TypeError:
      self.content = bytearray((content,))

  def size(self):
    if self.rle_size:
      return self.rle_size
    if self.content:
      return len(self.content)
    else:
      return 0

  def encode(self):
    if self.rle_size: #RLE record
      return struct.pack('>BHHHB', self.address >> 16, self.address & 0xffff, 
        self.rle_size, int(self.content[0]))
    return struct.pack('>BHH' + 'B' * self.size(), self.address >> 16, 
        self.address & 0xffff, self.size(), *[int(b) for b in self.content])

  def apply(self, orig_content):
    size = self.size()
    if self.rle_size:
      orig_content[self.address:self.address+size] = self.content * size
    if size:
      orig_content[self.address:self.address+size] = self.content
    
 
def main():
  parser = argparse.ArgumentParser(prog="ips",
      description="A utility for creating and appying IPS patches")
  parser.add_argument("-o","--output", type=str,
      help="The file name to be written.")
  parser.add_argument("file1", help="The first input file")
  parser.add_argument("file2", help="The second input file")
  args = parser.parse_args()

  patch_content = None

  file1 = open(args.file1, 'rb')
  file1_content = file1.read()
  file1.close()
  file2 = open(args.file2, 'rb')
  file2_content = file2.read()
  file2.close()

  if file1_content[:5] == b'PATCH':
    patch_content, patch_name = file1_content, args.file1
    file_content, file_name = file2_content, args.file2
  elif file2_content[:5] == b'PATCH':
    patch_content, patch_name = file2_content, args.file2
    file_content, file_name = file1_content, args.file1
  
  if patch_content:
    out = apply_ips(file_content, patch_content)
    if not args.output:
      try:
        patch_name_without_ext = patch_name[:patch_name.rindex('.')]
      except ValueError:
        patch_name_without_ext = patch_name
      try:
        ext = file_name[file_name.rindex('.'):]
      except ValueError:
        ext = '.patched'
      args.output = patch_name_without_ext + ext
  else:
    out = create_ips(file1_content, file2_content)
    if not args.output:
      args.output = args.file2 + '.ips'
  
  outfile = open(args.output, 'wb')
  outfile.write(out)
  outfile.close()


if __name__ == "__main__":
  main()
