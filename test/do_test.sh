#!/bin/bash

project_dir=$(dirname "$(realpath $0)")/..

obj=${project_dir}/build/tape_sort

test_folder=${project_dir}/test

answers_folder=${test_folder}/answers
rightans_folder=${test_folder}/right_answers

tests_folder=${test_folder}/tests

test_files_names=( "01" "02" "03" "04" "05" "06" "07" "08" "09" "10")

for i in ${!test_files_names[@]}; do
    echo ${test_files_names[$i]} diff:
    ${obj} ${tests_folder}/test_${test_files_names[$i]}.dat ${answers_folder}/ans_${test_files_names[$i]}.dat
    diff ${rightans_folder}/ans_${test_files_names[$i]}.dat ${answers_folder}/ans_${test_files_names[$i]}.dat
done

rm ${answers_folder}/*
