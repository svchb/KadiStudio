#!/usr/bin/perl
##
## Copyright 2022 Karlsruhe Institute of Technology
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##

use strict;
my ($count,$i,$j,@lines,$line,$modified_line,$newlinewritten,$quality_filename);

my $template = <<'END_MESSAGE';
{
  "description": "'ERROR' old: 'OLD' new: 'NEW'",
  "fingerprint": "FINGERPRINT",
  "severity": "SEVERITY",
  "location": {
    "path": "PATH",
    "lines": {
      "begin": LINE
    }
  }
}
END_MESSAGE


sub printQuality { #
  my ($fh, $line, $modified_line, $file, $linenumber, $count, $type) = @_;

  my $replace = $template;
  $replace =~s/ERROR/$type/g;
  $replace =~s/SEVERITY/major/g;

  my $escape = $line;
  $escape =~s/"/\\\"/g;
  $escape =~s/\t/\\t/g;
  chomp($escape);
  $replace =~s/OLD/$escape/g;

  my $escape = $modified_line;
  $escape =~s/"/\\\"/g;
  $escape =~s/\t/\\t/g;
  chomp($escape);
  $replace =~s/NEW/$escape/g;

  my $md5 = `echo "$file $escape" | md5sum`;
  $md5 = substr($md5,0,32);
  $replace =~s/FINGERPRINT/$md5/g;

  $file = substr($file, 2);
  $replace =~s/PATH/$file/g;
  $replace =~s/LINE/$linenumber/g;

  #print  $replace; ## "asd\n {\"description\": \"old:", $line, "new:" , $modified_line, "\", \" \"}" ;
  if(0 < $count) {
    print {$fh} ",";
  }
  print {$fh} $replace;
}

