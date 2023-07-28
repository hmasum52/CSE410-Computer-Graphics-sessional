#!/bin/bash

print_blue() {
    echo -e "\e[34m$1\e[0m"
}

# Function to print a message with a green tick mark
print_green_tick() {
    echo -e "\e[32m\u2713 $1\e[0m"
}

print_red_cross() {
    echo -e "\e[31m\u2717 $1\e[0m"
}

# Function to print a warning message with a yellow warning icon
print_warning() {
    echo -e "\e[33m⚠ $1\e[0m"
}

# Function to print a checkpoint message with an icon
print_checkpoint() {
    local icon="🔍"  # Unicode character for the magnifying glass icon
    echo -e "\e[1;33m$icon CHECKPOINT: $1\e[0m"
}


test_cases=$(ls -d test-cases/*/)

# interate over all the folders in the current directory (test-cases
# folder

for folder in $test_cases; do
    case_no=$(echo "$folder" | grep -o '[0-9]\+')
    print_blue "Running test case $case_no"
    if [ -d "$folder" ]; then
        # List all file names in the folder and store them in an array
        file_list=("$folder"/*)

        cd 1805052/
        g++ main.cpp -o main && ./main ../$folder/scene.txt
        rm ./main
        cd ..
        if cmp --silent -- <(tr -d '\r' < 1805052/stage1.txt) <(tr -d '\r' < $folder/stage1.txt); then
            print_checkpoint "stage1 matched!"
        else
            echo "files differ"
            # diff <(tr -d '\r' <1805052/stage1.txt) <(tr -d '\r' < $folder/stage1.txt)
            print_red_cross "Test $case_no failed on stage1"
            coninue
        fi  

        print_green_tick "Test $case_no passed!"

        # print newline
        echo ""

    else
        echo "Error: Folder not found or not a valid folder."
    fi
done



# file_list=("$folder_path"/*)

# diff <(tr -d '\r' <1805052/stage1.txt) <(tr -d '\r' <test-cases/4/stage1.txt)
# diff <(sed -n l ../1805052/stage1.txt) <(sed -n l file2)

