#!/usr/bin/perl -w

# Abstract: Convert a PCI ID file to a C header file.
# Author: Per Lundberg <plundis@chaosdev.org>

use strict;

my $INFILE;
my $OUTFILE;
my $infile_name = "pci.ids";
my $outfile_name = "pci-id.c";

open (INFILE, '<' . $infile_name) or die "Couldn't open $infile_name for reading.\n";
open (OUTFILE, '>' . $outfile_name) or die "Couldn't poen $outfile_name for writing.\n";

# Start in the outmost place.

my $mode = 0;

print OUTFILE '/* pci-id.c - Automatically generated from pci.pl. DON\'T EDIT! */

#include "pci.h"

pci_device_id_type pci_device_id[] = 
{
';

my $manufacturer;
my $manufacturer_id;
my %manufacturer = ();

# Abstract: Escape "s and

sub escape
{
  my $line = shift;
  
  $line =~ s/\"/\\\"/g;
  $line =~ s/\?/\\\?/g;
  return $line;
}

while (<INFILE>)
{
  my $line = $_;

  chop ($line);

  # Ignore comments and white lines.

  next if (substr ($line, 0, 1) eq '#');
  next if ($line =~ /^\s*$/);

  if ($line =~ /^\t[^\t]+$/)
  {
    (my $device_id, my $device_name) = ($line =~ /^\t([\da-fA-F]{4})\s+(.+)$/);
    
    $device_name = escape ($device_name);

    print (OUTFILE "  { 0x$manufacturer_id, 0x$device_id, \"$device_name\" },\n");

  }
  elsif ($line =~ /^\t\t[^\t]/)
  {
    # FIXME: Implement.
  }

  # Device class. FIXME: Implement.

  elsif ($line =~ /C \d+.*/)
  {
    last;
  }
  else
  {
    ($manufacturer_id, $manufacturer) = ($line =~ /^([\da-fA-F]{4})\s+(.+)$/);
    $manufacturer{$manufacturer_id} = escape ($manufacturer);
    unless ($manufacturer)  { print $line; exit 1; }
  }
}

# Iterate through the hash with manufacturers.

print OUTFILE "  { 0x0000, 0x0000, NULL }
};

pci_vendor_id_type pci_vendor_id[] =
{
";

foreach my $key (keys %manufacturer)
{
  print (OUTFILE "  { 0x$key, \"$manufacturer{$key}\" },\n");
}

print OUTFILE '  { 0x0000, NULL }
};
';

close (OUTFILE);
close (INFILE);
