#!/usr/bin/env python3

import struct
import argparse

def create_ips(file1_content, file2_content):
  """
  Creates a new Patch object based on the differences between file1 and file2

  :Parameters:
    file1_content : bytearray
      Contents of the first file
    file2_content : bytearray
      Contents of the second file

  rtype: Patch
  return: The newly created Patch object
  """
  return Patch.create(file1_content, file2_content).encode()

def apply_ips(file_content, patch_content):
  """
  Applied a patch to the given file

  :Parameters:
    file_content : bytearray
      The content of the original file
    patch_content : bytearray
      The content of the ips file

  rtype: bytearray
  return: The patched content of the binary file
  """
  patch = Patch(patch_content)
  return patch.apply(file_content)

class Patch:
  """
  A class for creating ips patch files
  """
  def __init__(self, ips_content=None):
    """
    Creates a new patch object
    
    :Parameters:
      ips_content : bytearray
        Optional. The contents of the ips patch to use for initialization.
        Creates an empty patch if omitted.
    """
    self.records = []
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
    """
    Applies this patch to the given content

    :Parameters:
      orig_content : bytearray
        The original file content this patch is being applied to.

    rtype: bytearray
    return: The patched content
    """
    if not isinstance(orig_content, bytearray):
      orig_content = bytearray(orig_content)
    for record in self.records:
      record.apply(orig_content)
    return orig_content

  def encode(self):
    """
    Encodes the Patch into ips format.

    rtype: bytearray
    return: The content of the new ips file.
    """
    encoded =  b''.join([r.encode() for r in self.records])
    return b''.join((b'PATCH', encoded, b'EOF'))

  def add_record(self, address, content, rle_size=None):
    """
    Adds a new patch record to this Patch.

    :Parameters:
      address : int
        The address where the content is to be applied.
      content : array or int
        The content to be added to this patch. This should be an array of bytes
        or ints unless this is a RLE record, in which case this should be a
        single int or byte.
      rle_size : int
        Optional, only for run length encoded records. If creating an RLE
        record, content should be a single int, and this indicates the length.
    """
    for r in self.records:
      if r.address == address:
        r.set_content(content)
        return
    self.records.append(Record(address, content, rle_size))

  def add_records(self, patchdict):
    """
    Adds multiple patch records.

    :Parameters:
      patchdict : dict
        A dictionary indexed by the address of each patch record, containing
        the content for each patch. This cannot be used to create RLE records.
    """
    for addr,value in patchdict.items():
      self.add_record(addr, value)

  def clear(self):
    """
    Clears all records from this patch.
    """
    self.records = []

  def combine(self, patch):
    """
    Combines this patch with another.

    :Parameters:
    patch : Patch
      The Patch to be combined with this one.
    """
    self.records = self.records + patch.records

  def clear(self):
    self.records = []

  def combine(self, patch):
    self.records = self.records + patch.records

  @staticmethod
  def create(orig_content, patched_content):
    """
    Creates a new patch from the content of 2 files.

    :Parameters:
      orig_content : bytearray
        The content of the original file.
      patched_content : bytearray
        The content of the modified file.

    rtype: Patch
    return: The newly created Patch
    """
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
  """
  A class for holding information about a Patch record.
  """
  def __init__(self, address, content=None, rle_size=None):
    """
    Creates a new patch record.
    """
    self.address = address 
    self.rle_size = rle_size #RLE records only
    if content is not None:
      self.set_content(content)

  def set_addr(self, addr):
    """
    Sets a new address for this Record.
    :Parameters:
      addr : int
        The new address for this record.
    """
    self.address = addr

  def set_content(self, content):
    """
    Sets the content for this record

    :Parameters:
      content : bytearray
        The new content for this record.
    """
    try:
      if len(content) > 1 and self.rle_size:
        raise ValueError("RLE records may only contain one byte of content, "
          "%d bytes provided" % len(content))
      self.content = bytearray(content)
    except TypeError:
      self.content = bytearray((content,))

  def size(self):
    """
    Returns the size of this record in bytes.

    rtype: int
    return: The size of the record in bytes.
    """
    if self.rle_size:
      return self.rle_size
    if self.content:
      return len(self.content)
    else:
      return 0

  def encode(self):
    """
    Encodes this Record into ips format.

    rtype: bytearray
    return: The ips-encoded format for this record.
    """
    if self.rle_size: #RLE record
      return struct.pack('>BHHHB', self.address >> 16, self.address & 0xffff, 0,
        self.rle_size, int(self.content[0]))
    return struct.pack('>BHH' + 'B' * self.size(), self.address >> 16, 
        self.address & 0xffff, self.size(), *[int(b) for b in self.content])

  def apply(self, orig_content):
    """
    Applies this record to the content of a binary file.

    :Parameters:
      orig_content : bytearray
        The original content of the file.
    """
    size = self.size()
    if self.rle_size:
      orig_content[self.address:self.address+size] = self.content * size
    elif size:
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
