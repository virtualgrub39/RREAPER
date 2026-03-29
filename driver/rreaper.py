#!/usr/bin/env python3
"""
WARNING: MADE BY A LLM (claude 4.6) - USE ONLY FOR TESTING

rreaper.py - Host-side utility for the RREAPER EEPROM writer (ATmega328p)
Protocol: Intel HEX over serial UART, 8N1

Usage:
  rreaper.py <port> [--baud N] write  <address> <file> [--verify]
  rreaper.py <port> [--baud N] read   <address> <size> <file>
  rreaper.py <port> [--baud N] verify <address> <file>

Examples:
  rreaper.py /dev/ttyUSB0 write 0x0000 firmware.bin
  rreaper.py /dev/ttyUSB0 write 0x0000 firmware.bin --verify
  rreaper.py /dev/ttyACM0 --baud 115200 read 0x0000 256 dump.bin
  rreaper.py /dev/ttyUSB0 verify 0x0000 firmware.bin
"""

import argparse
import sys
import time
import serial  # pip install pyserial

# Must match #define DATA_BLOCK_SZ on the device
DATA_BLOCK_SZ = 16


# ---------------------------------------------------------------------------
# Intel HEX helpers
# ---------------------------------------------------------------------------

def ihex_checksum(byte_count: int, addr: int, rec_type: int, data: bytes) -> int:
    """
    Intel HEX two's complement checksum:
    least significant byte of the two's complement of the sum of all bytes.
    """
    total = byte_count + ((addr >> 8) & 0xFF) + (addr & 0xFF) + rec_type
    total += sum(data)
    return (~total + 1) & 0xFF


def make_ihex_record(addr: int, data: bytes) -> str:
    """Build a type-00 (data) Intel HEX record string, without newline."""
    bc = len(data)
    cs = ihex_checksum(bc, addr, 0x00, data)
    hex_data = ''.join(f'{b:02X}' for b in data)
    return f':{bc:02X}{addr:04X}00{hex_data}{cs:02X}'


def parse_ihex_record(line: str):
    """
    Parse an Intel HEX record line.
    Returns (byte_count, addr, rec_type, data, checksum).
    Raises ValueError on format or checksum errors.
    """
    line = line.strip()
    if not line.startswith(':'):
        raise ValueError(f"Not an Intel HEX record: {line!r}")
    s = line[1:]
    try:
        bc   = int(s[0:2],  16)
        addr = int(s[2:6],  16)
        rt   = int(s[6:8],  16)
        data = bytes(int(s[8 + i*2 : 10 + i*2], 16) for i in range(bc))
        cs   = int(s[8 + bc*2 : 10 + bc*2], 16)
    except (ValueError, IndexError) as exc:
        raise ValueError(f"Malformed Intel HEX record: {line!r}") from exc

    expected = ihex_checksum(bc, addr, rt, data)
    if cs != expected:
        raise ValueError(
            f"Checksum error at 0x{addr:04X}: "
            f"got {cs:02X}, expected {expected:02X}"
        )
    return bc, addr, rt, data, cs


# ---------------------------------------------------------------------------
# Progress bar
# ---------------------------------------------------------------------------

def _progress(done: int, total: int, width: int = 50) -> None:
    filled = int(width * done / total) if total else width
    bar    = '#' * filled + '-' * (width - filled)
    print(f"\r  [{bar}] {done}/{total} bytes ", end='', flush=True)


# ---------------------------------------------------------------------------
# Device commands
# ---------------------------------------------------------------------------

def probe_device(ser: serial.Serial) -> bool:
    """Send 'I' and look for the RREAPER banner in the response."""
    ser.write(b'I')
    deadline = time.monotonic() + 3.0
    while time.monotonic() < deadline:
        raw = ser.readline()
        line = raw.decode(errors='replace').strip()
        if 'RREAPER' in line:
            print(f"  Device: {line}")
            return True
    return False


