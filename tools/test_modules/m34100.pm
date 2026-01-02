#!/usr/bin/env perl

##
## Author......: See docs/credits.txt
## License.....: MIT
##

use strict;
use warnings;
use FindBin qw($Bin);

sub module_constraints { [[0, 256], [0, 256], [-1, -1], [-1, -1], [-1, -1]] }

sub module_generate_hash
{
  my $word = shift;
  my $salt = shift;

  my $script = "$Bin/test_modules/m34100.sh";
  # Capture stdout of the script safely (no shell):
  open my $fh, "-|", $script, $word or die "exec $script: $!";
  local $/;
  my $digest = <$fh>;
  close $fh;

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
