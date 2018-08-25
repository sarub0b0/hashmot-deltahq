#!/bin/sh

git filter-branch --tree-filter "rm -f $1" HEAD --all;
git reflog expire --expire=now --all;
git gc --aggressive --prune=now;

