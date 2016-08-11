#!/bin/bash

declare -r pattern=$1
declare -r value=$2

read -a files <<< $(ag -l $pattern)

for file in "${files[@]}"; do
  declare -a ocurrences;
   while read -r ocurrence; do
    ocurrences+=("$ocurrence")
  done < <(ag $pattern $file)

  for ocurrence in "${ocurrences[@]}"; do
    echo "$file:"
    line=$(echo "$ocurrence" | awk -F: '{ print $1 }' )
    content=$(echo "$ocurrence" | awk -F: '{ print $2 }' )
    echo "-${line}:${content}";
    echo "+${line}:$(echo $content | awk '{gsub("'$pattern'", "'$value'")}1')";
    read -r -p 'confirm? [y/n]' response;
    if [ "$response" = "y" ]; then
      sed -i '' "${line}s/${pattern}/${value}/g" $file
    fi
  done;
done;
