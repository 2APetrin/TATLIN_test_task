#!/bin/bash

project_dir=$(dirname "$(realpath $0)")/..

obj=${project_dir}/build/tape_sort

test_folder=${project_dir}/test

answers_folder=${test_folder}/answers
rightans_folder=${test_folder}/right_answers

tests_folder=${test_folder}/tests

test_files_names=( "01" "02" "03" "04" "05" "06" "07" "08")

for i in ${!test_files_names[@]}; do
    ${obj} ${tests_folder}/test_${test_files_names[$i]}.dat ${answers_folder}/ans_$i.dat
    echo $i diff:
    diff ${rightans_folder}/ans_${test_files_names[$i]}.dat ${answers_folder}/ans_$i.dat
done
