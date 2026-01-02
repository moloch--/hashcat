#!/usr/bin/env perl

##
## Author......: See docs/credits.txt
## License.....: MIT
##

use strict;
use warnings;

use Digest::SHA qw (sha256 sha256_hex);

sub module_constraints { [[0, 256], [64, 64], [-1, -1], [-1, -1], [-1, -1]] }

sub module_generate_hash
{
  my $word = shift;
  my $salt = shift;

  my $salt_bin = pack("H*", $salt);

  my $digest = sha256 ($salt_bin . $word);

  for (my $i = 0; $i < 128; $i++)
  {
    $digest = sha256 ($digest);
  }

  my $hash = sprintf ('%s%s', unpack("H*", $digest), $salt);

  return $hash;
}

sub module_verify_hash
{
  my $line = shift;

  my ($hash_str, $word) = split (':', $line);

  my $hash_len = int(length($hash_str) / 2);

  return unless defined $word;
  return unless $hash_len == 64;

  my $hash = substr ($hash_str, 0, $hash_len);
  my $salt = substr ($hash_str, $hash_len);

  return unless defined $hash;
  return unless defined $salt;

  my $word_packed = pack_if_HEX_notation ($word);

  my $new_hash = module_generate_hash ($word_packed, $salt);

  return ($new_hash, $word);
}

1;
