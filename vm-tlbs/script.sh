declare -a arr=(2 4 8 16 48 64 160 256 640 1024 2560 4096)

for i in {1000000..5000000..1000000}; do
    for j in "${arr[@]}"; do
        #if [[ $(( $i % 4 )) = 0 ]]; then
        #fi
        ./tlb $j $i >> "output_$i"
    done
    echo "Completed $i"
done