def cmd_write(ser: serial.Serial, address: int, data: bytes) -> None:
    """
    Write binary data to EEPROM starting at address.
    Splits into DATA_BLOCK_SZ chunks and sends each as an Intel HEX record
    prefixed with 'W '.  Waits for 'OK' before sending the next chunk.
    """
    total  = len(data)
    offset = 0

    while offset < total:
        chunk  = data[offset : offset + DATA_BLOCK_SZ]
        record = make_ihex_record(address + offset, chunk)

        # Device waits for a ':' start-code after 'W', so the space is fine.
        ser.write(f'W {record}\r\n'.encode())

        # Wait for OK (or an error)
        while True:
            line = ser.readline().decode(errors='replace').strip()
            if not line:
                continue
            if 'OK' in line:
                break
            if line.startswith('ERR') or 'ERROR' in line.upper():
                raise RuntimeError(f"Device error at offset {offset}: {line!r}")

        offset += len(chunk)
        _progress(min(offset, total), total)

    print()  # newline after progress bar


def cmd_read(ser: serial.Serial, address: int, size: int, outfile: str) -> None:
    """
    Read size bytes from EEPROM starting at address.
    The device emits ceil(size/DATA_BLOCK_SZ) Intel HEX records.
    Decodes them, assembles a flat byte buffer, and writes it to outfile.
    """
    expected_records = (size + DATA_BLOCK_SZ - 1) // DATA_BLOCK_SZ

    # Address and size are sent as plain 4-digit hex words (ihex word format).
    ser.write(f'R {address:04X} {size:04X}\r\n'.encode())

    result       = bytearray(size)
    bytes_stored = 0
    records_got  = 0

    while records_got < expected_records:
        raw  = ser.readline()
        line = raw.decode(errors='replace').strip()

        if not line:
            continue

        if line.startswith(':'):
            bc, addr, rt, data, _ = parse_ihex_record(line)
            rel    = addr - address          # byte offset into result buffer
            actual = min(bc, size - rel)     # clamp to requested window
            result[rel : rel + actual] = data[:actual]
            bytes_stored += actual
            records_got  += 1
            _progress(bytes_stored, size)

        elif line.startswith('ERR') or 'ERROR' in line.upper():
            raise RuntimeError(f"Device error: {line!r}")

    print()  # newline after progress bar

    with open(outfile, 'wb') as f:
        f.write(result)
    print(f"  Saved {size} bytes → {outfile}")


class VerifyError(RuntimeError):
    """Raised when the device reports a verify mismatch."""
    def __init__(self, abs_addr: int, block_offset: int):
        self.abs_addr    = abs_addr        # absolute EEPROM address of mismatch
        self.block_offset = block_offset   # byte offset within the block
        super().__init__(
            f"Verify mismatch at EEPROM address 0x{abs_addr:04X} "
            f"(block offset {block_offset})"
        )


def _send_verify_record(ser: serial.Serial, address: int, chunk: bytes) -> None:
    """
    Send one 'V <ihex record>' to the device and wait for OK or E VERIFY:XX.
    Raises VerifyError on mismatch, RuntimeError on any other device error.
    """
    record = make_ihex_record(address, chunk)
    ser.write(f'V {record}\r\n'.encode())

    while True:
        line = ser.readline().decode(errors='replace').strip()
        if not line:
            continue
        if 'OK' in line:
            return
        if line.startswith('E VERIFY:'):
            # device sends the block-local byte offset as a hex byte
            try:
                block_offset = int(line[len('E VERIFY:'):], 16)
            except ValueError:
                block_offset = 0
            raise VerifyError(address + block_offset, block_offset)
        if line.startswith('ERR') or 'ERROR' in line.upper():
            raise RuntimeError(f"Device error during verify at 0x{address:04X}: {line!r}")


