#!/bin/bash

file=debian/changelog
echo "teamgit ($1-1) intrepid; urgency=low" > $file
git shortlog >> $file
echo \ -- Abhijit Bhopatkar \(My launchpad gpg key\) \<bain@devslashzero.com\> `date -R` >> $file