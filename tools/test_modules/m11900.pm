#!/usr/bin/env perl

##
## Author......: See docs/credits.txt
## License.....: MIT
##

use strict;
use warnings;

use Crypt::PBKDF2;
use MIME::Base64 qw (encode_base64 decode_base64);
use Digest::HMAC qw(hmac);
use Digest::MD5 qw(md5);

sub module_constraints { [[0, 256], [1, 15], [-1, -1], [-1, -1], [-1, -1]] }

sub pbkdf2_md5
{
  my ($password, $salt, $iterations, $key_length) = @_;
  $iterations  ||= 1000;
  $key_length  ||= 32;

  my $hash_length = 16;  # MD5 outputs 16 bytes
  my $block_count = int( ($key_length + $hash_length - 1) / $hash_length );

  my $output = '';

  for my $i (1 .. $block_count)
  {
    # pack N = big endian 32-bit
    my $block_index = pack('N', $i);

    # Initial U1 = HMAC(password, salt || block_index)
    my $u = hmac($salt . $block_index, $password, \&md5);

    my $t = $u;

    for (my $j = 1; $j < $iterations; $j++)
    {
      $u = hmac($u, $password, \&md5);
      $t ^= $u;
    }

    $output .= $t;
  }

  return substr($output, 0, $key_length);
}

sub module_generate_hash
{
  my $word       = shift;
  my $salt       = shift;
  my $iterations = shift // 1000;
  my $out_len    = shift // 32;

  # Generate derived key (binary)
  my $derived_key = pbkdf2_md5($word, $salt, $iterations, $out_len);

  # base64 encode salt and derived key
  my $base64_salt = encode_base64($salt, '');
  my $base64_key  = encode_base64($derived_key, '');

  # Format output string
  return sprintf("md5:%d:%s:%s", $iterations, $base64_salt, $base64_key);
}

sub module_verify_hash
{
  my $line = shift;

  my ($digest, $word) = split (/:([^:]+)$/, $line);

  return unless defined $digest;
  return unless defined $word;

  my @data = split (':', $digest);

  return unless scalar (@data) == 4;

  my $signature = shift @data;

  return unless ($signature eq 'md5');

  my $iterations = int (shift @data);

  my $salt = decode_base64 (shift @data);
  my $hash = decode_base64 (shift @data);

  my $out_len = length ($hash);

  my $word_packed = pack_if_HEX_notation ($word);

  my $new_hash = module_generate_hash ($word_packed, $salt, $iterations, $out_len);

  return ($new_hash, $word);
}

1;
