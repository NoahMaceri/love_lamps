import pathlib
import platform
import sys
import ctypes
import crccheck

if platform.system() != 'Linux':
	print('This script is only for Linux')
	sys.exit(1)

bin_path = pathlib.Path.cwd().parent / '.pio' / 'build' / 'esp32dev' / 'firmware.bin'
out_path = pathlib.Path.cwd().parent / '.pio' / 'build' / 'esp32dev' / 'firmware.lzss'
ota_path = pathlib.Path.cwd().parent / '.pio' / 'build' / 'esp32dev' / 'firmware.ota'

# Check if the firmware.bin exists
if not bin_path.exists():
	print('Firmware not found')
	sys.exit(1)

lzss_functions = ctypes.CDLL(str(pathlib.Path.cwd() / 'lzss.so'))

lzss_functions.encode_file.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lzss_functions.encode_file(str(bin_path).encode('utf-8'), str(out_path).encode('utf-8'))

if not out_path.exists():
	print('LZSS file not created')
	sys.exit(1)
print('LZSS file created')

# Read the binary file
in_file = open(out_path, "rb")
bin_data = bytearray(in_file.read())
in_file.close()

magic_number = 0x45535033.to_bytes(4,byteorder='little')
# Version field (byte array of size 8) - all 0 except the compression flag set.
version = bytearray([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40])

# Prepend magic number and version field to payload
bin_data_complete = magic_number + version + bin_data

# Calculate length and CRC32
bin_data_len = len(bin_data_complete)
bin_data_crc = crccheck.crc.Crc32.calc(bin_data_complete)

# Write to outfile
out_file = open(ota_path, "wb")
out_file.write((bin_data_len).to_bytes(4,byteorder='little'))
out_file.write((bin_data_crc).to_bytes(4,byteorder='little'))
out_file.write(bin_data_complete)
out_file.close()

if not ota_path.exists():
	print('OTA file not created')
	sys.exit(1)

print('OTA file created')