def cmd_verify(ser: serial.Serial, address: int, data: bytes) -> None:
    """
    Verify EEPROM contents against data starting at address.
    Sends each block as a 'V' record; the MCU reads back the EEPROM and
    compares on-device, replying OK or E VERIFY:<offset>.
    Raises VerifyError on first mismatch.
    """
    total  = len(data)
    offset = 0

    while offset < total:
        chunk = data[offset : offset + DATA_BLOCK_SZ]
        _send_verify_record(ser, address + offset, chunk)
        offset += len(chunk)
        _progress(min(offset, total), total)

    print()


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description='RREAPER host-side EEPROM utility (Intel HEX / UART)',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__)

    parser.add_argument('port',
        help='Serial device path, e.g. /dev/ttyUSB0 or /dev/ttyACM0')
    parser.add_argument('--baud', type=int, default=9600,
        help='Baud rate (default: 9600; must match BAUD in config.h)')
    parser.add_argument('--no-probe', action='store_true',
        help="Skip the 'I' device probe on startup")
    parser.add_argument('--timeout', type=float, default=5.0,
        help='Serial read timeout in seconds (default: 5)')

    sub = parser.add_subparsers(dest='command', required=True)

    # -- write subcommand --
    wp = sub.add_parser('write', help='Write a binary file to EEPROM')
    wp.add_argument('address', type=lambda x: int(x, 0),
        help='Start address, hex (0x0000) or decimal')
    wp.add_argument('file', help='Binary input file')
    wp.add_argument('--verify', action='store_true',
        help='Verify each block against EEPROM contents after writing')

    # -- read subcommand --
    rp = sub.add_parser('read', help='Read EEPROM region into a binary file')
    rp.add_argument('address', type=lambda x: int(x, 0),
        help='Start address, hex (0x0000) or decimal')
    rp.add_argument('size', type=lambda x: int(x, 0),
        help='Number of bytes to read, hex (0x100) or decimal')
    rp.add_argument('file', help='Binary output file')

    # -- verify subcommand --
    vp = sub.add_parser('verify', help='Verify EEPROM contents against a binary file')
    vp.add_argument('address', type=lambda x: int(x, 0),
        help='Start address, hex (0x0000) or decimal')
    vp.add_argument('file', help='Binary file to verify against')

    args = parser.parse_args()

    # -- open serial port --
    print(f"Opening {args.port} at {args.baud} baud...")
    try:
        ser = serial.Serial(args.port, args.baud, timeout=args.timeout)
    except serial.SerialException as exc:
        sys.exit(f"Error opening serial port: {exc}")

    # Many AVR boards reset when DTR is toggled on connect; give them time.
    time.sleep(2)
    ser.reset_input_buffer()

    # -- probe --
    if not args.no_probe:
        print("Probing device...")
        if not probe_device(ser):
            print("Warning: no RREAPER banner received (continuing anyway)",
                  file=sys.stderr)

    # -- dispatch --
    try:
        if args.command == 'write':
            try:
                with open(args.file, 'rb') as fh:
                    data = fh.read()
            except OSError as exc:
                sys.exit(f"Cannot read input file: {exc}")

            if not data:
                sys.exit("Error: input file is empty")

            n_records = (len(data) + DATA_BLOCK_SZ - 1) // DATA_BLOCK_SZ
            print(f"Writing {len(data)} bytes to 0x{args.address:04X} "
                  f"({n_records} records)...")
            cmd_write(ser, args.address, data)
            print("Write complete.")

            if args.verify:
                print(f"Verifying {len(data)} bytes at 0x{args.address:04X}...")
                cmd_verify(ser, args.address, data)
                print("Verify OK.")

        elif args.command == 'read':
            print(f"Reading {args.size} bytes from 0x{args.address:04X}...")
            cmd_read(ser, args.address, args.size, args.file)
            print("Read complete.")

        elif args.command == 'verify':
            try:
                with open(args.file, 'rb') as fh:
                    data = fh.read()
            except OSError as exc:
                sys.exit(f"Cannot read input file: {exc}")

            if not data:
                sys.exit("Error: input file is empty")

            print(f"Verifying {len(data)} bytes at 0x{args.address:04X}...")
            cmd_verify(ser, args.address, data)
            print("Verify OK.")

    except VerifyError as exc:
        print()
        sys.exit(f"VERIFY FAILED: {exc}")
    except RuntimeError as exc:
        print()
        sys.exit(f"Error: {exc}")
    except KeyboardInterrupt:
        print("\nInterrupted.", file=sys.stderr)
        sys.exit(1)
    finally:
        ser.close()


if __name__ == '__main__':
    main()
