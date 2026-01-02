#!/usr/bin/env perl

##
## Author......: See docs/credits.txt
## License.....: MIT
##

use strict;
use warnings;

use Digest::SHA qw (sha1_hex);
use Encode;

sub module_constraints { [[0, 256], [1, 10], [0, 16], [1, 10], [-1, -1]] }

sub as400_ssha1
{
  my ($username, $password) = @_;

  $username = substr ($username . " " x 10, 0, 10);

  my $salt_utf16be = encode ("UTF-16BE", uc $username);
  my $word_utf16be = encode ("UTF-16BE", $password);

  my $digest = sha1_hex ($salt_utf16be . $word_utf16be);

  return $digest;
}

sub module_generate_hash
{
  my $word = shift;
  my $salt = shift;

  my $hash_buf = as400_ssha1 (uc $salt, $word);

  my $hash = sprintf ('$as400$ssha1$*%s*%s', uc $salt, uc $hash_buf);

  return $hash;
}

sub module_verify_hash
{
  my $line = shift;

  my @line_elements = split (":", $line);

  return if scalar @line_elements < 2;

  my $hash_in = shift @line_elements;

  my $word = join (":", @line_elements);

  # check signature

  my @hash_elements = split ('\*', $hash_in);

  return unless ($hash_elements[0] eq '$as400$ssha1$');

  my $salt = $hash_elements[1];

  return unless defined $salt;
  return unless defined $word;

  $word = pack_if_HEX_notation ($word);

  my $new_hash = module_generate_hash ($word, $salt);

  return ($new_hash, $word);
}

1;
