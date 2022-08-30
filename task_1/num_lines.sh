#!/bin/bash

echo "Enter the clean file to inspect: "
read -r file_name
num_lines=$(wc -l "$file_name")
echo "Number of lines: $num_lines"
