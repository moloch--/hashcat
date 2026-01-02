#!/usr/bin/env perl

##
## Author......: See docs/credits.txt
## License.....: MIT
##

use strict;
use warnings;

sub module_constraints { [[0, 256], [0, 256], [-1, -1], [-1, -1], [-1, -1]] }

sub module_generate_hash
{
  my $word = shift;
  my $salt = shift;

  # Non-interpolating heredoc: Perl will NOT touch $… or backslashes inside
  my $python_code = <<'PYCODE';
#!/usr/bin/env python3
import hashlib, hmac, random, os, sys

# password comes from argv[1] (Perl passes $word as the first arg)
password = sys.argv[1]
masterkey = password.encode()

keyfile = os.urandom(32) if random.randint(0, 1) else b""
masterseed = os.urandom(32)
transformseed = os.urandom(32)
t = random.randint(1, 8)                # iterations
m = (1 << random.randint(12, 18))       # KiB
p = random.randint(1, 8)                # parallelism
header = os.urandom(253)

# Step 1
h1 = hashlib.sha256(masterkey).digest()
argon_in = hashlib.sha256(h1 + keyfile).digest()

try:
    from argon2.low_level import Type as Argon2Type, hash_secret_raw
except Exception as e:
    print("ERROR: Requires 'argon2-cffi' (pip install argon2-cffi)", file=sys.stderr)
    raise

# Step 2
a2_type = [Argon2Type.D, Argon2Type.ID][random.randint(0, 1)]
argon_out = hash_secret_raw(
    secret=argon_in,
    salt=transformseed,
    time_cost=t,
    memory_cost=m,
    parallelism=p,
    hash_len=32,
    type=a2_type,
    version=19
)

# Step 3
final = hashlib.sha512(masterseed + argon_out + b"\x01").digest()
# Step 4
out = hashlib.sha512(b"\xff"*8 + final).digest()
# Step 5
header_hmac = hmac.new(out, header, hashlib.sha256).digest()

uuid_map = { Argon2Type.D: "ef636ddf", Argon2Type.ID: "9e298b19" }
argon_uuid = uuid_map[a2_type]
m = m * 1024  # bytes instead of KiB

print(f"$keepass$*4*{t}*{argon_uuid}*{m}*19*{p}*{masterseed.hex()}*{transformseed.hex()}*{header.hex()}*{header_hmac.hex()}", end="")
if len(keyfile) == 32:
    print(f"*1*64*{keyfile.hex()}", end="")
print()
PYCODE

  # Run python reading program from stdin; pass $word as argv[1]
  my $digest = do {
    # qx here-doc to avoid shell-quoting pitfalls
    local $ENV{PYTHONUTF8} = 1; # optional: force UTF-8 mode
    qx{python3 - "$word" <<'PY'
$python_code
PY
};
  };

  $digest =~ s/[\r\n]//g;

  return $digest;
}

sub module_verify_hash
{
  my $line = shift;

  my ($hash, $salt, $word) = split (':', $line);

  return unless defined $hash;
  return unless defined $salt;
  return unless defined $word;

  my $word_packed = pack_if_HEX_notation ($word);

  my $new_hash = module_generate_hash ($word_packed, $salt);

  return ($new_hash, $word);
}

1;
