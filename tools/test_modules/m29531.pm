#!/usr/bin/env perl

##
## Author......: See docs/credits.txt
## License.....: MIT
##

use strict;
use warnings;
use FindBin qw($Bin);
use File::Basename qw(basename);

# Derive module id from this file's name: m<id>.pm
my $MODULE_ID = do {
  my $base = basename(__FILE__);
  $base =~ /^m(\d+)\.pm$/ or die "Cannot parse module id from filename '$base'";
  $1;
};

sub module_constraints { [[0, 256], [0, 256], [-1, -1], [-1, -1], [-1, -1]] }

sub module_generate_hash
{
  my $word = shift;
  my $salt = shift;

  my $script = "$Bin/test_modules/luks1.sh";

  # Use derived module id instead of hardcoded number:
  open my $fh, "-|", $script, $MODULE_ID, $word
    or die "exec $script: $!";

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
