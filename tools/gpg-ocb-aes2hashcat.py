#!/usr/bin/env python3

# extract hashes from private-keys-v1.d/*.key files to use with hashcat mode 17050

import sys, binascii
from pathlib import Path

def skip_ws(buf, pos):
    while pos < len(buf) and buf[pos] in b" \t\r\n":
        pos += 1
    return pos

def read_quoted(buf, pos):
    assert buf[pos:pos+1] == b'"'
    end = pos + 1
    while end < len(buf) and buf[end:end+1] != b'"':
        end += 1
    if end >= len(buf):
        raise ValueError("Unterminated quoted string")
    return buf[pos+1:end], end+1

def read_hash_hex(buf, pos):
    # Reads #...# and strips any whitespace inside
    assert buf[pos:pos+1] == b'#'
    end = pos + 1
    while end < len(buf) and buf[end:end+1] != b'#':
        end += 1
    if end >= len(buf):
        raise ValueError("Unterminated #hex# block")
    raw = buf[pos+1:end]
    raw = b"".join(raw.split())  # collapse whitespace just in case
    try:
        return binascii.unhexlify(raw), end+1
    except binascii.Error as e:
        raise ValueError(f"Bad hex inside #...#: {e}")

def extract_block(buf, start):
    # Returns the balanced S-expression starting at '('
    depth = 0
    for i in range(start, len(buf)):
        c = buf[i:i+1]
        if c == b'(':
            depth += 1
        elif c == b')':
            depth -= 1
            if depth == 0:
                return buf[start:i+1], i+1
    raise ValueError("Unbalanced parentheses")

def parse_gpg(path: str):
    data = Path(path).read_bytes()

    # Find all "(protected" blocks and pick the one that contains "openpgp-s2k3-ocb-aes"
    i = 0
    target = None
    while True:
        j = data.find(b"(protected", i)
        if j < 0:
            break
        block, nxt = extract_block(data, j)
        if b"openpgp-s2k3-ocb-aes" in block and b"(sha1" in block:
            target = block
            break
        i = nxt
    if target is None:
        raise SystemExit("Could not find a (protected openpgp-s2k3-ocb-aes …) block")

    # Work on a whitespace-relaxed view for simpler scanning
    flat = b" ".join(target.split())

    # Walk from the marker
    pos = flat.find(b"openpgp-s2k3-ocb-aes")
    if pos < 0:
        raise SystemExit("Marker not found after extraction")

    # Seek to "(sha1"
    pos = flat.find(b"(sha1", pos)
    if pos < 0:
        raise SystemExit("sha1 block not found")
    pos += len(b"(sha1")
    pos = skip_ws(flat, pos)

    # salt: either "…8 bytes…" or #…16 hex…#
    if flat[pos:pos+1] == b'"':
        salt_b, pos = read_quoted(flat, pos)
    elif flat[pos:pos+1] == b'#':
        salt_b, pos = read_hash_hex(flat, pos)
    else:
        raise SystemExit("Unexpected salt token")
    if len(salt_b) not in (8, 16):
        raise SystemExit(f"Unexpected salt length: {len(salt_b)}")

    pos = skip_ws(flat, pos)

    # iterations: digits, sometimes quoted
    if flat[pos:pos+1] == b'"':
        it_s, pos = read_quoted(flat, pos)
    else:
        start = pos
        while pos < len(flat) and flat[pos:pos+1] not in b") ":
            pos += 1
        it_s = flat[start:pos]
    iters = int(it_s)

    # Skip to end of (sha1 …)
    while pos < len(flat) and flat[pos:pos+1] != b')':
        pos += 1
    pos += 1
    pos = skip_ws(flat, pos)

    # nonce: #…# or "…", 12 bytes expected
    if flat[pos:pos+1] == b'#':
        nonce_b, pos = read_hash_hex(flat, pos)
    else:
        nonce_b, pos = read_quoted(flat, pos)
    if len(nonce_b) != 12:
        raise SystemExit(f"Unexpected nonce length: {len(nonce_b)}")

    pos = skip_ws(flat, pos)
    if flat[pos:pos+1] == b')':  # inner close
        pos += 1
    pos = skip_ws(flat, pos)

    # ciphertext: usually #…hex…# and may contain whitespace inside the #...#
    if flat[pos:pos+1] == b'#':
        ct_b, pos = read_hash_hex(flat, pos)
    else:
        ct_b, pos = read_quoted(flat, pos)
    if len(ct_b) < 16:
        raise SystemExit("Ciphertext too short")

    # John format for private-keys-v1 + S2K3(SHA1) + OCB-AES
    modulus_size=4096 #TODO dynamically change this..
    cipher_mode=7 #AES with 128-bit key(sym 7)
    return f"$gpg$*1*{len(ct_b)}*{modulus_size}*{ct_b.hex()}*1*254*2*{cipher_mode}*{len(nonce_b)}*{nonce_b.hex()}*{iters}*{salt_b.hex()}"

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} /path/to/private-keys-v1-file", file=sys.stderr)
        sys.exit(1)
    print(parse_gpg(sys.argv[1]))
