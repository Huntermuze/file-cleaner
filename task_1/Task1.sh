#!/bin/bash


echo "Enter the dirty file to clean: "
read -r file_name
time (grep -E "^[a-z]{3,15}$" "$file_name" | sort -u -k1.3 | shuf > "clean.txt")
echo "Successfully cleansed dirty file. Check the current directory for the cleaned file entitled \"clean.txt\"."
