#!/bin/csh -f

if ($argv[1] != "") then
  echo "deleting $argv[1]"
  rm -f $argv[1]
endif