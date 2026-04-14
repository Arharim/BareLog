#!/usr/bin/env python3
"""BareLog Flash Decoder - reads BareLog flash log sector from a binary dump.

Usage:
    python blog_flash_decode.py <binary_file> [--base-addr 0x0800F000]

Reads the raw flash sector written by blog_flash backend and prints
the decoded log messages as text.
"""

import argparse
import sys


def decode_flash(data: bytes) -> str:
    end = len(data)
    for i in range(len(data) - 1, -1, -1):
        if data[i] != 0xFF:
            end = i + 1
            break
    else:
        return ""

    text = data[:end]
    return text.decode("ascii", errors="replace")


def main() -> None:
    parser = argparse.ArgumentParser(description="Decode BareLog flash sector")
    parser.add_argument("file", help="Binary dump file path")
    parser.add_argument(
        "--base-addr",
        type=lambda x: int(x, 0),
        default=0x0800F000,
        help="Flash sector base address (default: 0x0800F000)",
    )
    parser.add_argument(
        "--size",
        type=lambda x: int(x, 0),
        default=1024,
        help="Flash sector size in bytes (default: 1024)",
    )
    args = parser.parse_args()

    try:
        with open(args.file, "rb") as f:
            raw = f.read()
    except FileNotFoundError:
        print(f"Error: file not found: {args.file}", file=sys.stderr)
        sys.exit(1)

    offset = 0
    if len(raw) > args.base_addr:
        offset = args.base_addr

    if offset + args.size <= len(raw):
        sector = raw[offset : offset + args.size]
    else:
        sector = raw[offset:] if offset < len(raw) else raw[: args.size]

    decoded = decode_flash(sector)
    if decoded:
        print(decoded, end="")
    else:
        print("No log data found in flash sector.", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
