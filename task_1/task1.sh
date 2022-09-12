#!/bin/bash

echo "Enter the dirty file to clean: "
read -r file_name
# If we want to sort based shuffle the output, simply pipe the output to the shuf command.
time (grep -E "^[a-z]{3,15}$" "$file_name" | sort -k1.3 | uniq -u >"../data/task1_bash_sorted_list.txt")
echo "Successfully cleansed dirty file. Check the current directory for the cleaned file entitled \"task1_bash_sorted_list.txt\"."
