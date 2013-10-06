#!/bin/bash

# gather all runtime stats

for i in `ls output/job_*.out`; do
	echo "$i"
	cat "$i" | grep "Operations has been executed"
done
