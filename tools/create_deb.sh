#!/bin/bash

git archive  --format=tar --prefix=teamgit-$1/ v$1 | gzip > ../teamgit_$1.orig.tar.gz
tools/write-changelog.sh $1
debuild -S
