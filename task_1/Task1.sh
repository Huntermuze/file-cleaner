#!/bin/bash

echo "Enter the dirty file to clean: "
read -r file_name
# If we want to sort based on 3rd character onwards, simply use the flag "-k1.3" for sort command.
time (grep -E "^[a-z]{3,15}$" "$file_name" | sort -u | shuf > "../data/clean_bash.txt")
echo "Successfully cleansed dirty file. Check the current directory for the cleaned file entitled \"clean_bash.txt\"."