for (@ARGV) {
  if (-l $_) {
    print "skipping file link: $_\n";
    next;
  }
  if (-d $_) {
    print "skipping directory: $_\n";
    next;
  }
  if ($_ =~ /_gen.[ch]$/) {
    print "skipping generated file: $_\n";
    next;
  }
  print "cleaning: $_\n";

  if (defined $ENV{'QUALITY_JSON'}) {
    $quality_filename = "quality.json";
  } else {
    $quality_filename = "/dev/null";
  }

  if (-e $quality_filename and -s $quality_filename) {
    my $fsize = -s $quality_filename;
    # print $fsize."\n";
    open(QUALITY_FILE, "+<", $quality_filename) or die $!;
    seek QUALITY_FILE, $fsize-1, 0; # or 0 (numeric) instead of SEEK_SET
    if ($fsize > 3) {
      $count=1;
    } else {
      $count=0;
    }

  } else {
    open(QUALITY_FILE, ">", $quality_filename) or die "Could not open file $_ ($!)\n";
    print QUALITY_FILE "[";
    $count=0;
  }

  ## open the file in reading mode and read it line by line
  if (open(FILE,"$_") == 0) {
    print STDERR "Could not open file $_ ($!)\n";
  } else {
    @lines=<FILE>;
    close(FILE);
    ## store empty lines and print them only when in the middle of the file
    $j=0;
    $newlinewritten="false";
    my $found = 0;

    ## open the file in overwrite mode
    open(FILE,">$_") || die("Could not write to file $_ ($!)");
    for ($i=0;$i<@lines;$i++) {
      my $replace = $template;
      $line=$lines[$i];
      $modified_line = $line;
      ## remove CR
      $modified_line =~s/[\r]+//g;
      ## replace tabs by two whitespaces
      $modified_line =~s/[\t]/  /g;
      ## remove trailing whitespaces
      $modified_line =~s/[ ]+$//;

      ## replace common formating errors
      $modified_line=~s/ if\(/ if \(/g;
      $modified_line=~s/ for\(/ for \(/g;
      $modified_line=~s/\}while/\} while/g;
      $modified_line=~s/ while\(/ while \(/g;
      $modified_line=~s/ switch\(/ switch \(/g;
      $modified_line=~s/\}else/} else/g;
      $modified_line=~s/ else\{/ else \{/g;
      $modified_line=~s/if (?x)(\( ( [^()]++ | (?1) )* \))\{/if \1 \{/g;
      $modified_line=~s/for (?x)(\( ( [^()]++ | (?1) )* \))\{/for \1 \{/g;
      $modified_line=~s/switch (?x)(\( ( [^()]++ | (?1) )* \))\{/switch \1 \{/g;
      $modified_line=~s/while (?x)(\( ( [^()]++ | (?1) )* \))\{/while \1 \{/g;

      if ($modified_line ne $line) {
        printQuality(\*QUALITY_FILE, $line, $modified_line, $_, $i, $count, "Formatting error");
        $line = $modified_line;
        $count++;
      }

      ## replace mutated vowels
#       $line=~s/ä/ae/g;
#       $line=~s/Ä/Ae/g;
#       $line=~s/ö/oe/g;
#       $line=~s/Ö/Oe/g;
#       $line=~s/ü/ue/g;
#       $line=~s/Ü/Ue/g;
#       $line=~s/ß/ss/g;

      ## print the changed line into the file
      if ( length($line) == 1 and substr($line, 0, 1) eq "\n" ) {
        ## remember how many empty lines there are
        $j++;
      } elsif ( length($line) > 0 ) {
        ## print the stored empty lines, if we find a new not empty line
        for (;$j>0;$j--) {
          print FILE "\n";
        }

        if ( $_ =~ /.h$/ and (($line =~ /#[ ]*ifndef .*_[hH].*$/) or ($line =~ /#[ ]*pragma[ ]*once$/))) {
          if ($line =~ /#[ ]*ifndef .*_[hH].*$/) {
            if ($found != 0) {
              print FILE $lines[$i];
              $found++;
            } else {
              ## check for header guard
              $i++;
              if ($i>=@lines) { die("Reached end of header file ",$_); }
              $line=$lines[$i];
              if (not $line =~ /#[ ]*define .*_[hH].*$/) {
                ## something went wrong write back what we already picked up
                print FILE $lines[$i-1];
                print FILE $line;
              } else {
                my $newfilename = $_;
                $newfilename =~ s/^\.//g;        # cut off .
                $newfilename =~ s/^\///g;        # cut off /
                $newfilename =~ s/[\/\.\-]/_/g;    # replace / and . with _
                $newfilename = uc($newfilename); # to upper case
                print FILE "#ifndef ", $newfilename, "\n#define ", $newfilename, "\n";
                $found++;
              }
            }
          } elsif ($line =~ /#[ ]*pragma[ ]*once$/) {
            print FILE $line;
            $found++;
          }
        } else {
          ## "normal" line
          print FILE "$line";

          $line=~s/.*$//;
          if ( length($line) == 1 ) {
            $newlinewritten="true";
          } else {
            $newlinewritten="false";
          }
        }
      }
    }

    ## if the file does not end with a newline, add it
    if ( $newlinewritten eq "false" ) {
      print FILE "\n";
      printQuality(\*QUALITY_FILE, $line, $modified_line, $_, $i, $count, "Missing new line at end of file");
      $count++;
    }

    if ($_ =~ /.h$/ and $found != 1) { # check if header file was well formed
      my $msg;
      if ($found == 0) {
        $msg = "No header guard found";
      } else {
        $msg = "header guard found $found times";
      }
      print STDERR "Header file ", $_, " has a malformed header ($msg).\n";
      printQuality(\*QUALITY_FILE, "", "", $_, 1, $count, "Header file is malformed $(msg).");
      $count++;
    }
    if ($j > 0) {
      printQuality(\*QUALITY_FILE, $j, 1, $_, $i, $count, "Too many empty lines at end of file");
      $count++;
    }

    close(FILE);
    print QUALITY_FILE "]";
    close(QUALITY_FILE);
  }
}
