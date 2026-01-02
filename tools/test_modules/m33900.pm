#!/usr/bin/env perl

##
## Author......: See docs/credits.txt
## License.....: MIT
##

use strict;
use warnings;

use Crypt::PBKDF2;
use MIME::Base64 qw (encode_base64 decode_base64);

sub module_constraints { [[0, 256], [64, 64], [-1, -1], [-1, -1], [-1, -1]] }

sub module_generate_hash
{
  my $word = shift;
  my $salt = shift;

  my $pbkdf2 = Crypt::PBKDF2->new
  (
    hasher     => Crypt::PBKDF2->hasher_from_algorithm ('HMACSHA2', 256),
    iterations => 2500,
    output_len => 32
  );

  my $salt_bin = pack ("H*", $salt);

  my $digest = $pbkdf2->PBKDF2 ($salt_bin, $word);

  my $hash = sprintf ("5%s%s", $salt, unpack ("H*", $digest));

  return $hash;
}

sub module_verify_hash
{
  my $line = shift;

  my ($hash, $word) = split (':', $line);

  return unless defined $hash;
  return unless defined $word;

  my $signature = substr ($hash, 0, 1);

  return unless $signature eq '5';

  my $salt = substr ($hash, 1, 64);

  my $word_packed = pack_if_HEX_notation ($word);

  my $new_hash = module_generate_hash ($word_packed, $salt);

  return ($new_hash, $word);
}

1;
