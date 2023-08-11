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

src_dir="1805052-part2"
test_cases=$(ls -d test-cases-part2/*/)

# interate over all the folders in the current directory (test-cases
# folder

for folder in $test_cases; do
    case_no=$(echo "$folder" | grep -o '[0-9]\+')
    print_blue "Running test case $case_no"
    if [ -d "$folder" ]; then
        # List all file names in the folder and store them in an array
        file_list=("$folder"/*)

        # cd to the src directory
        cd $src_dir
        g++ main.cpp -o main && ./main ../$folder/scene.txt ../$folder/config.txt
        rm ./main
        cd ..

        # boolean variable for success
        success=true
        
        for s in 1 2 3; do
            if cmp --silent -- <(tr -d '\r' < $src_dir/stage$s.txt) <(tr -d '\r' < $folder/stage$s.txt); then
                print_checkpoint "stage$s matched!"
            else
                # diff <(tr -d '\r' <1805052/stage1.txt) <(tr -d '\r' < $folder/stage1.txt)
                print_red_cross "Test $case_no failed on stage$s"
                success=false
                break
            fi 
        done

        # check z_buffer.txt file matches or not
        # Diff with -b
        DIFF_Z=$(diff -b $src_dir/z_buffer.txt $folder/z_buffer.txt)
        if [ "$DIFF_Z" != "" ] 
        then
            print_red_cross "Test $case_no failed on z_buffer.txt"
            success=false
        else
            print_checkpoint "z_buffer.txt matched!"
        fi

        # check out.bmp file matches or not using diff command 
        DIFF=$(diff $src_dir/out.bmp $folder/out.bmp)
        if [ "$DIFF" != "" ] 
        then
            print_red_cross "Test $case_no failed on out.bmp"
            success=false
        else
            print_checkpoint "out.bmp matched!"
        fi

        if($success); then
            print_green_tick "Test $case_no passed!"
        fi

        # delete stage files in 1805052 folder
        rm $src_dir/stage*.txt
        rm $src_dir/z_buffer.txt
        rm $src_dir/out.bmp

        # print newline
        echo ""

    else
        echo "Error: Folder not found or not a valid folder."
    fi
done



# file_list=("$folder_path"/*)

# diff <(tr -d '\r' <1805052/stage1.txt) <(tr -d '\r' <test-cases/4/stage1.txt)
# diff <(sed -n l ../1805052/stage1.txt) <(sed -n l file2)

